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


#include <edit/save.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <ipc/focus.h>
#include <database/modifications.h>
#include <database/logs.h>
#include <checksum/logic.h>
#include <editor/export.h>
#include <locale/translate.h>


string edit_save_url ()
{
  return "edit/save";
}


bool edit_save_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string edit_save (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Modifications database_modifications = Database_Modifications ();
  
  if (request->post.count ("bible") && request->post.count ("book") && request->post.count ("chapter") && request->post.count ("html") && request->post.count ("checksum")) {
    
    string bible = request->post["bible"];
    int book = convert_to_int (request->post["book"]);
    int chapter = convert_to_int (request->post["chapter"]);
    string html = request->post["html"];
    string checksum = request->post["checksum"];

    if (Checksum_Logic::get (html) == checksum) {
      
      html = filter_string_trim (html);
      if (html != "") {
        if (unicode_string_is_valid (html)) {
          
          string stylesheet = request->database_config_user()->getStylesheet();
          
          Editor_Export editor_export = Editor_Export (request);
          editor_export.load (html);
          editor_export.stylesheet (stylesheet);
          editor_export.run ();
          string usfm = editor_export.get ();
          
          vector <BookChapterData> book_chapter_text = usfm_import (usfm, stylesheet);
          for (auto & data : book_chapter_text) {
            int book_number = data.book;
            int chapter_number = data.chapter;
            string chapter_data_to_save = data.data;
            if (((book_number == book) || (book_number == 0)) && (chapter_number == chapter)) {
              
              // Collect some data about the changes for this user.
              string username = request->session_logic()->currentUser ();
              int oldID = request->database_bibles()->getChapterId (bible, book, chapter);
              string oldText = request->database_bibles()->getChapter (bible, book, chapter);
              
              // Safely store the chapter.
              bool saved = usfm_safely_store_chapter (request, bible, book, chapter, chapter_data_to_save);
              
              if (saved) {
                // Store details for the user's changes.
                int newID = request->database_bibles()->getChapterId (bible, book, chapter);
                string newText = chapter_data_to_save;
                database_modifications.recordUserSave (username, bible, book, chapter, oldID, oldText, newID, newText);
                return translate("Saved");
              } else {
                return translate("Not saved because of too many changes");
              }
            } else {
              Database_Logs::log ("The following data could not be saved and was discarded: " + chapter_data_to_save);
              return translate("Save failure");
            }
          }
        } else {
          Database_Logs::log ("The text was not valid Unicode UTF-8. The chapter could not saved and has been reverted to the last good version.");
          return translate("Save failure");
        }
      } else {
        Database_Logs::log ("There was no text. Nothing was saved. The original text of the chapter was reloaded.");
        return translate("Nothing to save");
      }
    } else {
      request->response_code = 409;
      return translate("Checksum error");
    }
  } else {
    return translate("Nothing to save");
  }
  return translate ("Bibledit is confused");
}

