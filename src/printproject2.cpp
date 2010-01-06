/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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


void PrintProject2::print() // Todo moving / implementing all code into the XeTeX object.
// Runs the project through xetex and shows it in a pdf viewer.
{
  // Scripture related data.
  // Possible exclusion of books.
  if (portionproject.empty())
    portionproject = myproject->name;
  scriptureportions = new ScripturePortions(portionproject);
  if (scriptureportions->books.empty()) {
    gtkw_dialog_info(NULL, "There were no books to print\nSelect some books and try again");
    return;
  }

  // Create the XeTeX object.
  XeTeX xetex (0);

  // Settings.
  extern Settings *settings;
  //ProjectConfiguration *projectconfig = settings->projectconfig(myproject->name);

  // Page.
  xetex.page_size_set(settings->genconfig.paper_width_get(), settings->genconfig.paper_height_get());
  xetex.page_margins_set(settings->genconfig.paper_inside_margin_get(), settings->genconfig.paper_outside_margin_get(), settings->genconfig.paper_top_margin_get(), settings->genconfig.paper_bottom_margin_get());



/*

  // Styles.
  usfm2xetex.add_styles(usfm2xslfo_read_stylesheet(stylesheet_get_actual ()));

  // Headers.
  if (settings->genconfig.printdate_get()) {
    xetex2pdf.print_date_in_header();
  }
  // Font.
  if (!projectconfig->editor_font_default_get())
    xetex2pdf.set_font(projectconfig->editor_font_name_get());

  // Line spacing.
  if (!projectconfig->editor_font_default_get())
    xetex2pdf.set_line_spacing(projectconfig->text_line_height_get());

  // Right-to-left.
  xetex2pdf.set_right_to_left(projectconfig->right_to_left_get());

  // Inclusion of full references with the notes.
  if (settings->session.print_references_in_notes_in_full)
    usfm2xetex.set_include_full_references_with_notes();

  // Portions.
  for (unsigned int i = 0; i < scriptureportions->books.size(); i++) {
    vector <unsigned int> chapters_from, chapters_to;
    vector <ustring> verses_from, verses_to;
    select_portion_get_values(portionproject, books_english_to_id(scriptureportions->books[i]), scriptureportions->portions[i], chapters_from, verses_from, chapters_to, verses_to);
    usfm2xetex.add_print_portion(books_english_to_id(scriptureportions->books[i]), chapters_from, verses_from, chapters_to, verses_to);
  }

  // Language.
  usfm2xetex.set_language (projectconfig->language_get());

  // No bold.
  if (nobold) {
    usfm2xetex.no_bold();
  }
    
  // Start off with inserting any remarks.
  for (unsigned int r = 0; r < comments.size(); r++) {
    xetex2pdf.open_paragraph();
    xetex2pdf.add_text(comments[r]);
    xetex2pdf.close_paragraph();
  }

  // Collect usfm code for all the books.
  for (unsigned int i = 0; i < scriptureportions->books.size(); i++) {
    // Open the book.
    vector < ustring > book_lines;
    unsigned int id = books_english_to_id(scriptureportions->books[i]);
    for (unsigned int i2 = 0; i2 < myproject->data.size(); i2++) {
      if (myproject->data[i2].number == id) {
        book_lines = myproject->data[i2].get_data();
      }
    }

    // Do text replacements on the lines.
    text_replacement(book_lines);
    // Add the lines to the converter.
    usfm2xetex.add_usfm_code(book_lines);
  }

  // Process the data.
  usfm2xetex.process();
  xetex2pdf.run();
  */

  // Process the data.
  ustring pdf_file = xetex.run ();

  // Display the pdf file if there was one.
  // There would be none if the formatting process was cancelled.
  if (!pdf_file.empty()) {
    pdfviewer_view (pdf_file);
  }
}

