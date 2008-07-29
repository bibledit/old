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
#include "print_parallel_references.h"
#include "utilities.h"
#include "bible.h"
#include "usfm.h"
#include "usfmtools.h"
#include "pdfviewer.h"
#include "xmlutils.h"
#include "paper.h"
#include "xep.h"
#include "xmlfo-utils.h"
#include "constants.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "directories.h"
#include "fonts.h"
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
#include "usfm2xslfo.h"
#include "usfm2text.h"

void view_parallel_references_pdf(ProjectMemory& main_project, vector <ustring> * extra_projects, vector <Reference> references, bool keep_verses_together_within_page, vector<ustring> * remarks, bool highlight)
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
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig(main_project.name);
  ustring stylesheet = projectconfig->stylesheet_get();

  // Store the additional projects to print.
  vector<ustring> additional_projects;
  if (extra_projects)
    additional_projects = *extra_projects;
  settings->session.additional_printing_projects = additional_projects;

  // Prepare for mapping.
  Mapping mapping(projectconfig->versification_get(), 0);

  // The converter.
  Usfm2XslFo usfm2xslfo(gw_build_filename(directories_get_temp(), "usfm2pdf.pdf"));
  usfm2xslfo.add_styles(usfm2xslfo_read_stylesheet(stylesheet));
  PrintingFonts printingfonts(main_project.name);
  usfm2xslfo.set_fonts(printingfonts.printing_families, projectconfig->printing_font_size_get());
  usfm2xslfo.set_line_height(projectconfig->printing_line_height_get());
  if (projectconfig->right_to_left_get())
    usfm2xslfo.set_right_to_left();
  usfm2xslfo.set_page_size(settings->genconfig.paper_width_get(), settings->genconfig.paper_height_get());
  usfm2xslfo.set_margins(settings->genconfig.paper_top_margin_get(), settings->genconfig.paper_bottom_margin_get(), settings->genconfig.paper_inside_margin_get(), settings->genconfig.paper_outside_margin_get());
  if (settings->genconfig.printdate_get())
    usfm2xslfo.set_print_date();
  usfm2xslfo.no_bold();
  usfm2xslfo.no_space_before_or_after();
  usfm2xslfo.no_new_page();

  // Start off with inserting any remarks.
  if (remarks) {
    for (unsigned int r = 0; r < remarks->size(); r++) {
      usfm2xslfo.add_usfm_code(usfm_get_full_opening_marker(usfm2xslfo.default_style()));
      usfm2xslfo.add_usfm_code(remarks->at(r));
    }
  }

  // Some variables to avoid excessive session access during highlighting.
  vector<bool> highlight_casesensitives;
  vector<ustring> highlight_words;
  if (highlight) {
    for (unsigned int hl = 0; hl < settings->session.highlights.size(); hl++) {
      highlight_casesensitives.push_back(settings->session.highlights[hl].casesensitive);
      highlight_words.push_back(settings->session.highlights[hl].word);
    }
  }

  // All the projects to be put in this parallel Bible, together with
  // their related information, like mapping, fonts.
  vector <ustring> project_names;
  vector <ProjectMemory> project_memories;
  vector <Mapping> mapping_s;
  vector <ustring> fonts;
  vector <unsigned int> fontsizes;
  vector <unsigned int> lineheights;
  if (extra_projects) {
    vector<ustring> project_s_raw = *extra_projects;
    for (unsigned int i = 0; i < project_s_raw.size(); i++) {
      ProjectMemory projectmemory(project_s_raw[i], true);
      project_memories.push_back(projectmemory);
      ProjectConfiguration * projectconfig = settings->projectconfig(project_s_raw[i]);
      project_names.push_back(project_s_raw[i]);
      Mapping mapping(projectconfig->versification_get(), 0);
      mapping_s.push_back(mapping);
      PrintingFonts printingfonts(project_s_raw[i]);
      fonts.push_back(printingfonts.printing_families_comma_separated());
      fontsizes.push_back(projectconfig->printing_font_size_get());
      lineheights.push_back(projectconfig->printing_line_height_get());
    }
  }

  // Produce chunks for the xsl-fo file for all references.
  for (unsigned int rf = 0; rf < references.size(); rf++) {

    // Update progress bar.
    progresswindow.iterate();

    // Whether to keep things on one page.    
    if (keep_verses_together_within_page)
      usfm2xslfo.add_usfm_code(usfm_get_full_opening_marker(usfm2xslfo.keep_together_on_page_style()));

    // Open two spacing paragraphs.
    usfm2xslfo.add_usfm_code(usfm_get_full_opening_marker(usfm2xslfo.line_spacing_style()));
    usfm2xslfo.add_usfm_code(".");
    usfm2xslfo.add_usfm_code(usfm_get_full_opening_marker(usfm2xslfo.line_spacing_style()));

    // Add the reference to the text.
    usfm2xslfo.add_usfm_code(references[rf].human_readable(""));

    // Map this verse to the original, that is, to Hebrew or Greek.
    vector<int> hebrew_greek_chapters;
    vector<int> hebrew_greek_verses;
    mapping.book_change(references[rf].book);
    mapping.me_to_original(references[rf].chapter, references[rf].verse, hebrew_greek_chapters, hebrew_greek_verses);
    // Get verse text for each version.
    for (unsigned int vsn = 0; vsn <= project_names.size(); vsn++) {

      // Get the verse text.
      ustring line;
      if (vsn == 0) {
        // First version.
        ProjectBook * projectbook = main_project.get_book_pointer(references[rf].book);
        if (projectbook) {
          ProjectChapter * projectchapter = projectbook->get_chapter_pointer(references[rf].chapter);
          if (projectchapter) {
            ProjectVerse * projectverse = projectchapter->get_verse_pointer(references[rf].verse);
            if (projectverse) {
              line = projectverse->data;
            }
          }
        }
      } else {
        // Other versions. 
        // Get mapped chapters / verses.
        vector<int> mychapters;
        vector<int> myverses;
        mapping_s[vsn - 1].book_change(references[rf].book);
        mapping_s[vsn - 1].original_to_me(hebrew_greek_chapters, hebrew_greek_verses, mychapters, myverses);
        // Get text of any of the mapped verses.
        for (unsigned int mp = 0; mp < mychapters.size(); mp++) {
          // Add new paragraph to the usfm code.
          usfm2xslfo.add_usfm_code(usfm_get_full_opening_marker(usfm2xslfo.default_style()));
          // Get the verse and add it to the usfm code.
          ProjectBook * projectbook = project_memories [vsn - 1].get_book_pointer(references[rf].book);
          if (projectbook) {
            ProjectChapter * projectchapter = projectbook->get_chapter_pointer(mychapters[mp]);
            if (projectchapter) {
              ProjectVerse * projectverse = projectchapter->get_verse_pointer(convert_to_string(myverses[mp]));
              if (projectverse) {
                line = projectverse->data;
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
        usfm2xslfo.add_usfm_code(project + " ");
      }

      // Do text replacement.
      text_replacement(line);

      // Positions in the line, and lengths to highlight.
      vector <size_t> highlight_positions;
      vector <size_t> highlight_lengths;

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
      for (unsigned int i = 0; i < highlight_positions.size(); i++) {
        // Text to insert to highlight it.
        ustring taggedtext = usfm_get_full_opening_marker(INSERTION_MARKER) + line.substr(highlight_positions[i], highlight_lengths[i]) + usfm_get_full_closing_marker(INSERTION_MARKER);
        // Insert tag.
        line.replace(highlight_positions[i], highlight_lengths[i], taggedtext);
        // Push any following positions up.
        for (unsigned int i2 = i + 1; i2 < highlight_positions.size(); i2++) {
          highlight_positions[i2] = highlight_positions[i2] + usfm_get_full_opening_marker (INSERTION_MARKER).length() + usfm_get_full_closing_marker (INSERTION_MARKER).length();
        }
      }

      // Add the font family and size for each additional project. 
      if (vsn > 0) {
        ustring s;
        s = usfm_get_full_opening_marker(usfm2xslfo.font_family_size_line_height_style());
        s.append(" ");
        s.append(fonts[vsn-1]);
        s.append(" ");
        s.append(convert_to_string(fontsizes[vsn-1]));
        s.append(" ");
        s.append(convert_to_string(lineheights[vsn-1]));
        usfm2xslfo.add_usfm_code(s);
      }

      // Add usfm code of the verse text to the converter.
      usfm2xslfo.add_usfm_code(line);

      // If font information was added, close that marker here.
      if (vsn > 0) {
        usfm2xslfo.add_usfm_code(usfm_get_full_closing_marker(usfm2xslfo.font_family_size_line_height_style()));
      }
    }

    // Close the two spacing paragraphs.
    usfm2xslfo.add_usfm_code(usfm_get_full_closing_marker(usfm2xslfo.line_spacing_style()));
    usfm2xslfo.add_usfm_code(usfm_get_full_closing_marker(usfm2xslfo.line_spacing_style()));

    // Whether to close code keeping things on one page.    
    if (keep_verses_together_within_page)
      usfm2xslfo.add_usfm_code(usfm_get_full_closing_marker(usfm2xslfo.keep_together_on_page_style()));
  }

  // Hide progeressbar.
  progresswindow.hide();

  // Process the data.
  usfm2xslfo.process();

  // Display the pdf file.
  usfm2xslfo.display(NULL);

  // Log: ready.
  gw_message("Ready printing the parallel references");
}

void view_parallel_references_pdf2(ProjectMemory& main_project, vector <ustring> * extra_projects, vector <Reference> references, bool keep_verses_together_within_page, vector<ustring> * remarks, bool highlight)
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
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig(main_project.name);
  ustring stylesheet = projectconfig->stylesheet_get();

  // Store the additional projects to print.
  vector<ustring> additional_projects;
  if (extra_projects)
    additional_projects = *extra_projects;
  settings->session.additional_printing_projects = additional_projects;

  // Prepare for mapping.
  Mapping mapping(projectconfig->versification_get(), 0);

  // The converter.
  Text2Pdf text2pdf(gw_build_filename(directories_get_temp(), "document.pdf"));
  
  // Page.
  text2pdf.page_size_set(settings->genconfig.paper_width_get(), settings->genconfig.paper_height_get());
  text2pdf.page_margins_set (settings->genconfig.paper_inside_margin_get(), settings->genconfig.paper_outside_margin_get(), settings->genconfig.paper_top_margin_get(), settings->genconfig.paper_bottom_margin_get());
  text2pdf.page_one_column_only();

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
  vector<bool> highlight_casesensitives;
  vector<ustring> highlight_words;
  if (highlight) {
    for (unsigned int hl = 0; hl < settings->session.highlights.size(); hl++) {
      highlight_casesensitives.push_back(settings->session.highlights[hl].casesensitive);
      highlight_words.push_back(settings->session.highlights[hl].word);
    }
  }

  // All the projects to be put in this parallel Bible, together with
  // their related information, like mapping, fonts.
  vector <ustring> project_names;
  vector <ProjectMemory> project_memories;
  vector <Mapping> mapping_s;
  vector <ustring> fonts;
  vector <unsigned int> fontsizes;
  vector <unsigned int> lineheights;
  if (extra_projects) {
    vector<ustring> project_s_raw = *extra_projects;
    for (unsigned int i = 0; i < project_s_raw.size(); i++) {
      ProjectMemory projectmemory(project_s_raw[i], true);
      project_memories.push_back(projectmemory);
      ProjectConfiguration * projectconfig = settings->projectconfig(project_s_raw[i]);
      project_names.push_back(project_s_raw[i]);
      Mapping mapping(projectconfig->versification_get(), 0);
      mapping_s.push_back(mapping);
      PrintingFonts printingfonts(project_s_raw[i]);
      fonts.push_back(printingfonts.printing_families_comma_separated());
      fontsizes.push_back(projectconfig->printing_font_size_get());
      lineheights.push_back(projectconfig->printing_line_height_get());
    }
  }

  // Produce chunks for the xsl-fo file for all references.
  for (unsigned int rf = 0; rf < references.size(); rf++) {

    // Update progress bar.
    progresswindow.iterate();

    // Whether to keep things on one page.    
    if (keep_verses_together_within_page) {
      text2pdf.open_keep_together();
    }

    // Add the reference to the text.
    text2pdf.add_text(references[rf].human_readable(""));

    // Map this verse to the original, that is, to Hebrew or Greek.
    vector<int> hebrew_greek_chapters;
    vector<int> hebrew_greek_verses;
    mapping.book_change(references[rf].book);
    mapping.me_to_original(references[rf].chapter, references[rf].verse, hebrew_greek_chapters, hebrew_greek_verses);
    // Get verse text for each version.
    for (unsigned int vsn = 0; vsn <= project_names.size(); vsn++) {

      // Get the verse text.
      ustring line;
      if (vsn == 0) {
        // First version.
        ProjectBook * projectbook = main_project.get_book_pointer(references[rf].book);
        if (projectbook) {
          ProjectChapter * projectchapter = projectbook->get_chapter_pointer(references[rf].chapter);
          if (projectchapter) {
            ProjectVerse * projectverse = projectchapter->get_verse_pointer(references[rf].verse);
            if (projectverse) {
              line = projectverse->data;
            }
          }
        }
      } else {
        // Other versions. 
        // Get mapped chapters / verses.
        vector<int> mychapters;
        vector<int> myverses;
        mapping_s[vsn - 1].book_change(references[rf].book);
        mapping_s[vsn - 1].original_to_me(hebrew_greek_chapters, hebrew_greek_verses, mychapters, myverses);
        // Get text of any of the mapped verses.
        for (unsigned int mp = 0; mp < mychapters.size(); mp++) {
          // Get the verse and add it to the usfm code.
          ProjectBook * projectbook = project_memories [vsn - 1].get_book_pointer(references[rf].book);
          if (projectbook) {
            ProjectChapter * projectchapter = projectbook->get_chapter_pointer(mychapters[mp]);
            if (projectchapter) {
              ProjectVerse * projectverse = projectchapter->get_verse_pointer(convert_to_string(myverses[mp]));
              if (projectverse) {
                line = projectverse->data;
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
      vector <size_t> highlight_positions;
      vector <size_t> highlight_lengths;

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
      for (unsigned int i = 0; i < highlight_positions.size(); i++) {
        // Text to insert to highlight it.
        ustring taggedtext = usfm_get_full_opening_marker(INSERTION_MARKER) + line.substr(highlight_positions[i], highlight_lengths[i]) + usfm_get_full_closing_marker(INSERTION_MARKER);
        // Insert tag.
        line.replace(highlight_positions[i], highlight_lengths[i], taggedtext);
        // Push any following positions up.
        for (unsigned int i2 = i + 1; i2 < highlight_positions.size(); i2++) {
          highlight_positions[i2] = highlight_positions[i2] + usfm_get_full_opening_marker (INSERTION_MARKER).length() + usfm_get_full_closing_marker (INSERTION_MARKER).length();
        }
      }

      // Add the font family and size for each additional project. Todo do in a different way. 
      if (vsn > 0) {
        /*
        ustring s;
        s = usfm_get_full_opening_marker(usfm2text.font_family_size_line_height_style());
        s.append(" ");
        s.append(fonts[vsn-1]);
        s.append(" ");
        s.append(convert_to_string(fontsizes[vsn-1]));
        s.append(" ");
        s.append(convert_to_string(lineheights[vsn-1]));
        usfm2text.add_usfm_code(s);
        */
      }

      // Add usfm code of the verse text to the converter. Todo implement this.
      Usfm2Text usfm2text(&text2pdf, false);
      usfm2text.add_styles(usfm2xslfo_read_stylesheet(stylesheet));
      /* Todo
      PrintingFonts printingfonts(main_project.name);
      usfm2text.set_fonts(printingfonts.printing_families, projectconfig->printing_font_size_get());
      usfm2text.set_line_height(projectconfig->printing_line_height_get());
      if (projectconfig->right_to_left_get())
        usfm2text.set_right_to_left();
      if (settings->genconfig.printdate_get())
        usfm2text.set_print_date();
      */
      usfm2text.no_bold();
      usfm2text.no_space_before_or_after();
      usfm2text.no_new_page();
      usfm2text.add_usfm_code(line);
      usfm2text.process();  
    }

    // Add a bit of space.
    text2pdf.close_paragraph();
    text2pdf.add_text (" ");
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
