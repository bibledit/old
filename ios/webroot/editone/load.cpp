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


#include <editone/load.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <checksum/logic.h>
#include <editor/import.h>


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

  if (part == "prefix") {
    vector <string> lines;
    for (int vs = 0; vs < verse; vs++) {
      lines.push_back (usfm_get_verse_text (usfm, vs));
    }
    usfm = filter_string_implode (lines, "\n");

    Editor_Import editor_import = Editor_Import (request);
    editor_import.load (usfm);
    editor_import.stylesheet (stylesheet);
    editor_import.run ();
    
    string html = editor_import.get ();
    
    html = editone_load_remove_id_notes (html);
    
    return html;
  }
  
  if (part == "verse") {
    usfm = usfm_get_verse_text (usfm, verse);

    Editor_Import editor_import = Editor_Import (request);
    editor_import.load (usfm);
    editor_import.stylesheet (stylesheet);
    editor_import.run ();
    
    string html = editor_import.get ();
    
    return Checksum_Logic::send (html);
  }
  
  if (part == "suffix") {
    vector <int> verses = usfm_get_verse_numbers (usfm);
    
    vector <string> lines;
    for (auto vs : verses) {
      if (vs > verse) {
        lines.push_back (usfm_get_verse_text (usfm, vs));
      }
    }

    usfm = filter_string_implode (lines, "\n");
    
    Editor_Import editor_import = Editor_Import (request);
    editor_import.load (usfm);
    editor_import.stylesheet (stylesheet);
    editor_import.run ();
    
    string html = editor_import.get ();
    
    html = editone_load_remove_id_notes (html);

    return html;
  }
  
  return "";
}
