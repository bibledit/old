/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "libraries.h"
#include <glib.h>
#include "printproject.h"
#include "utilities.h"
#include "bible.h"
#include "usfmtools.h"
#include "pdfviewer.h"
#include "xmlutils.h"
#include "paper.h"
#include "xmlfo-utils.h"
#include "style.h"
#include "stylesheetutils.h"
#include "xep.h"
#include "constants.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "directories.h"
#include "fonts.h"
#include "portion_utils.h"
#include "projectutils.h"
#include "books.h"
#include "unixwrappers.h"
#include "textreplacement.h"
#include "tiny_utilities.h"
#include "usfm2xslfo.h"


PrintProject::PrintProject (ProjectMemory * project)
{
  myproject = project;
  scriptureportions = NULL;
}


PrintProject::~PrintProject ()
{
  if (scriptureportions) delete scriptureportions;
}


void PrintProject::portion_project (const ustring& project)
{
  portionproject = project;
}


void PrintProject::comment (const ustring& text)
// Adds a comment to be printed with the main text.
{
  comments.push_back (text);
}


void PrintProject::print ()
// Formats the project and shows it in a pdf viewer.
{
  // Scripture related data.
  // Possible exclusion of books.
  if (portionproject.empty ()) portionproject = myproject->name;
  scriptureportions = new ScripturePortions (portionproject);
  if (scriptureportions->books.empty()) {
    gtkw_dialog_info (NULL, "There were no books to print\nSelect some books and try again");
    return;
  }

  // Settings.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (myproject->name);
  
  // Usfm to XslFo to pdf converter.
  Usfm2XslFo usfm2xslfo (gw_build_filename (directories_get_temp (), "usfm2pdf.pdf"));
  usfm2xslfo.add_styles (usfm2xslfo_read_stylesheet (projectconfig->stylesheet_get ()));
  PrintingFonts printingfonts (settings->genconfig.project_get());
  usfm2xslfo.set_fonts (printingfonts.printing_families, projectconfig->printing_font_size_get());
  usfm2xslfo.set_line_height (projectconfig->printing_line_height_get());
  if (projectconfig->right_to_left_get ()) usfm2xslfo.set_right_to_left ();
  usfm2xslfo.set_page_size (settings->genconfig.paper_width_get (), settings->genconfig.paper_height_get ());
  usfm2xslfo.set_margins (settings->genconfig.paper_top_margin_get (), settings->genconfig.paper_bottom_margin_get (), settings->genconfig.paper_left_margin_get (), settings->genconfig.paper_right_margin_get ());
  usfm2xslfo.set_two_columns ();
  if (settings->session.print_references_in_notes_in_full) {
    usfm2xslfo.set_include_full_references_with_notes ();
  }
  if (settings->genconfig.printdate_get ()) usfm2xslfo.set_print_date ();
  for (unsigned int i = 0; i < scriptureportions->books.size(); i++) {
    vector <unsigned int> chapters_from, chapters_to;
    vector <ustring> verses_from, verses_to;
    select_portion_get_values (portionproject, books_english_to_id (scriptureportions->books[i]),
                               scriptureportions->portions[i],
                               chapters_from, verses_from, chapters_to, verses_to);
    usfm2xslfo.add_print_portion (books_english_to_id (scriptureportions->books[i]), chapters_from, verses_from, chapters_to, verses_to);
  }
  
  // Start off with inserting any remarks.
  for (unsigned int r = 0; r < comments.size(); r++) {
    usfm2xslfo.add_usfm_code (usfm_get_full_opening_marker (usfm2xslfo.default_style ()));
    usfm2xslfo.add_usfm_code (comments[r]);
  }
  
  // Collect usfm code for all the books.
  for (unsigned int i = 0; i < scriptureportions->books.size(); i++) {
    // Open the book.
    vector<ustring> book_lines;
    unsigned int id = books_english_to_id (scriptureportions->books[i]);
    for (unsigned int i2 = 0; i2 < myproject->data.size (); i2++) {
      if (myproject->data[i2].number == id) {
        book_lines = myproject->data[i2].get_data ();
      }
    }
    
    // Do text replacements on the lines.
    text_replacement (book_lines);
    // Add the lines to the converter.
    usfm2xslfo.add_usfm_code (book_lines);
    // Add a column balancing style after each book.
    usfm2xslfo.add_usfm_code (usfm_get_full_opening_marker (usfm2xslfo.column_balancing_style ()));
  }

  // Process the data.
  usfm2xslfo.process ();
  
  // Display the pdf file.
  usfm2xslfo.display (NULL);
}
