/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#include "libraries.h"
#include <glib.h>
#include "printproject2.h"
#include "utilities.h"
#include "bible.h"
#include "usfmtools.h"
#include "pdfviewer.h"
#include "xmlutils.h"
#include "paper.h"
#include "style.h"
#include "stylesheetutils.h"
#include "constants.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "directories.h"
#include "portion_utils.h"
#include "projectutils.h"
#include "books.h"
#include "unixwrappers.h"
#include "textreplacement.h"
#include "tiny_utilities.h"
#include "xetex.h"
#include <glib/gi18n.h>

PrintProject2::PrintProject2(ProjectMemory * project)
{
  myproject = project;
  scriptureportions = NULL;
  nobold = false;
}


PrintProject2::~PrintProject2()
{
  if (scriptureportions)
    delete scriptureportions;
}


void PrintProject2::portion_project(const ustring & project)
{
  portionproject = project;
}


void PrintProject2::comment(const ustring & text)
// Adds a comment to be printed with the main text.
{
  comments.push_back(text);
}


void PrintProject2::no_bold ()
// No bold printing (except for the changes).
{
  nobold = true;
}


void PrintProject2::print()
// Runs the project through xetex and shows it in a pdf viewer.
{
  // Scripture related data.
  // Possible exclusion of books.
  if (portionproject.empty())
    portionproject = myproject->name;
  scriptureportions = new ScripturePortions(portionproject);
  if (scriptureportions->books.empty()) {
    gtkw_dialog_info(NULL, _("There were no books to print\nSelect some books and try again"));
    return;
  }

  // Create the XeTeX object.
  XeTeX xetex (0);

  // Usfm style object.
  Usfm usfm (stylesheet_get_actual ());

  // Collect usfm code for all the books.
  for (unsigned int i = 0; i < scriptureportions->books.size(); i++) {

    // Book id.
    unsigned int book_id = books_english_to_id(scriptureportions->books[i]);

    // Ranges to include.
    WithinReferencesRange inrange;
    {
      vector <unsigned int> chapters_from, chapters_to;
      vector <ustring> verses_from, verses_to;
      select_portion_get_values(portionproject, book_id, scriptureportions->portions[i], chapters_from, verses_from, chapters_to, verses_to);
      inrange.add_portion(book_id, chapters_from, verses_from, chapters_to, verses_to);
    }
    inrange.set_book (book_id);
    inrange.set_chapter (0);
    inrange.set_verse ("0");

    // Open the book.
    vector <ustring> full_book_lines;
    for (unsigned int i2 = 0; i2 < myproject->data.size(); i2++) {
      if (myproject->data[i2].number == book_id) {
        full_book_lines = myproject->data[i2].get_data();
      }
    }

    // Take only those portions the user wishes to print.
    vector <ustring> book_lines;
    for (unsigned int i = 0; i < full_book_lines.size(); i++) {
      ustring line = full_book_lines[i];
      ustring marker = usfm_extract_marker(line);
      if (usfm.is_chapter_number(marker)) {
        inrange.set_chapter (convert_to_int(number_in_string(line)));
        inrange.set_verse ("0");
      }
      if (usfm.is_verse_number(marker)) {
        size_t position = line.find(" ");
        position = CLAMP(position, 0, line.length());
        ustring verse = line.substr(0, position);
        inrange.set_verse (verse);
      }
      if (inrange.in_range()) {
        book_lines.push_back (full_book_lines[i]);
      } else {
        if (marker == "id") {
          ParseLine parseline (full_book_lines[i]);
          if (!parseline.lines.empty ()) {
            book_lines.push_back (parseline.lines[0]);
          }
        }
      }
    }

    // Do text replacements on the lines.
    text_replacement(book_lines);
    // Add the lines to the XeTeX object.
    xetex.add_book (book_id, book_lines);
  }

  // Process the data.
  ustring pdf_file = xetex.run ();

  // Display the pdf file if there is one.
  // There would be none if the formatting process was cancelled by the user.
  if (!pdf_file.empty()) {
    pdfviewer_view (pdf_file);
  }
}

