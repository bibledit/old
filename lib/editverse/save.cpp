/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <editverse/save.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
// Todo #include <filter/bibles.h>
#include <webserver/request.h>
#include <checksum/logic.h>
#include <database/modifications.h>
#include <database/logs.h>
#include <locale/translate.h>


string editverse_save_url ()
{
  return "editverse/save";
}


bool editverse_save_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string editverse_save (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  Database_Modifications database_modifications = Database_Modifications ();

  
  // Check on information about where to save the verse.
  bool save = (request->post.count ("bible") && request->post.count ("book") && request->post.count ("chapter") && request->post.count ("verse") && request->post.count ("usfm"));
  if (save) {
    return gettext("Don't know where to save");
  }

  
  string bible = request->post["bible"];
  int book = convert_to_int (request->post["book"]);
  int chapter = convert_to_int (request->post["chapter"]);
  int verse = convert_to_int (request->post["verse"]);
  string usfm = request->post["usfm"];
  string checksum = request->post["checksum"];

  
  // Checksum. Todo test it.
  if (Checksum_Logic::get (usfm) != checksum) {
    request->response_code = 409;
    return gettext ("Checksum error");
  }

  
  // Check there's anything to save at all. Todo test it.
  usfm = filter_string_trim (usfm);
  if (usfm == "") {
    return gettext("Nothing to save");
  }
  
  
  // Check on valid UTF-8.
  if (!unicode_string_is_valid (usfm)) {
    return gettext("Cannot save: Needs Unicode");
  }
  
  
  // Get the old chapter USFM into an array of verse => USFM fragment.
  string usfmString = request->database_bibles()->getChapter (bible, book, chapter);
  vector <int> verses = usfm_get_verse_numbers (usfmString);
  {
    set <int> unique (verses.begin(), verses.end());
    verses.assign (unique.begin(), unique.end());
  }
  sort (verses.begin(), verses.end());
  map <int, string> usfmMap;
  for (auto vs : verses) {
    usfmMap [vs] = usfm_get_verse_text (usfmString, vs);
  }
  
  
  // Store the verse USFM in the map.
  usfmMap [verse] = usfm;
  
  
  // Create the updated chapter USFM as a string.
  usfm.clear ();
  for (auto & element : usfmMap) {
    cout << element.first << endl; // Todo
    if (!usfm.empty ()) usfm.append ("\n");
    usfm.append (element.second);
  }
  
  
  string stylesheet = request->database_config_user()->getStylesheet();
  vector <BookChapterData> book_chapter_text = usfm_import (usfm, stylesheet);
  for (BookChapterData & data : book_chapter_text) {
    int book_number = data.book;
    int chapter_number = data.chapter;
    string chapter_data_to_save = data.data;
    if (((book_number == book) || (book_number == 0)) && (chapter_number == chapter)) {
      // Collect some data about the changes for this user.
      string username = request->session_logic()->currentUser ();
      int oldID = request->database_bibles()->getChapterId (bible, book, chapter);
      string oldText = request->database_bibles()->getChapter (bible, book, chapter);
      // Safely store the chapter.
      /* Todo port Filter_Bibles and write tests for it.
      bool saved = Filter_Bibles::safeStoreChapter (bible, book, chapter, chapter_data_to_save);
      if (saved) {
        // Store details for the user's changes.
        int newID = request->database_bibles->getChapterId (bible, book, chapter);
        string newText = $chapter_data_to_save;
        database_modifications.recordUserSave (username, bible, book, chapter, oldID, oldText, newID, newText);
        return gettext("Saved");
      } else {
        return gettext("Not saved because of too many changes");
      }
       */
    } else {
      Database_Logs::log ("The following data could not be saved and was discarded: " + chapter_data_to_save);
      return gettext("Save failure");
    }
  }

  
  return "";
}
