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

  string chapter_usfm = request->database_bibles()->getChapter (bible, book, chapter);

  string focused_verse_usfm = usfm_get_verse_text (chapter_usfm, verse);
  
  vector <int> verses = usfm_get_verse_numbers (chapter_usfm);
  int highest_verse = 0;
  if (!verses.empty ()) highest_verse = verses.back ();

  string prefix_usfm = usfm_get_verse_range_text (chapter_usfm, 0, verse - 1, focused_verse_usfm);

  string suffix_usfm = usfm_get_verse_range_text (chapter_usfm, verse + 1, highest_verse, focused_verse_usfm);

  // Last paragraph style of the prefix: To be used for the starting visual style for the focused verse.
  string prefix_paragraph_style;
  
  string prefix_html;
  if (!prefix_usfm.empty ()) {
    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (prefix_usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.run ();
    prefix_html = editor_usfm2html.get ();
    prefix_paragraph_style = editor_usfm2html.currentParagraphStyle;
  }
  
  // Last paragraph style of the focused verse: For the starting visual style of the suffix.
  string focused_verse_paragraph_style;
  
  string focused_verse_html;
  if (!focused_verse_usfm.empty ()) {
    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (focused_verse_usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.run ();
    focused_verse_html = editor_usfm2html.get ();
    focused_verse_html = editone_load_remove_id_notes (focused_verse_html);
    focused_verse_paragraph_style = editor_usfm2html.currentParagraphStyle;
  }

  
  // If the first paragraph of the focused verse does not have a paragraph style applied,
  // apply the last paragraph style of the prefix to the first paragraph of the focused verse.
  // For example, html like this:
  // <p><span class="v">7</span><span> </span><span>For Yahweh knows the way of the righteous,</span></p><p class="q2"><span>but the way of the wicked shall perish.</span></p>
  // ... becomes like this:
  // <p class="q1"><span class="v">7</span><span /><span>For Yahweh knows the way of the righteous,</span></p><p class="q2"><span>but the way of the wicked shall perish.</span></p>
  string focused_verse_applied_style;
  if (!focused_verse_html.empty ()) {
    if (!prefix_paragraph_style.empty ()) {
      xml_document document;
      document.load_string (focused_verse_html.c_str(), parse_ws_pcdata_single);
      xml_node p_node = document.first_child ();
      string p_style = p_node.attribute ("class").value ();
      if (p_style.empty ()) {
        p_node.append_attribute ("class") = prefix_paragraph_style.c_str ();
        // Send the applied paragraph style to the browser,
        // for later use when it saves the modified verse text.
        focused_verse_applied_style = prefix_paragraph_style;
      }
      stringstream output;
      document.print (output, "", format_raw);
      focused_verse_html = output.str ();
    }
  }
  
  
  string suffix_html;
  if (!suffix_usfm.empty ()) {
    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (suffix_usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.run ();
    suffix_html = editor_usfm2html.get ();
    suffix_html = editone_load_remove_id_notes (suffix_html);
  }
  
  // If the first paragraph of the suffix does not have a paragraph style applied,
  // apply the last paragraph style of the focused verse to the first paragraph of the suffix.
  // For example, html like this:
  // <p><span class="v">7</span><span> </span><span>For Yahweh knows the way of the righteous,</span></p><p class="q2"><span>but the way of the wicked shall perish.</span></p>
  // ... becomes like this:
  // <p class="q1"><span class="v">7</span><span /><span>For Yahweh knows the way of the righteous,</span></p><p class="q2"><span>but the way of the wicked shall perish.</span></p>
  if (!suffix_html.empty ()) {
    if (!focused_verse_paragraph_style.empty ()) {
      xml_document document;
      document.load_string (suffix_html.c_str(), parse_ws_pcdata_single);
      xml_node p_node = document.first_child ();
      string p_style = p_node.attribute ("class").value ();
      if (p_style.empty ()) {
        p_node.append_attribute ("class") = focused_verse_paragraph_style.c_str ();
      }
      stringstream output;
      document.print (output, "", format_raw);
      suffix_html = output.str ();
    }
  }

  
  string data;
  data.append (focused_verse_applied_style);
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
