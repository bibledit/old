/*
** Copyright (©) 2003-2009 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/

#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "bibletime.h"
#include "bible.h"
#include "shell.h"
#include "books.h"
#include "directories.h"
#include "sqlite_reader.h"
#include <sqlite3.h>
#include "ipc.h"
#include "tiny_utilities.h"

#define STAGE_ZERO 0
#define STAGE_COMMUNICATE 1
#define STAGE_WAIT_RETRY 1000
#define STAGE_RETRY 1200

#define BIBLETIME "bibletime"

BibleTime::BibleTime(bool dummy)
/*
This organizes the communication with BibleTime. It uses KDE's DCOP protocol.
It centralizes all communication with BibleTime so that all happens synchronized,
and two different commands, occurring at the same time, will not disturb each
other.
*/
{
  getmodules();
}

BibleTime::~BibleTime()
{
  // Shut down bibledit-bibletime.
  vector < ustring > payload;
  payload.push_back(" ");
  extern InterprocessCommunication *ipc;
  ipc->send(ipcstBibleditBibletime, ipcctBibleTimeShutdown, payload);
}

bool BibleTime::connected()
// Whether BibleTime is connected.
{
  // Get the methodcall.
  extern InterprocessCommunication *ipc;
  vector < ustring > payload;
  payload = ipc->get_payload(ipcctBibleTimeConnected);

  // If there is no data yet, that means Bibletime is still regarded as disconnected.
  if (payload.empty())
    return false;

  // If there is data, the flag indicates whether BibleTime is connected.
  return convert_to_bool(payload[0]);
}

void BibleTime::sendreference(const Reference & reference)
// Sends the reference for BibleTime.
// Bibledit-bibletime will pass it on shortly after.
{
  // BibleTime does not accept verses like "2-6a", etc.
  // So we take the whole verse that can be extracted from the verse.
  // That would be "2" in this example. 
  // If the verse is "6b-11", then it would be "7".
  vector < int >encodedverses = verses_encode(reference.verse);
  ustring verse;
  for (unsigned int i = 0; i < encodedverses.size(); i++) {
    if (verse.empty()) {
      if ((encodedverses[i] % 2) == 0) {
        verse = convert_to_string(encodedverses[i] / 2);
      }
    }
  }
  if (verse.empty())
    verse = number_in_string(reference.verse);

  // BibleTime does not accept chapter 0 or verse 0.
  // If this is sent to it, it goes to another reference instead.
  // Solution is, if chapter is 0 make it 1, and the same for the verse.
  unsigned int chapter = reference.chapter;
  if (chapter == 0) {
    chapter = 1;
  }
  if (verse == "0") {
    verse = "1";
  }
  // Send the reference.
  vector < ustring > payload;
  payload.push_back(books_id_to_osis(reference.book) + "." + convert_to_string(chapter) + "." + verse);
  extern InterprocessCommunication *ipc;
  ipc->send(ipcstBibleditBibletime, ipcctBibleTimeReference, payload);
}

bool BibleTime::getreference(Reference & reference)
// Read the current reference.
// This was sent by bibledit-bibletime.
{
  extern InterprocessCommunication *ipc;
  vector < ustring > payload;
  payload = ipc->get_payload(ipcctBibleTimeReference);
  ipc->erase_payload(ipcctBibleTimeReference);

  // Return false if there was no valid reference received.
  if (payload.empty())
    return false;

  // Assemble the reference.
  bool gotreference = reference_discover(0, 0, "", payload[0], reference.book, reference.chapter, reference.verse);

  // Got a reference?
  return gotreference;
}

vector < ustring > BibleTime::getbibles()
{
  // Get the Bibles.
  extern InterprocessCommunication *ipc;
  vector < ustring > bibles;
  bibles = ipc->get_payload(ipcctBibleTimeBibles);

  // Signal the helper to again get the modules.
  getmodules();

  // Return the Bibles.
  return bibles;
}

vector < ustring > BibleTime::getcommentaries()
{
  // Get the commentaries.
  extern InterprocessCommunication *ipc;
  vector < ustring > commentaries;
  commentaries = ipc->get_payload(ipcctBibleTimeCommentaries);

  // Signal the helper to again get the modules.
  getmodules();

  // Return the commentaries.
  return commentaries;
}

vector < ustring > BibleTime::search_in_default_bible(const ustring & searchtext)
{
  return search("", searchtext, 0);;
}

vector < ustring > BibleTime::search_in_open_modules(const ustring & searchtext)
{
  return search("", searchtext, 1);
}

vector < ustring > BibleTime::search_in_module(const ustring & modulename, const ustring & searchtext)
{
  return search(modulename, searchtext, 2);
}

vector < ustring > BibleTime::search(const ustring & modulename, const ustring & searchtext, int selector)
{
  // Pointers to IPC.
  extern InterprocessCommunication *ipc;

  // Write search commands.
  vector < ustring > searchcommand;
  searchcommand.push_back(modulename);
  searchcommand.push_back(searchtext);
  searchcommand.push_back(convert_to_string(selector));
  ipc->send(ipcstBibleditBibletime, ipcctBibleTimeSearchCommand, searchcommand);

  // Wait till the bibledit-bibletime has done the search.
  bool searching = true;
  while (searching) {
    vector < ustring > searchdone;
    searchdone = ipc->get_payload(ipcctBibleTimeSearchDone);
    searching = searchdone.empty();
    g_usleep(100000);
  }

  // Search done: erase that flag.
  ipc->erase_payload(ipcctBibleTimeSearchDone);

  // Obtain the search results.
  vector < ustring > searchresults;
  searchresults = ipc->get_payload(ipcctBibleTimeSearchResults);
  ipc->erase_payload(ipcctBibleTimeSearchResults);
  return searchresults;
}

void BibleTime::reloadmodules()
{
  vector < ustring > payload;
  payload.push_back(" ");
  extern InterprocessCommunication *ipc;
  ipc->send(ipcstBibleditBibletime, ipcctBibleTimeReloadModules, payload);
}

ustring BibleTime::database()
// Returns the filename of the database.
{
  return gw_build_filename(directories_get_temp(), "bibletime.sql");
}

void BibleTime::getmodules()
// Sends that the available modules should be fetched.
{
  vector < ustring > payload;
  payload.push_back(" ");
  extern InterprocessCommunication *ipc;
  ipc->send(ipcstBibleditBibletime, ipcctBibleTimeGetModules, payload);
}
