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


#include <editone/load.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <checksum/logic.h>
#include <editor/usfm2html.h>


string editone_load_url ()
{
  return "editone/load";
}


bool editone_load_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string editone_load_remove_id_notes (string html)
{
  return filter_string_str_replace (" id=\"notes\"", "", html);
}


string editone_load (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string bible = request->query ["bible"];
  int book = convert_to_int (request->query ["book"]);
  int chapter = convert_to_int (request->query ["chapter"]);
  int verse = convert_to_int (request->query ["verse"]);
  string part = request->query ["part"];
  
  string stylesheet = request->database_config_user()->getStylesheet ();

  string usfm = request->database_bibles()->getChapter (bible, book, chapter);

  string focused_verse_usfm = usfm_get_verse_text (usfm, verse);

  if (part == "prefix") {
    
    usfm = usfm_get_verse_range_text (usfm, 0, verse - 1, focused_verse_usfm);

    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.run ();
    
    string html = editor_usfm2html.get ();
    
    html = editone_load_remove_id_notes (html);
    
    return html;
  }
  
  if (part == "verse") {

    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (focused_verse_usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.run ();
    
    string html = editor_usfm2html.get ();

    string user = request->session_logic ()->currentUser ();
    bool readwrite = !request->database_users ()->hasReadOnlyAccess2Book (user, bible, book);

    return Checksum_Logic::send (html, readwrite);
  }
  
  if (part == "suffix") {
    
    vector <int> verses = usfm_get_verse_numbers (usfm);
    int highest_verse = 0;
    if (!verses.empty ()) highest_verse = verses.back ();
    usfm = usfm_get_verse_range_text (usfm, verse + 1, highest_verse, focused_verse_usfm);

    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.run ();
    
    string html = editor_usfm2html.get ();
    
    html = editone_load_remove_id_notes (html);

    return html;
  }
  
  return "";
}
