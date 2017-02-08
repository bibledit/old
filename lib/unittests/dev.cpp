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


#include <unittests/dev.h>
#include <unittests/utilities.h>
#include <editor/usfm2html.h>
#include <editor/html2usfm.h>
#include <styles/logic.h>


void test_dev () // Todo
{
  trace_unit_tests (__func__);
  return;
  // \sd Semantic division.
  {
    string standard_usfm =
    "\\p aragraph\n"
    "\\sd\n"
    "\\p paragraph";
    {
      // DOM-based editor.
      Editor_Usfm2Html editor_usfm2html;
      editor_usfm2html.load (standard_usfm);
      editor_usfm2html.stylesheet (styles_logic_standard_sheet ());
      editor_usfm2html.run ();
      string html = editor_usfm2html.get ();
      string standard_html = "<p class=\"p\"><span>paragraph</span></p><p class=\"b\"><br></p><p class=\"p\"><span>paragraph</span></p>";
      evaluate (__LINE__, __func__, standard_html, html);
      
      Editor_Html2Usfm editor_html2usfm;
      editor_html2usfm.load (html);
      editor_html2usfm.stylesheet (styles_logic_standard_sheet ());
      editor_html2usfm.run ();
      string usfm = editor_html2usfm.get ();
      evaluate (__LINE__, __func__, standard_usfm, usfm);
    }
    {
      // Quill-based editor.
      Editor_Usfm2Html editor_usfm2html;
      editor_usfm2html.load (standard_usfm);
      editor_usfm2html.stylesheet (styles_logic_standard_sheet ());
      editor_usfm2html.quill ();
      editor_usfm2html.run ();
      string html = editor_usfm2html.get ();
      string standard_html = "<p class=\"b-p\"><span>paragraph</span></p><p class=\"b-b\"><br></p><p class=\"b-p\"><span>paragraph</span></p>";
      evaluate (__LINE__, __func__, standard_html, html);
      
      Editor_Html2Usfm editor_html2usfm;
      editor_html2usfm.quill ();
      editor_html2usfm.load (html);
      editor_html2usfm.stylesheet (styles_logic_standard_sheet ());
      editor_html2usfm.run ();
      string usfm = editor_html2usfm.get ();
      evaluate (__LINE__, __func__, standard_usfm, usfm);
    }
  }

  
}
