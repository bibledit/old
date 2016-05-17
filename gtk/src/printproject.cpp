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
#include "printproject.h"
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
#include "text2pdf.h"
#include "usfm2text.h"
#include <glib/gi18n.h>

PrintProject::PrintProject(ProjectMemory * project)
{
  myproject = project;
  scriptureportions = NULL;
  nobold = false;
}


PrintProject::~PrintProject()
{
  if (scriptureportions)
    delete scriptureportions;
}


void PrintProject::portion_project(const ustring & project)
{
  portionproject = project;
}


void PrintProject::comment(const ustring & text)
// Adds a comment to be printed with the main text.
{
  comments.push_back(text);
}


void PrintProject::no_bold ()
// No bold printing (except for the changes).
{
  nobold = true;
}


void PrintProject::print()
// Formats the project and shows it in a pdf viewer.
{
  // Scripture related data.
  // Possible exclusion of books.
  if (portionproject.empty()) {
    portionproject = myproject->name;
  }
  scriptureportions = new ScripturePortions(portionproject);
  if (scriptureportions->books.empty()) {
    gtkw_dialog_info(NULL, _("There were no books to print\nSelect some books and try again"));
    return;
  }
  // Settings.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject->name);

  // Create Text to PDF and Usfm to Text converters.
  Text2Pdf text2pdf(gw_build_filename(Directories->get_temp(), "document.pdf"), settings->genconfig.print_engine_use_intermediate_text_get());
  Usfm2Text usfm2text(&text2pdf, true);

  // Styles.
  usfm2text.add_styles(usfm2xslfo_read_stylesheet(stylesheet_get_actual ()));

  // Page.
  text2pdf.page_size_set(settings->genconfig.paper_width_get(), settings->genconfig.paper_height_get());
  text2pdf.page_margins_set(settings->genconfig.paper_inside_margin_get(), settings->genconfig.paper_outside_margin_get(), settings->genconfig.paper_top_margin_get(), settings->genconfig.paper_bottom_margin_get());

  // Headers.
  if (settings->genconfig.printdate_get()) {
    text2pdf.print_date_in_header();
  }
  // Font.
  if (!projectconfig->editor_font_default_get())
    text2pdf.set_font(projectconfig->editor_font_name_get());

  // Line spacing...was in project config, but better to have it in general config
  //if (projectconfig->text_line_height_get())
  //  text2pdf.set_line_spacing(projectconfig->text_line_height_get());

  if (settings->genconfig.printdraft_get()) {
    // Defaults for draft mode are double-spaced (300) and single column.
    text2pdf.set_line_spacing(300);
    text2pdf.page_one_column_only();
  }
  
  if (settings->genconfig.column_spacing_get() != 0.0) {
	text2pdf.column_spacing_set(settings->genconfig.column_spacing_get());
  }
  else {
	// Default to 1.0 cm
	text2pdf.column_spacing_set(1.0);
  }

  // Right-to-left.
  text2pdf.set_right_to_left(projectconfig->right_to_left_get());

  // Inclusion of full references with the notes.
  if (settings->session.print_references_in_notes_in_full)
    usfm2text.set_include_full_references_with_notes();

  // Portions.
  for (unsigned int i = 0; i < scriptureportions->books.size(); i++) {
    vector <unsigned int> chapters_from, chapters_to;
    vector <ustring> verses_from, verses_to;
    select_portion_get_values(portionproject, books_english_to_id(scriptureportions->books[i]), scriptureportions->portions[i], chapters_from, verses_from, chapters_to, verses_to);
    usfm2text.add_print_portion(books_english_to_id(scriptureportions->books[i]), chapters_from, verses_from, chapters_to, verses_to);
  }

  // Language.
  usfm2text.set_language (projectconfig->language_get());

  // No bold.
  if (nobold) {
    usfm2text.no_bold();
  }
    
  // Start off with inserting any remarks.
  for (unsigned int r = 0; r < comments.size(); r++) {
    text2pdf.open_paragraph();
    text2pdf.add_text(comments[r]);
    text2pdf.close_paragraph();
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
    usfm2text.add_usfm_code(book_lines);
  }

  // Process the data.
  usfm2text.process();
  text2pdf.run();

  // Display the pdf file.
  text2pdf.view();
}

