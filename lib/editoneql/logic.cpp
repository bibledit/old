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


void editoneql_logic_editable_html (string usfm, string stylesheet, string & html)
{
  if (!usfm.empty ()) {
    Editor_Usfm2Html editor_usfm2html;
    editor_usfm2html.load (usfm);
    editor_usfm2html.stylesheet (stylesheet);
    editor_usfm2html.quill ();
    editor_usfm2html.run ();
    html = editor_usfm2html.get ();
  }
}


string editoneql_logic_html_to_usfm (string stylesheet, string html)
{
  // Convert xml entities to normal characters.
  html = filter_string_desanitize_html (html);
  
  // Convert the html back to USFM in the special way for editing one verse.
  string usfm = editor_export_verse_quill (stylesheet, html);
  
  // Done.
  return usfm;
}
