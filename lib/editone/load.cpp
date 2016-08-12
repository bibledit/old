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
#include <pugixml/pugixml.hpp>
#include <config/globals.h>
#include <access/bible.h>
#include <editone/logic.h>


using namespace pugi;


string editone_load_url ()
{
  return "editone/load";
}


bool editone_load_acl (void * webserver_request)
{
  if (Filter_Roles::access_control (webserver_request, Filter_Roles::translator ())) return true;
  bool read, write;
  access_a_bible (webserver_request, read, write);
  return read;
}


string editone_load (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string bible = request->query ["bible"];
  int book = convert_to_int (request->query ["book"]);
  int chapter = convert_to_int (request->query ["chapter"]);
  int verse = convert_to_int (request->query ["verse"]);
  
  string stylesheet = request->database_config_user()->getStylesheet ();

  string chapter_usfm = request->database_bibles()->getChapter (bible, book, chapter);

  vector <int> verses = usfm_get_verse_numbers (chapter_usfm);
  int highest_verse = 0;
  if (!verses.empty ()) highest_verse = verses.back ();
  
  string editable_usfm = usfm_get_verse_text (chapter_usfm, verse);
  string prefix_usfm = usfm_get_verse_range_text (chapter_usfm, 0, verse - 1, editable_usfm);
  string suffix_usfm = usfm_get_verse_range_text (chapter_usfm, verse + 1, highest_verse, editable_usfm);

  // Last paragraph style of the prefix: To be used for the starting visual style for the focused verse.
  string prefix_last_p_style;

  string prefix_html;
  editone_logic_prefix_html_stage_one (prefix_usfm, stylesheet, prefix_html, prefix_last_p_style);
  
  // Last paragraph style of the focused verse: For the starting visual style of the suffix.
  string focused_verse_last_p_style;
  
  string focused_verse_html;
  string focused_verse_applied_p_style;
  editone_logic_editable_html (prefix_last_p_style, editable_usfm, stylesheet, focused_verse_html, focused_verse_last_p_style, focused_verse_applied_p_style);
  
  string suffix_html;
  editone_logic_suffix_html (focused_verse_last_p_style, suffix_usfm, stylesheet, suffix_html);
  
  // If the verse was empty, ensure that it has a non-breaking space as the last character,
  // for easier text entry in the verse.
  if (usfm_contains_empty_verses (editable_usfm)) {
    string search = "<span> </span></p>";
    string replace = "<span>" + unicode_non_breaking_space_entity () + "</span></p>";
    focused_verse_html = filter_string_str_replace (search, replace, focused_verse_html);
  }

  string data;
  data.append (focused_verse_applied_p_style);
  data.append ("#_be_#");
  data.append (prefix_html);
  data.append ("#_be_#");
  data.append (focused_verse_html);
  data.append ("#_be_#");
  data.append (suffix_html);
  
  string user = request->session_logic ()->currentUser ();
  bool write = access_bible_book_write (webserver_request, user, bible, book);
  data = Checksum_Logic::send (data, write);
  
  return data;
}
