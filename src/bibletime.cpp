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
#include "tiny_utilities.h"
#include "settings.h"
#include "d_bus.h"


const gchar * bibletime_dbus_object ()
{
  return "/BibleTime";
}


const gchar * bibletime_dbus_interface ()
{
  return "info.bibletime.BibleTime";
}


void bibletime_reference_send (Reference reference)
// Send a reference to BibleTime.
{
  // Check whether sending references to BibleTime has been enabled by the user.
  extern Settings * settings;
  if (settings->genconfig.reference_exchange_send_to_bibletime_get()) {
    // Check whether the user does not receive referenes from BibleTime at this moment.
    if (!settings->session.receiving_references || !settings->genconfig.reference_exchange_receive_from_bibletime_get()) {
      // BibleTime does not accept verses like "2-6a", etc.
      // So we take the whole verse that can be extracted from the verse.
      reference.verse = number_in_string (reference.verse);    
      // BibleTime does not accept chapter 0 or verse 0.
      // If this is sent to it, it goes to another reference instead.
      // Solution is, if chapter is 0 make it 1, and the same for the verse.
      if (reference.chapter == 0) {
        reference.chapter = 1;
      }
      if (reference.verse == "0") {
        reference.verse = "1";
      }
      // Create the payload.
      ustring payload = books_id_to_osis(reference.book) + "." + convert_to_string(reference.chapter) + "." + reference.verse;
      // Send it.
      extern DBus * dbus;
      dbus->send_to_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "syncAllVerseBasedModules", payload);
    }
  }
}


bool bibletime_reference_receive (Reference& reference)
{
  extern DBus * dbus;
  vector <ustring> reply = dbus->receive_from_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "getCurrentReference");
  if (reply.empty()) 
    return false;
  // The response should be something like: "[KJV] [BIBLE] Jer.48.13" (without the quotes).
  replace_text (reply[0], ".", " ");
  Parse parse (reply[0]);
  if (parse.words.size() == 5) {
    reference.book = books_osis_to_id (parse.words[2]);
    reference.chapter = convert_to_int (parse.words[3]);
    reference.verse = parse.words[4];
    return true;
  }
  return false;
}


void bibletime_reload_modules ()
{
  extern DBus * dbus;
  dbus->send_to_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "reloadModules", "");
}


/*

To checkout bibletime:
svn co https://bibletime.svn.sourceforge.net/svnroot/bibletime/trunk/bibletime bibletime 

*/
