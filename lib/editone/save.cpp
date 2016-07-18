/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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


#include <editone/save.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <checksum/logic.h>
#include <database/modifications.h>
#include <database/logs.h>
#include <database/git.h>
#include <locale/translate.h>
#include <locale/logic.h>
#include <editor/html2usfm.h>
#include <access/bible.h>


string editone_save_url ()
{
  return "editone/save";
}


bool editone_save_acl (void * webserver_request)
{
  if (Filter_Roles::access_control (webserver_request, Filter_Roles::translator ())) return true;
  bool read, write;
  access_a_bible (webserver_request, read, write);
  return read;
}


string editone_save (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  Database_Modifications database_modifications;

  
  // Check on information about where to save the verse.
  bool save = (request->post.count ("bible") && request->post.count ("book") && request->post.count ("chapter") && request->post.count ("verse") && request->post.count ("html"));
  if (!save) {
    return translate("Don't know where to save");
  }

  
  string bible = request->post["bible"];
  int book = convert_to_int (request->post["book"]);
  int chapter = convert_to_int (request->post["chapter"]);
  int verse = convert_to_int (request->post["verse"]);
  string html = request->post["html"];
  string checksum = request->post["checksum"];
  string added_style = request->post["style"];

  
  // Checksum.
  if (Checksum_Logic::get (html) != checksum) {
    request->response_code = 409;
    return translate ("Checksum error");
  }

  
  // Decode html encoded in javascript.
  html = filter_url_tag_to_plus (html);

  
  // Check there's anything to save at all.
  html = filter_string_trim (html);
  if (html.empty ()) {
    return translate ("Nothing to save");
  }
  
  
  // Check on valid UTF-8.
  if (!unicode_string_is_valid (html)) {
    return translate ("Cannot save: Needs Unicode");
  }
  
  
  if (!access_bible_book_write (request, "", bible, book)) {
    return translate ("No write access");
  }

  
  // If an initial style was added to the first paragraph, remove it again.
  if (!added_style.empty ()) {
    // First do a html to xml conversion to avoid a mismatched tag error later in the save chain.
    html = html2xml (html);
    xml_document document;
    document.load_string (html.c_str(), parse_ws_pcdata_single);
    xml_node p_node = document.first_child ();
    string p_style = p_node.attribute ("class").value ();
    if (added_style == p_style) {
      p_node.remove_attribute ("class");
    }
    stringstream output;
    document.print (output, "", format_raw);
    html = output.str ();
  }
  
  
  string stylesheet = request->database_config_user()->getStylesheet();
  
  
  // Convert xml entities to normal characters.
  html = filter_string_desanitize_html (html);
  
  
  // Convert the html back to USFM in the special way for editing one verse.
  string usfm = editor_export_verse (stylesheet, html);

  
  // Collect some data about the changes for this user.
  string username = request->session_logic()->currentUser ();
  int oldID = request->database_bibles()->getChapterId (bible, book, chapter);
  string oldText = request->database_bibles()->getChapter (bible, book, chapter);

  
  // Safely store the verse.
  string message = usfm_safely_store_verse (request, bible, book, chapter, verse, usfm); // Todo
  if (message.empty ()) {
    // Server: Store details for the user's changes.
#ifndef HAVE_CLIENT
    int newID = request->database_bibles()->getChapterId (bible, book, chapter);
    string newText = request->database_bibles()->getChapter (bible, book, chapter);
    database_modifications.recordUserSave (username, bible, book, chapter, oldID, oldText, newID, newText);
    Database_Git::store_chapter (username, bible, book, chapter, oldText, newText);
#endif
    (void) database_modifications;
    return locale_logic_text_saved ();
  }

  
  return message;
}
