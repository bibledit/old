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
#include "print_parallel_references.h"
#include "utilities.h"
#include "bible.h"
#include "usfm.h"
#include "usfmtools.h"
#include "pdfviewer.h"
#include "xmlutils.h"
#include "paper.h"
#include "constants.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "directories.h"
#include "notecaller.h"
#include "mapping.h"
#include "portion_utils.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "books.h"
#include "unixwrappers.h"
#include "settings.h"
#include "textreplacement.h"
#include "tiny_utilities.h"
#include "usfm2text.h"
#include "stylesheetutils.h"


void view_parallel_references_pdf(ProjectMemory & main_project, vector < ustring > *extra_projects, vector < Reference > references, bool keep_verses_together_within_page, vector < ustring > *remarks, bool highlight)
/*
 Formats the references in "references", and highlights all words in
 "session->highlights*" and shows them in a pdf viewer.
 There is a pointer to a list of "remarks". Any remarks will be printed first.
 */
{
  // Log.
  gw_message("Printing parallel references");

  // Progress system.
  ProgressWindow progresswindow("Printing Parallel References", false);
  progresswindow.set_iterate(0, 1, references.size());

  // Configuration
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(main_project.name);
  ustring stylesheet = stylesheet_get_actual ();

  // Store the additional projects to print.
  vector < ustring > additional_projects;
  if (extra_projects)
    additional_projects = *extra_projects;
  settings->session.additional_printing_projects = additional_projects;

  // Prepare for mapping.
  Mapping mapping(projectconfig->versification_get(), 0);

  // The converter.
  Text2Pdf text2pdf(gw_build_filename(directories_get_temp(), "document.pdf"), settings->genconfig.print_engine_use_intermediate_text_get());

  // Page.
  text2pdf.page_size_set(settings->genconfig.paper_width_get(), settings->genconfig.paper_height_get());
  text2pdf.page_margins_set(settings->genconfig.paper_inside_margin_get(), settings->genconfig.paper_outside_margin_get(), settings->genconfig.paper_top_margin_get(), settings->genconfig.paper_bottom_margin_get());
  text2pdf.page_one_column_only();

  // Headers.
  if (settings->genconfig.printdate_get()) {
    text2pdf.print_date_in_header();
  }
  // Font, etc., of main project.
  ustring main_font = projectconfig->editor_font_name_get();
  unsigned int main_line_spacing = projectconfig->text_line_height_get();
  if (projectconfig->editor_font_default_get()) {
    main_font.clear();
    main_line_spacing = 100;
  }
  text2pdf.set_font(main_font);
  text2pdf.set_line_spacing(main_line_spacing);
  bool main_right_to_left = projectconfig->right_to_left_get();
  text2pdf.set_right_to_left(main_right_to_left);

  // Start off with inserting any remarks.
  if (remarks) {
    for (unsigned int r = 0; r < remarks->size(); r++) {
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_column_count(1);
      text2pdf.add_text(remarks->at(r));
      text2pdf.close_paragraph();
    }
  }
  // Some variables to avoid excessive session access during highlighting.
  vector < bool > highlight_casesensitives;
  vector < ustring > highlight_words;
  if (highlight) {
    for (unsigned int hl = 0; hl < settings->session.highlights.size(); hl++) {
      highlight_casesensitives.push_back(settings->session.highlights[hl].casesensitive);
      highlight_words.push_back(settings->session.highlights[hl].word);
    }
  }
  // All the projects to be put in this parallel Bible, together with
  // their related information, like mapping, fonts.
  vector < ustring > project_names;
  vector < ProjectMemory > project_memories;
  vector < Mapping > mapping_s;
  vector < ustring > fonts;
  vector < unsigned int >line_spacings;
  vector < bool > right_to_lefts;
  if (extra_projects) {
    vector < ustring > project_s_raw = *extra_projects;
    for (unsigned int i = 0; i < project_s_raw.size(); i++) {
      ProjectMemory projectmemory(project_s_raw[i], true);
      project_memories.push_back(projectmemory);
      ProjectConfiguration *projectconfig = settings->projectconfig(project_s_raw[i]);
      project_names.push_back(project_s_raw[i]);
      Mapping mapping(projectconfig->versification_get(), 0);
      mapping_s.push_back(mapping);
      ustring font = projectconfig->editor_font_name_get();
      unsigned int line_spacing = projectconfig->text_line_height_get();
      if (projectconfig->editor_font_default_get()) {
        font.clear();
        line_spacing = 100;
      }
      fonts.push_back(font);
      line_spacings.push_back(line_spacing);
      right_to_lefts.push_back(projectconfig->right_to_left_get());
    }
  }
  // Produce chunks for all references.
  for (unsigned int rf = 0; rf < references.size(); rf++) {

    // Update progress bar.
    progresswindow.iterate();

    // Whether to keep things on one page.    
    if (keep_verses_together_within_page) {
      text2pdf.open_keep_together();
    }
    // Set main font, etc.
    text2pdf.set_font(main_font);
    text2pdf.set_line_spacing(main_line_spacing);
    text2pdf.set_right_to_left(main_right_to_left);

    // Add the reference to the text.
    text2pdf.add_text(references[rf].human_readable(""));

    // Map this verse to the original, that is, to Hebrew or Greek.
    vector <int> hebrew_greek_chapters;
    vector <int> hebrew_greek_verses;
    mapping.book_change(references[rf].book);
    mapping.me_to_original(references[rf].chapter, references[rf].verse, hebrew_greek_chapters, hebrew_greek_verses);
    // Get verse text for each version.
    for (unsigned int vsn = 0; vsn <= project_names.size(); vsn++) {

      // Add the font, etc., for each project.
      ustring font(main_font);
      unsigned int line_spacing = main_line_spacing;
      bool right_to_left = main_right_to_left;
      if (vsn > 0) {
        font = fonts[vsn - 1];
        line_spacing = line_spacings[vsn - 1];
        right_to_left = right_to_lefts[vsn - 1];
      }
      text2pdf.set_font(font);
      text2pdf.set_line_spacing(line_spacing);
      text2pdf.set_right_to_left(right_to_left);

      // Get the verse text.
      ustring line;
      if (vsn == 0) {
        // First version.
        ProjectBook *projectbook = main_project.get_book_pointer(references[rf].book);
        if (projectbook) {
          ProjectChapter *projectchapter = projectbook->get_chapter_pointer(references[rf].chapter);
          if (projectchapter) {
            ProjectVerse *projectverse = projectchapter->get_verse_pointer(references[rf].verse);
            if (projectverse) {
              line = projectverse->data;
            }
          }
        }
      } else {
        // Other versions. 
        // Get mapped chapters / verses.
        line.clear();
        vector <int> mychapters;
        vector <int> myverses;
        mapping_s[vsn - 1].book_change(references[rf].book);
        mapping_s[vsn - 1].original_to_me(hebrew_greek_chapters, hebrew_greek_verses, mychapters, myverses);
        // Get text of any of the mapped verses.
        for (unsigned int mp = 0; mp < mychapters.size(); mp++) {
          // Get the verse and add it to the usfm code.
          ProjectBook *projectbook = project_memories[vsn - 1].get_book_pointer(references[rf].book);
          if (projectbook) {
            ProjectChapter *projectchapter = projectbook->get_chapter_pointer(mychapters[mp]);
            if (projectchapter) {
              ProjectVerse *projectverse = projectchapter->get_verse_pointer(convert_to_string(myverses[mp]));
              if (projectverse) {
                if (!line.empty()) 
                  line.append (" ");
                line.append (projectverse->data);
              }
            }
          }
        }
      }

      // Add the project name if there are more than one.
      if (!project_names.empty()) {
        ustring project;
        if (vsn == 0)
          project = main_project.name;
        else
          project = project_names[vsn - 1];
        text2pdf.open_paragraph();
        text2pdf.inline_set_font_size_percentage(65);
        text2pdf.add_text(project);
        text2pdf.inline_clear_font_size_percentage();
        text2pdf.add_text(" ");
      } else {
        text2pdf.open_paragraph();
      }

      // Do text replacement.
      text_replacement(line);

      // Positions in the line, and lengths to highlight.
      vector < size_t > highlight_positions;
      vector < size_t > highlight_lengths;

      // Go through all the words to highlight.
      for (unsigned int i2 = 0; i2 < highlight_casesensitives.size(); i2++) {
        // Word to highlight
        ustring highlightword;
        if (highlight_casesensitives[i2])
          highlightword = highlight_words[i2];
        else
          highlightword = highlight_words[i2].casefold();
        // Variabele s holds a shadow string.
        ustring s;
        if (highlight_casesensitives[i2])
          s = line;
        else
          s = line.casefold();
        // Find positions for highlighting.
        size_t offposition = s.find(highlightword);
        while (offposition != string::npos) {
          // Store position and length.
          highlight_positions.push_back(offposition);
          highlight_lengths.push_back(highlightword.length());
          // Look for possible next word to highlight.
          offposition = offposition + highlightword.length() + 1;
          // There is something like a bug in s.find. If the offposition
          // is greater than the length of s, then s.find will return
          // a value below offposition, instead of string::npos as is
          // expected. Workaround.
          if (offposition > s.length())
            break;
          offposition = s.find(highlightword, offposition);
        }
      }
      // Sort the positions from small to bigger.
      xml_sort_positions(highlight_positions, highlight_lengths);
      // Combine overlapping positions.
      xml_combine_overlaps(highlight_positions, highlight_lengths);

      // Insert the code for highlighting.
      for (int i = highlight_positions.size () - 1; i >= 0; i--) {
        for (int i2 = highlight_lengths.size() - 1; i2 >= 0; i2--) {
          line.insert (highlight_positions[i] + i2, INSERTION_FLAG);
        }
      }

      // Add usfm converter to the layout engine, and set various things.
      Usfm2Text usfm2text(&text2pdf, false);
      usfm2text.add_styles(usfm2xslfo_read_stylesheet(stylesheet));

      usfm2text.no_bold();
      usfm2text.no_space_before_or_after();
      usfm2text.no_new_page();
      usfm2text.add_usfm_code(line);
      usfm2text.process();
    }

    // Add a bit of space.
    text2pdf.close_paragraph();
    text2pdf.add_text(" ");
    text2pdf.close_paragraph();

    // Whether to close code keeping things on one page.    
    if (keep_verses_together_within_page) {
      text2pdf.close_keep_together();
    }
  }

  // Hide progeressbar.
  progresswindow.hide();

  // Process the data.
  text2pdf.run();

  // Display the pdf file.
  text2pdf.view();

  // Log: ready.
  gw_message("Ready printing the parallel references");
}
