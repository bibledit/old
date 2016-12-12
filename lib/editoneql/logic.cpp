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


#include <editoneql/logic.h>
#include <editor/usfm2html.h>
#include <editor/html2usfm.h>
#include <filter/string.h>
#include <quill/logic.h>


void editoneql_logic_editable_html (string prefix_last_p_style, string usfm, string stylesheet, string & html, string & editable_last_p_style, string & editable_first_applied_p_style)
{
  if (!usfm.empty ()) {
    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.quill ();
    editor_usfm2html.run ();
    html = editor_usfm2html.get ();
    editable_last_p_style = editor_usfm2html.currentParagraphStyle;
  }
  
  // If the first paragraph of the editable verse does not have a paragraph style applied,
  // apply the last paragraph style of the prefix to the first paragraph of the focused verse.
  // For example, html like this:
  // <p><span class="v">7</span><span> </span><span>For Yahweh knows the way of the righteous,</span></p><p class="q2"><span>but the way of the wicked shall perish.</span></p>
  // ... becomes like this:
  // <p class="q1"><span class="v">7</span><span /><span>For Yahweh knows the way of the righteous,</span></p><p class="q2"><span>but the way of the wicked shall perish.</span></p>
  if (!html.empty ()) {
    if (!prefix_last_p_style.empty ()) {
      xml_document document;
      html = html2xml (html);
      document.load_string (html.c_str(), parse_ws_pcdata_single);
      xml_node p_node = document.first_child ();
      string p_style = p_node.attribute ("class").value ();
      if (p_style.empty ()) {
        // Add the Quill-based prefix for paragraphs.
        p_style = quill_logic_class_prefix_block () + prefix_last_p_style;
        p_node.append_attribute ("class") = p_style.c_str ();
        // Send the applied paragraph style to the browser,
        // for later use when it saves the modified verse text.
        editable_first_applied_p_style = prefix_last_p_style;
      }
      stringstream output;
      document.print (output, "", format_raw);
      html = output.str ();
    }
  }
}


string editoneql_logic_html_to_usfm (string stylesheet, string html, string applied_p_style)
{
  // If an initial style was added to the first paragraph, remove it again.
  if (!applied_p_style.empty ()) {
    // Convert to Quill-based style.
    applied_p_style.insert (0, quill_logic_class_prefix_block ());
    // First do a html to xml conversion to avoid a mismatched tag error later in the save chain.
    html = html2xml (html);
    xml_document document;
    document.load_string (html.c_str(), parse_ws_pcdata_single);
    xml_node p_node = document.first_child ();
    string p_style = p_node.attribute ("class").value ();
    if (applied_p_style == p_style) {
      p_node.remove_attribute ("class");
    }
    stringstream output;
    document.print (output, "", format_raw);
    html = output.str ();
  }
  
  // Convert xml entities to normal characters.
  html = filter_string_desanitize_html (html);
  
  // Convert the html back to USFM in the special way for editing one verse.
  string usfm = editor_export_verse_quill (stylesheet, html);
  
  return usfm;
}
