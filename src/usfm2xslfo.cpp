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

#include "usfm2xslfo.h"
#include "gwrappers.h"
#include <glib.h>
#include "utilities.h"
#include "tiny_utilities.h"
#include "date_time_utils.h"
#include "usfmtools.h"
#include "constants.h"
#include "books.h"
#include "color.h"
#include "directories.h"
#include "gtkwrappers.h"
#include "pdfviewer.h"
#include "settings.h"
#include "referenceutils.h"
#include "projectutils.h"

Usfm2XslFo::Usfm2XslFo(const ustring& pdfoutputfile) {
  // Initialize variables.
  pdffile = pdfoutputfile;
  font_size = 12;
  line_height = 100;
  right_to_left = false;
  page_width = 21.0;
  page_height = 29.7;
  top_margin = 2;
  bottom_margin = 2;
  inner_margin = 2;
  outer_margin = 2;
  two_columns = false;
  even_page_count = false;
  chapter_number_in_running_header_at_left_pages = true;
  chapter_number_in_running_header_at_right_pages = true;
  print_date = false;
  usfm_buffer_pointer = 0;
  chapter_number_to_output_at_first_verse = 0;
  progresswindow = NULL;
  progress_event_id = 0;
  progresswindow = new ProgressWindow ("Processing", true);
  progress_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc (signal_progress_timeout), gpointer(this), NULL);
  cancel = false;
  printversenumber = true;
  book = 0;
  chapter = 0;
  verse = "0";
  id_page_break_count = 0;
  book_spans_columns = false;
  endnote_position = eptAfterEverything;
  do_not_allow_new_page = false;
  include_full_references_with_notes = false;
  verses_in_paragraph_count = 0;

  // Add a couple of extra styles.
  add_style(default_style(), u2xtParagraphNormalParagraph);
  add_style(column_balancing_style(), u2xtColumnBalancer);
  add_style(ELASTIC_MARKER, u2xtElastic);
  add_style(INSERTION_MARKER, u2xtInsertion);
  add_style(DELETION_MARKER, u2xtDeletion);
  add_style(line_spacing_style(), u2xtLineSpacing);
  add_style(keep_together_on_page_style(), u2xtKeepOnPage);
  add_style(font_family_size_line_height_style(), u2xtFontFamilySizeLineHeight);
}

Usfm2XslFo::~Usfm2XslFo() {
  gw_destroy_source(progress_event_id);
  if (progresswindow)
    delete progresswindow;
}

void Usfm2XslFo::add_usfm_code(const vector <ustring>& lines)
// Add some usfm code to the object's usfm input buffer.
{
  for (unsigned int i = 0; i < lines.size(); i++) {
    add_usfm_code(lines[i]);
  }
}

void Usfm2XslFo::add_usfm_code(const ustring& line)
// Add some usfm code to the object's usfm input buffer.
{
  usfm_code.push_back(line);
}

void Usfm2XslFo::preprocess()
// Does the preprocessing of the usfm data, 
// extracting all sorts of information from it.
{
  // Variable for dumping endnotes.
  bool endnote_dump_marker_encountered = false;

  // Variables for going through the usfm input.
  usfm_buffer_pointer = 0;
  ustring marker;
  size_t marker_position;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  unsigned int book = 0;
  unsigned int chapter = 0;

  // Go through all usfm input.
  while (unprocessed_usfm_code_available()) {
    if (cancel)
      return;
    usfm_line = get_next_chapter();
    while (gtk_events_pending())
      gtk_main_iteration();
    while (!usfm_line.empty()) {
      bool processed = false;
      marker_found = usfm_search_marker(usfm_line, marker, marker_position, marker_length, is_opener);
      if (marker_found) {
        if (marker_position == 0) {
          if (is_opener) {
            Usfm2XslFoStyle * stylepointer = marker_get_pointer_to_style(marker);
            if (stylepointer) {
              switch (stylepointer->type)
              {
                case u2xtIdentifierBook:
                {
                  // Extract the book from the id string.
                  ustring id_line = get_erase_code_till_next_marker(usfm_line, 0, marker_length, true);
                  if (id_line.length() >= 3) {
                    id_line = id_line.substr(0, 3);
                    book = books_paratext_to_id(id_line);
                    // Initialize the highest chapter to 0.
                    highest_chapter_number [book] = 0;
                    // Store the book in a list for later use.
                    if (book)
                      books.push_back(book);
                    // Reset chapter.  
                    chapter = 0;
                  }
                  // Data was processed.
                  processed = true;
                  break;
                }
                case u2xtIdentifierComment:
                {
                  break;
                }
                case u2xtIdentifierRunningHeader:
                {
                  collect_running_headers(usfm_line, stylepointer, marker_length, book);
                  processed = true;
                  break;
                }
                case u2xtIdentifierLongTOC:
                {
                  toc_collect_text(usfm_line, marker_length, true, book);
                  processed = true;
                  break;
                }
                case u2xtIdentifierShortTOC:
                {
                  toc_collect_text(usfm_line, marker_length, false, book);
                  processed = true;
                  break;
                }
                case u2xtIdentifierBookAbbreviation:
                case u2xtParagraphMainTitle:
                case u2xtParagraphSubTitle:
                case u2xtParagraphSectionHeading:
                case u2xtParagraphNormalParagraph:
                case u2xtInlineText:
                {
                  break;
                }
                case u2xtChapterNumber:
                {
                  // Get the highest chapter number in the current book.
                  ustring chaptertext = get_erase_code_till_next_marker(usfm_line, 0, marker_length, true);
                  chapter = convert_to_int(chaptertext);
                  highest_chapter_number [book] = chapter;
                  // Whether to print the chapter number in the left/right headers.
                  chapter_number_in_running_header_at_left_pages = stylepointer->print_in_left_running_header;
                  chapter_number_in_running_header_at_right_pages = stylepointer->print_in_right_running_header;
                  // Data was processed.
                  processed = true;
                  break;
                }
                case u2xtVerseNumber:
                case u2xtFootNoteStart:
                {
                  break;
                }
                case u2xtEndNoteStart:
                {
                  if (!endnote_dump_marker_encountered) {
                    add_style(stylepointer->dump_endnotes_upon_encountering_this_marker, u2xtDumpEndnotes);
                    endnote_position = stylepointer->endnote_position_type;
                    endnote_dump_marker_encountered = true;
                  }
                  break;
                }
                case u2xtFootEndNoteStandardContent:
                case u2xtFootEndNoteContent:
                case u2xtFootEndNoteContentWithEndmarker:
                case u2xtFootEndNoteParagraph:
                case u2xtCrossreferenceStart:
                case u2xtCrossreferenceStandardContent:
                case u2xtCrossreferenceContent:
                case u2xtCrossreferenceContentWithEndmarker:
                case u2xtPublication:
                case u2xtTableOfContents:
                case u2xtPreface:
                case u2xtIntroduction:
                case u2xtGlossary:
                case u2xtConcordance:
                case u2xtIndex:
                case u2xtMapIndex:
                case u2xtCover:
                case u2xtSpine:
                case u2xtPicture:
                case u2xtPageBreak:
                case u2xtTableElementRow:
                case u2xtTableElementHeading:
                case u2xtTableElementCell:
                case u2xtColumnBalancer:
                case u2xtElastic:
                case u2xtDumpEndnotes:
                case u2xtInsertion:
                case u2xtDeletion:
                case u2xtLineSpacing:
                case u2xtKeepOnPage:
                case u2xtFontFamilySizeLineHeight:
                {
                  break;
                }
                case u2xtChapterLabel:
                {
                  // Get / store the chapter label.
                  ustring label = get_erase_code_till_next_marker(usfm_line, 0, marker_length, true);
                  ChapterLabel chapterlabel(book, chapter, label);
                  chapter_labels.push_back(chapterlabel);
                  // Data was processed.
                  processed = true;
                  break;
                }
                case u2xtPublishedChapterMarker:
                {
                  // Get / store the chapter label.
                  ustring label = get_erase_code_till_next_marker(usfm_line, 0, marker_length, true);
                  ChapterLabel publishedchaptermarker(book, chapter, label);
                  published_chapter_markers.push_back(publishedchaptermarker);
                  // Data was processed.
                  processed = true;
                  break;
                }
                case u2xtIdentifierCommentWithEndmarker:
                case u2xtGeneralWordlistEntry:
                case u2xtHebrewWordlistEntry:
                case u2xtGreekWordlistEntry:
                case u2xtSubjectIndexEntry:
                {
                  break;
                }
              }
            }
          }
        }
      }
      // Default for unprocessed text.
      if (!processed) {
        get_erase_code_till_next_marker(usfm_line, marker_position, marker_length, false);
      }
    }
  }
}

void Usfm2XslFo::process() {
}

void Usfm2XslFo::set_page_size(double width_cm, double height_cm)
// Sets the page size, width and height, in cm.
{
  page_width = width_cm;
  page_height = height_cm;
}

void Usfm2XslFo::set_margins(double top_cm, double bottom_cm, double inner_cm, double outer_cm)
// Sets the page margins, in cm.
{
  top_margin = top_cm;
  bottom_margin = bottom_cm;
  inner_margin = inner_cm;
  outer_margin = outer_cm;
}

void Usfm2XslFo::set_fonts(const vector <ustring>& fonts_in, double font_size_in) {
  fonts = fonts_in;
  font_size = font_size_in;
}

void Usfm2XslFo::write_root_open()
// Writes the fo:root element, leaving it open.
{
  xmlTextWriterStartElement(writer, BAD_CAST "fo:root");

  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:fo", BAD_CAST "http://www.w3.org/1999/XSL/Format");

  /*
   In XEP, you can specify multiple font-families in font-family property and if 
   font-selection-strategy is set to character-by-character value, XEP can 
   substitute glyphs missing in the first specified font with characters from other 
   fonts in the list.
   */
  ustring font;
  for (unsigned int i = 0; i < fonts.size(); i++) {
    if (i)
      font.append(",");
    font.append(fonts[i]);
  }
  xmlTextWriterWriteAttribute(writer, BAD_CAST "font-family", BAD_CAST font.c_str ());
  if (fonts.size() > 1)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-selection-strategy", BAD_CAST "character-by-character");

  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-size", "%gpt", font_size);

  xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align", BAD_CAST "justify");

  // If the line height is 100%, do not insert anything, because appears to 
  // disturb proper calculation of the line heights with different font sizes.
  if (line_height != 100) {
    ustring lineheight = convert_to_string(line_height) + "%";
    xmlTextWriterWriteAttribute(writer, BAD_CAST "line-height", BAD_CAST lineheight.c_str());
  }

  // Possible right-to-left text flow.
  if (right_to_left) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "writing-mode", BAD_CAST "rl-tb");
  }
}

void Usfm2XslFo::set_line_height(unsigned int line_height_in) {
  line_height = line_height_in;
}

void Usfm2XslFo::set_right_to_left() {
  right_to_left = true;
}

void Usfm2XslFo::set_two_columns() {
  two_columns = true;
}

void Usfm2XslFo::write_layout_master_set() {
  xmlTextWriterStartElement(writer, BAD_CAST "fo:layout-master-set");

  // Page sizes.
  ustring pagewidth = convert_to_string(page_width) + "cm";
  ustring pageheight = convert_to_string(page_height) + "cm";

  // A blank page master. It is there so as not to print headers on a blank page.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:simple-page-master");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-name", BAD_CAST "blank-page");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "page-width", BAD_CAST pagewidth.c_str ());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "page-height", BAD_CAST pageheight.c_str ());
  // XEP 4 only validates if the region-body is empty, but counts white-space
  // as non-empty space. Strange behaviour, but solved so:
  xmlTextWriterStartElement(writer, BAD_CAST "fo:region-body");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:region-before");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "region-name", BAD_CAST "header-blank");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "extent", BAD_CAST "0cm");
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // The page master for the odd page.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:simple-page-master");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-name", BAD_CAST "odd-page");
  // Fill in paper size, margins.
  xmlTextWriterWriteAttribute(writer, BAD_CAST "page-width", BAD_CAST pagewidth.c_str ());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "page-height", BAD_CAST pageheight.c_str ());
  xmlTextWriterStartElement(writer, BAD_CAST "fo:region-body");
  ustring topmargin = convert_to_string(top_margin) + "cm";
  ustring bottommargin = convert_to_string(bottom_margin) + "cm";
  ustring innermargin = convert_to_string(inner_margin) + "cm";
  ustring outermargin = convert_to_string(outer_margin) + "cm";
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-top", BAD_CAST topmargin.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-bottom", BAD_CAST bottommargin.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-left", BAD_CAST innermargin.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-right", BAD_CAST outermargin.c_str());
  if (two_columns)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "column-count", BAD_CAST "2");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:region-before");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "region-name", BAD_CAST "header-odd");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "extent", BAD_CAST topmargin.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // The page master for the even page.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:simple-page-master");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-name", BAD_CAST "even-page");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "page-width", BAD_CAST pagewidth.c_str ());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "page-height", BAD_CAST pageheight.c_str ());
  // Mirrored paper : swap left/right margins on even pages.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:region-body");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-top", BAD_CAST topmargin.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-bottom", BAD_CAST bottommargin.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-left", BAD_CAST outermargin.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "margin-right", BAD_CAST innermargin.c_str());
  if (two_columns)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "column-count", BAD_CAST "2");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:region-before");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "region-name", BAD_CAST "header-even");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "extent", BAD_CAST topmargin.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // The page sequence master.  
  xmlTextWriterStartElement(writer, BAD_CAST "fo:page-sequence-master");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-name", BAD_CAST "text");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:repeatable-page-master-alternatives");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:conditional-page-master-reference");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "blank-or-not-blank", BAD_CAST "blank");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-reference", BAD_CAST "blank-page");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:conditional-page-master-reference");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "odd-or-even", BAD_CAST "odd");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-reference", BAD_CAST "odd-page");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:conditional-page-master-reference");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "odd-or-even", BAD_CAST "even");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-reference", BAD_CAST "even-page");
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // Close the layout master set.  
  xmlTextWriterEndElement(writer);
}

void Usfm2XslFo::set_even_page_count()
// This forces an even page count in the page sequence.
{
  even_page_count = true;
}

void Usfm2XslFo::write_page_sequence_open()
// Write the page sequence, and leave the element open.
{
  xmlTextWriterStartElement(writer, BAD_CAST "fo:page-sequence");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "master-reference", BAD_CAST "text");
  if (even_page_count)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "force-page-count", BAD_CAST "end-on-even");
}

void Usfm2XslFo::set_print_date() {
  print_date = true;
}

void Usfm2XslFo::write_static_content() {
  // Today's date.
  ustring date = date_time_julian_human_readable(date_time_julian_day_get_current(), false);

  // Margins.
  ustring innermargin = convert_to_string(inner_margin) + "cm";
  ustring outermargin = convert_to_string(outer_margin) + "cm";
  ustring tableheight = convert_to_string(top_margin - 0.2) + "cm";

  // Blank pages.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:static-content");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "flow-name", BAD_CAST "header-blank");
  xmlTextWriterEndElement(writer);

  // Odd pages.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:static-content");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "flow-name", BAD_CAST "header-odd");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "table-layout", BAD_CAST "fixed");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "width", BAD_CAST "100%");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST innermargin.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST "proportional-column-width(1)");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST "proportional-column-width(1)");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST outermargin.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-body");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-row");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "height", BAD_CAST tableheight.c_str());
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "display-align", BAD_CAST "after");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align", BAD_CAST "start");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:page-number");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:leader");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "leader-length", BAD_CAST "1cm");
  xmlTextWriterEndElement(writer);
  if (print_date)
    xmlTextWriterWriteFormatString(writer, date.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "display-align", BAD_CAST "after");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align", BAD_CAST "end");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:retrieve-marker");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "retrieve-class-name", BAD_CAST "rightheader");
  xmlTextWriterEndElement(writer);
  xmlTextWriterWriteFormatString(writer, " ");
  if (chapter_number_in_running_header_at_right_pages) {
    xmlTextWriterStartElement(writer, BAD_CAST "fo:retrieve-marker");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "retrieve-class-name", BAD_CAST "chapter");
    xmlTextWriterEndElement(writer);
  }
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // Even pages.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:static-content");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "flow-name", BAD_CAST "header-even");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "table-layout", BAD_CAST "fixed");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "width", BAD_CAST "100%");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST outermargin.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST "proportional-column-width(1)");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST "proportional-column-width(1)");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-column");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "column-width", BAD_CAST innermargin.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-body");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-row");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "height", BAD_CAST tableheight.c_str());
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "display-align", BAD_CAST "after");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align", BAD_CAST "start");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:retrieve-marker");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "retrieve-class-name", BAD_CAST "leftheader");
  xmlTextWriterEndElement(writer);
  xmlTextWriterWriteFormatString(writer, " ");
  if (chapter_number_in_running_header_at_left_pages) {
    xmlTextWriterStartElement(writer, BAD_CAST "fo:retrieve-marker");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "retrieve-class-name", BAD_CAST "chapter");
    xmlTextWriterEndElement(writer);
  }
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "display-align", BAD_CAST "after");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align", BAD_CAST "end");
  if (print_date)
    xmlTextWriterWriteFormatString(writer, date.c_str());
  xmlTextWriterStartElement(writer, BAD_CAST "fo:leader");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "leader-length", BAD_CAST "1cm");
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:page-number");
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
}

void Usfm2XslFo::write_flow_open()
// Write the fo:flow, leaving it open.
{
  xmlTextWriterStartElement(writer, BAD_CAST "fo:flow");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "flow-name", BAD_CAST "xsl-region-body");
}

void Usfm2XslFo::add_styles(const vector <Usfm2XslFoStyle>& styles_in) {
  for (unsigned int i = 0; i < styles_in.size(); i++) {
    styles.push_back(styles_in[i]);
  }
}

void Usfm2XslFo::add_style(const ustring& marker_in, Usfm2XslFoStyleType type_in) {
  Usfm2XslFoStyle style(marker_in);
  style.type = type_in;
  styles.push_back(style);
}

bool Usfm2XslFo::unprocessed_usfm_code_available()
// Returns true if there still is some unprocessed usfm code available.
{
  return ((usfm_buffer_pointer) < usfm_code.size());
}

ustring Usfm2XslFo::get_next_chapter()
// Returns the next chapter from the available unprocessed usfm code.
{
  bool first_line = true;
  ustring chapter_marker("c");
  for (unsigned int i = 0; i < styles.size(); i++) {
    if (styles[i].type == u2xtChapterNumber)
      chapter_marker = styles[i].marker;
  }
  ustring chapter;
  while (unprocessed_usfm_code_available()) {
    ustring line = usfm_code [usfm_buffer_pointer];
    ustring marker = usfm_extract_marker(line);
    if (!first_line) {
      if (marker == chapter_marker) {
        return chapter;
      }
    }
    if (!first_line)
      chapter.append(" ");
    chapter.append(usfm_code [usfm_buffer_pointer]);
    first_line = false;
    usfm_buffer_pointer++;
    replace_text(chapter, "\n", " ");
  }
  return chapter;
}

void Usfm2XslFo::output_text_starting_new_paragraph(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool keep_with_next_paragraph)
/*
 This function deals with a marker that starts a paragraph.
 It does the administration that starting the paragraph requires.
 */
{
  // Remove the markup from the line.
  line.erase(0, marker_length);

  // If no printing, bail out.
  if (!inrange.in_range())
    return;

  // Close possible inline and block styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Set the new fo:block style.
  fo_block_style = stylepointer;
  open_fo_block(fo_block_style, keep_with_next_paragraph);
}

Usfm2XslFoStyle * Usfm2XslFo::marker_get_pointer_to_style(const ustring& marker)
// Returns the type of the marker.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    if (marker == styles[i].marker)
      return &styles[i];
  }
  return NULL;
}

void Usfm2XslFo::output_text_fallback(ustring& line, Usfm2XslFoStyle * & fo_block_style)
/*
 This function outputs text that was not handled by any other method.
 It is a fallback function that gets called when everything else has failed.
 It takes one character from the available text, and moves it to the output.
 By taking only one character at a time it is hoped that one of the next times,
 the lines gives enough markup information again to be handled properly.
 */
{
  // Ensure that a fo:block is open, if we print this portion.
  if (inrange.in_range()) {
    ensure_fo_block_open(fo_block_style);
    xmlTextWriterWriteFormatString(writer, line.substr (0, 1).c_str());
  }
  line.erase(0, 1);
}

void Usfm2XslFo::open_fo_block(Usfm2XslFoStyle * style, bool keep_with_next_paragraph) {
  // Open the block.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");

  // Fontsize
  ustring s = convert_to_string(style->fontsize) + "pt";
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-size", "%gpt", style->fontsize);

  // Italic
  if (style->italic == ON) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-style", BAD_CAST "italic");
  }

  // Bold
  if (style->bold == ON) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-weight", BAD_CAST "bold");
  }

  // Underline
  if (style->underline == ON) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "text-decoration", BAD_CAST "underline");
  }

  // Smallcaps
  if (style->smallcaps == ON) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-variant", BAD_CAST "small-caps");
  }

  // Justification
  if (style->justification == "last-justify")
    xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align-last", BAD_CAST "justify");
  else {
    if (!style->justification.empty()) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "text-align", BAD_CAST style->justification.c_str());
    }
  }

  // Space before
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "space-before", "%gmm", style->spacebefore);

  // Space after
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "space-after", "%gmm", style->spaceafter);

  // Left margin
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start-indent", "%gmm", style->leftmargin);

  // Right margin
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end-indent", "%gmm", style->rightmargin);

  // First line indent
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "text-indent", "%gmm", style->firstlineindent);

  // Spanning of columns.
  if ((style->spancolumns) || book_spans_columns)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "span", BAD_CAST "all");

  // Keeping with next paragraph.
  if (keep_with_next_paragraph)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "keep-with-next.within-column", BAD_CAST "always");

  // Prepare for possible float.
  xmlTextWriterWriteAttribute(writer, BAD_CAST "intrusion-displace", BAD_CAST "line");

  // Set widows and orphans.
  xmlTextWriterWriteAttribute(writer, BAD_CAST "widows", BAD_CAST "2");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "orphans", BAD_CAST "2");

  // Disregard line height adjustment as a result of e.g. superscript.
  xmlTextWriterWriteAttribute(writer, BAD_CAST "line-height-shift-adjustment", BAD_CAST "disregard-shifts");
}

void Usfm2XslFo::close_possible_fo_block(Usfm2XslFoStyle * & style) {
  // Bail out if no fo:block was open.
  if (style == NULL)
    return;

  // Close the block.
  xmlTextWriterEndElement(writer);
  style = NULL;
}

gchar * Usfm2XslFo::default_style()
// The name of the default style.
{
  return "usfm2xslfo_default_style";
}

void Usfm2XslFo::open_fo_inline(Usfm2XslFoStyle * style, Usfm2XslFoStyle * block) {
  // Variable for general use.
  ustring s;

  // Open the block.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:inline");

  // Fontsize
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-size", "%i%%", style->fontpercentage);

  // Italic
  s.clear();
  if (style->italic == ON)
    s = "italic";
  else if (style->italic == OFF)
    s =("normal");
  else if (style->italic == INHERIT)
    s = "inherit";
  else if (style->italic == TOGGLE) {
    if (block) {
      if (block->italic == ON)
        s = "normal";
      else
        s = "italic";
    }
  }
  if (!s.empty()) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-style", BAD_CAST s.c_str());
  }

  // Bold
  s.clear();
  if (style->bold == ON)
    s = "bold";
  else if (style->bold == OFF)
    s =("normal");
  else if (style->bold == INHERIT)
    s = "inherit";
  else if (style->bold == TOGGLE) {
    if (block) {
      if (block->bold == ON)
        s = "normal";
      else
        s = "bold";
    }
  }
  if (!s.empty()) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-weight", BAD_CAST s.c_str());
  }

  // Underline
  s.clear();
  if (style->underline == ON)
    s = "underline";
  else if (style->underline == OFF)
    s =("none");
  else if (style->underline == INHERIT)
    s = "inherit";
  else if (style->underline == TOGGLE) {
    if (block) {
      if (block->underline == ON)
        s = "none";
      else
        s = "underline";
    }
  }
  if (!s.empty()) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "text-decoration", BAD_CAST s.c_str());
  }

  // Smallcaps
  s.clear();
  if (style->smallcaps == ON)
    s = "small-caps";
  else if (style->smallcaps == OFF)
    s =("normal");
  else if (style->smallcaps == INHERIT)
    s = "inherit";
  else if (style->smallcaps == TOGGLE) {
    if (block) {
      if (block->smallcaps == ON)
        s = "normal";
      else
        s = "small-caps";
    }
  }
  if (!s.empty()) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "font-variant", BAD_CAST s.c_str());
  }

  // Superscript.
  if (style->superscript) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "vertical-align", BAD_CAST "super");
  }

  // Color
  if (style->color) {
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "color", "#%s", color_decimal_to_hex (style->color).c_str());
  }
}

void Usfm2XslFo::output_chapter_number_try_normal(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, unsigned int book)
// This function tries to output the chapter number at the normal position.
// Else it stores the data needed for outputting it later on at the first verse.
{
  // Bail out of this portion should not be printed.
  if (!inrange.in_range())
    return;

  // Close possible inline and block styles.  
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Insert marker for chapter number in the running headers.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterStartElement(writer, BAD_CAST "fo:marker");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "marker-class-name", "chapter");
  xmlTextWriterWriteFormatString(writer, "%i", chapter);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  // Print no chapter number for books with only one chapter.
  if (highest_chapter_number[book] == 1)
    return;

  // Whether to print the chapter number here, or at first verse.
  chapter_number_to_output_at_first_verse = 0;
  bool at_first_verse = stylepointer->print_chapter_number_at_first_verse;
  if (at_first_verse) {
    chapter_number_to_output_at_first_verse = chapter;
  }

  // Bail out if printing the chapter number later on, not here.
  if (at_first_verse)
    return;

  // Write the chapter number in a fo:block, and close that block again.
  // Normally this is the number as it is, 
  // but it can be modified if a published chapter marker is given.
  // Insert a possible chapter label too.
  fo_block_style = stylepointer;
  open_fo_block(fo_block_style, true);
  ustring chapterlabel;
  for (unsigned int i = 0; i < chapter_labels.size(); i++) {
    if (chapter_labels[i].book == book) {
      if ((chapter_labels[i].chapter == 0) || (chapter_labels[i].chapter == chapter)) {
        chapterlabel = chapter_labels[i].label;
      }
    }
  }
  if (!chapterlabel.empty()) {
    xmlTextWriterWriteFormatString(writer, "%s", chapterlabel.c_str());
    xmlTextWriterWriteFormatString(writer, " ");
  }
  ustring chapternumber = convert_to_string(chapter);
  for (unsigned int i = 0; i < published_chapter_markers.size(); i++) {
    if (book == published_chapter_markers[i].book) {
      if (chapter == published_chapter_markers[i].chapter) {
        chapternumber = published_chapter_markers[i].label;
      }
    }
  }
  xmlTextWriterWriteFormatString(writer, "%s", chapternumber.c_str());
  close_possible_fo_block(fo_block_style);
}

void Usfm2XslFo::output_chapter_number_try_at_first_verse(ustring line, Usfm2XslFoStyle * & fo_block_style) {
  // Bail out if there is no chapter to write.
  if (chapter_number_to_output_at_first_verse == 0)
    return;

  // This function is called at the marker of a normal paragraph. 
  // See if the next marker that comes after is a verse marker.
  // If so, write the chapter number there.
  ustring marker;
  size_t position;
  size_t length;
  bool is_opener;
  bool marker_found = usfm_search_marker(line, marker, position, length, is_opener);
  if (marker_found) {
    Usfm2XslFoStyle * stylepointer = marker_get_pointer_to_style(marker);
    if (stylepointer) {
      if (stylepointer->type == u2xtVerseNumber) {
        // We're now at the point that the chapter number can be written.

        // Ensure that a block is open.
        ensure_fo_block_open(fo_block_style);

        // Open the float.
        xmlTextWriterStartElement(writer, BAD_CAST "fo:float");
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "float", "start");

        // Search the chapter style.
        Usfm2XslFoStyle * chapter_style= NULL;
        for (unsigned int i = 0; i < styles.size(); i++) {
          if (styles[i].type == u2xtChapterNumber)
            chapter_style = &styles[i];
        }

        // Assemble chapter number.
        // Normally this is the number as it is, 
        // but it can be modified if a published chapter marker is given.
        ustring chapternumber = convert_to_string(chapter_number_to_output_at_first_verse);
        for (unsigned int i = 0; i < published_chapter_markers.size(); i++) {
          if (book == published_chapter_markers[i].book) {
            if (chapter_number_to_output_at_first_verse == published_chapter_markers[i].chapter) {
              chapternumber = published_chapter_markers[i].label;
            }
          }
        }

        // Write the chapter block.
        if (chapter_style) {
          open_fo_block(chapter_style, true);
          xmlTextWriterWriteFormatString(writer, "%s", chapternumber.c_str());
          close_possible_fo_block(chapter_style);
        }

        // Close the float.
        xmlTextWriterEndElement(writer);

        // The next verse number won't be written.
        printversenumber = false;

        // Done: clear the chapter number.
        chapter_number_to_output_at_first_verse = 0;
      }
    }
  }
}

void Usfm2XslFo::ensure_fo_block_open(Usfm2XslFoStyle *& style) {
  // If a block is open, bail out.
  if (style)
    return;

  // Open a block with the default style.
  style = marker_get_pointer_to_style(default_style());
  open_fo_block(style, false);
}

void Usfm2XslFo::close_possible_fo_inline(Usfm2XslFoStyle * & style) {
  // Close possible fo:inline style.
  if (style) {
    xmlTextWriterEndElement(writer);
    style = NULL;
  }
}

void Usfm2XslFo::output_text_running_header(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, unsigned int book) {
  // Close possible inline and block styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Erase the code: we don't use it here.
  get_erase_code_till_next_marker(line, 0, marker_length, false);

  // Open a modified default fo:block.
  Usfm2XslFoStyle * style = marker_get_pointer_to_style(default_style());
  ustring default_style_justification = style->justification;
  style->justification = "last-justify";
  open_fo_block(style, false);
  style->justification = default_style_justification;

  // Insert the left and right headers.  
  xmlTextWriterStartElement(writer, BAD_CAST "fo:marker");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "marker-class-name", "leftheader");
  xmlTextWriterWriteFormatString(writer, "%s", book_header_left[book].c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterStartElement(writer, BAD_CAST "fo:marker");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "marker-class-name", "rightheader");
  xmlTextWriterWriteFormatString(writer, "%s", book_header_right[book].c_str());
  xmlTextWriterEndElement(writer);

  // Close the block.  
  close_possible_fo_block(style);
}

void Usfm2XslFo::collect_running_headers(ustring& line, Usfm2XslFoStyle * stylepointer, size_t marker_length, unsigned int book)
// This function collects the running headers.
{
  // Extract running header.
  ustring runningheader = get_erase_code_till_next_marker(line, 0, marker_length, true);

  // Store left and/or right header.
  if (stylepointer->print_in_left_running_header) {
    if (!book_header_left[book].empty())
      book_header_left[book].append(" ");
    book_header_left[book].append(runningheader);
  }
  if (stylepointer->print_in_right_running_header) {
    if (!book_header_right[book].empty())
      book_header_right[book].append(" ");
    book_header_right[book].append(runningheader);
  }
}

bool Usfm2XslFo::signal_progress_timeout(gpointer user_data) {
  ((Usfm2XslFo *) user_data)->signal_progress();
  return true;
}

void Usfm2XslFo::signal_progress() {
  if (progresswindow) {
    progresswindow->pulse();
    if (progresswindow->cancel)
      cancel = true;
  }
}

void Usfm2XslFo::output_verse_number(Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length)
// Writes the verse number.
{
  // Bail out if this portion is not printed.
  if (!inrange.in_range())
    return;

  // Close possible inline style and ensure that a block is open.
  close_possible_fo_inline(fo_inline_style);
  ensure_fo_block_open(fo_block_style);

  // If the verse is to restart the paragraph, handle that here.
  if (verses_in_paragraph_count > 1) {
    if (stylepointer->restart_paragraph) {
      Usfm2XslFoStyle * style = fo_block_style;
      close_possible_fo_block(style);
      style = fo_block_style;
      open_fo_block(style, false);
    }
  }

  // Write any number different from 1.
  if (verse != "1")
    printversenumber = true;

  // Write the verse number.
  if (printversenumber && stylepointer->print) {
    fo_inline_style = stylepointer;
    open_fo_inline(fo_inline_style, fo_block_style);
    xmlTextWriterWriteFormatString(writer, verse.c_str());
    close_possible_fo_inline(fo_inline_style);
  }

  // Write next verse number again.
  printversenumber = true;
}

void Usfm2XslFo::output_text_character_style(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener)
/*
 This function deals with a marker that starts or ends a character style.
 It does the administration that it requires.
 */
{
  // Remove the markup from the line.
  line.erase(0, marker_length);

  // Bail out if the portion should not be printed.
  if (!inrange.in_range())
    return;

  // Ensure that a fo:block is open.
  ensure_fo_block_open(fo_block_style);

  // Close possible inline style.
  close_possible_fo_inline(fo_inline_style);

  // Set the new fo:inline style.
  if (is_opener) {
    fo_inline_style = stylepointer;
    open_fo_inline(fo_inline_style, fo_block_style);
  }
}

void Usfm2XslFo::output_text_table(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length)
// This function collects all table related data that is available in a row,
// and then processes that to output the complete table.
{

  // Pass 1: Collecting the raw usfm code bits.


  // Containers for all table related styles, and the texts belonging to them.
  vector <Usfm2XslFoStyle *> table_styles;
  vector <ustring> table_texts;

  // Go through the data collecting bits,
  // till we reach a marker that does not belong to the table,
  // or till we reach the end of the text.
  {
    bool in_table;
    do {
      in_table = false;
      ustring marker;
      size_t marker_position;
      size_t marker_length;
      bool is_opener;
      bool marker_found = usfm_search_marker(line, marker, marker_position, marker_length, is_opener);
      if (marker_found) {
        if (marker_position == 0) {
          Usfm2XslFoStyle * stylepointer = marker_get_pointer_to_style(marker);
          if (stylepointer) {
            if ((stylepointer->type == u2xtTableElementRow) || (stylepointer->type == u2xtTableElementHeading) || (stylepointer->type == u2xtTableElementCell)) {
              in_table = true;
              ustring text = get_erase_code_till_next_marker(line, marker_position, marker_length, true);
              table_styles.push_back(stylepointer);
              table_texts.push_back(text);
            }
          }
        }
      }
    } while (in_table && (!line.empty()));
  }

  // Bail out if there is nothing to do.
  if (table_styles.empty())
    return;

  // Bail out if we're not to print this portion.
  if (!inrange.in_range())
    return;

  // Pass 2: Assemble the table rows and cells.


  // Raw table data storage.
  vector <XslFoTableRow> rows_raw;

  {
    // Storage for one row.
    XslFoTableRow row(0);

    // Go through the buffer building cells and rows.
    for (unsigned int i = 0; i < table_styles.size(); i++) {
      if (table_styles[i]->type == u2xtTableElementRow) {
        if (!row.cells.empty())
          rows_raw.push_back(row);
        row.cells.clear();
      }
      if ((table_styles[i]->type == u2xtTableElementHeading) || (table_styles[i]->type == u2xtTableElementCell)) {
        XslFoTableCell cell(table_styles[i], table_texts[i]);
        row.cells.push_back(cell);
      }
    }

    // Save any data in the row.
    if (!row.cells.empty())
      rows_raw.push_back(row);
  }

  // Pass 3: Retrieve the highest column number in this table.


  unsigned int highest_column = 0;
  for (unsigned int r = 0; r < rows_raw.size(); r++) {
    for (unsigned int c = 0; c < rows_raw[r].cells.size(); c++) {
      Usfm2XslFoStyle * style = rows_raw[r].cells[c].style;
      if (style->table_column_number > (int) highest_column)
        highest_column = style->table_column_number;
    }
  }

  // Pass 4: Produce a tidied table.


  // This tidied table has the same amount of columns in each row.
  // It may have empty cells in cases there was no data for it.
  vector <XslFoTableRow> rows_tidy;
  for (unsigned int r = 0; r < rows_raw.size(); r++) {
    XslFoTableRow row(0);
    for (unsigned int c = 0; c < highest_column; c++) {
      XslFoTableCell cell(NULL, "");
      row.cells.push_back(cell);
    }
    for (unsigned int c = 0; c < rows_raw[r].cells.size(); c++) {
      Usfm2XslFoStyle * style = rows_raw[r].cells[c].style;
      int column = style->table_column_number;
      if (column < 1)
        column = 1;
      column--;
      row.cells[column].style = rows_raw[r].cells[c].style;
      // Append text to any already existing tetxt in the cell. Very unlikely to happen...
      ustring text = row.cells[column].text;
      if (!text.empty())
        text.append(" ");
      text.append(rows_raw[r].cells[c].text);
      row.cells[column].text = text;
    }
    rows_tidy.push_back(row);
  }

  // Pass 5: Produce the xslfo code for this table.


  // Ensure that any open styles are closed.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Open the xml table.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:table");

  // Go through each row.
  for (unsigned int r = 0; r < rows_tidy.size(); r++) {
    XslFoTableRow row = rows_tidy[r];
    if (row.cells.empty())
      continue;

    // If the first row is a header, handle it, else it is the table body.
    ustring header_or_body = "fo:table-";
    bool heading = false;
    if (r == 0) {
      if (row.cells[0].style->type == u2xtTableElementHeading) {
        heading = true;
      }
    }
    if (heading)
      header_or_body.append("header");
    else
      header_or_body.append("body");
    xmlTextWriterStartElement(writer, BAD_CAST header_or_body.c_str());
    xmlTextWriterStartElement(writer, BAD_CAST "fo:table-row");

    // Handle each cell.
    for (unsigned int c = 0; c < row.cells.size(); c++) {
      xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
      vector <ustring> lines;
      Usfm2XslFoStyle * style = row.cells[c].style;
      if (style == NULL)
        style = marker_get_pointer_to_style(default_style());
      open_fo_block(style, false);
      lines.push_back(row.cells[c].text);
      for (unsigned int i = 0; i < lines.size(); i++) {
        xmlTextWriterWriteString(writer, BAD_CAST lines[i].c_str());
      }
      close_possible_fo_block(style);
      xmlTextWriterEndElement(writer);
    }

    // Close row.  
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
  }

  // Close the xml table.
  xmlTextWriterEndElement(writer);
}

gchar * Usfm2XslFo::column_balancing_style() {
  return "_column_balancer_";
}

gchar * Usfm2XslFo::line_spacing_style() {
  return "_line_spacing_";
}

void Usfm2XslFo::output_column_balancer(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length) {
  // Close possible inline and block styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Remove the markup from the line.
  line.erase(0, marker_length);

  // Balance text columns using a block that spans the columns.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "span", "all");
  xmlTextWriterEndElement(writer);
}

void Usfm2XslFo::create_note_callers()
// This function goes through all available styles, and then creates
// the note caller objects from them.
{
  unsigned int poolpointer = 0;
  for (unsigned int i = 0; i < styles.size(); i++) {
    if ((styles[i].type == u2xtFootNoteStart) || (styles[i].type == u2xtEndNoteStart) || (styles[i].type == u2xtCrossreferenceStart)) {
      // Create the note caller.
      NoteCaller * notecaller = new NoteCaller (styles[i].note_numbering_type, styles[i].note_numbering_user_sequence);
      if (styles[i].note_numbering_restart_type == nnrtPage) {
        notecaller->renumber_per_page_temporal_caller_text = note_caller_numbering_per_page_pool ().substr(poolpointer, 1);
        poolpointer++;
        notecaller->renumber_per_page_temporal_caller_note = note_caller_numbering_per_page_pool ().substr(poolpointer, 1);
        poolpointer++;
      }
      // Store it in the main map.
      notecallers[&styles[i]] = notecaller;
      // Store it in the list of notecaller that need renumbering each page.
      if (styles[i].note_numbering_restart_type == nnrtPage) {
        notecallers_per_page.push_back(notecaller);
      }
    }
  }
}

void Usfm2XslFo::destroy_note_callers()
// This function goes through all available styles, and then creates
// the note caller objects from them.
// Or it destroys them.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    if ((styles[i].type == u2xtFootNoteStart) || (styles[i].type == u2xtEndNoteStart) || (styles[i].type == u2xtCrossreferenceStart)) {
      delete notecallers[&styles[i]];
    }
  }
}

void Usfm2XslFo::output_text_note(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, size_t marker_length, bool is_opener)
// Outputs a footnote or crossreference.
{
  // Erase the marker from the text.
  line.erase(0, marker_length);

  // Bail out if the opener and closer are not there.
  if (!is_opener)
    return;
  ustring endmarker = usfm_get_full_closing_marker(stylepointer->marker);
  size_t endmarkerpos = line.find(endmarker);
  if (endmarkerpos == string::npos)
    return;

  // Get raw note text and erase it from the input buffer.
  ustring rawnote(line.substr(0, endmarkerpos));
  line.erase(0, endmarkerpos + endmarker.length());

  // Bail out if we're not to print this portion.
  if (!inrange.in_range())
    return;

  // Bail out if this notes is not to be printed.
  if (!stylepointer->print)
    return;

  // Bail out if there is no note body.
  if (rawnote.empty())
    return;

  // Make sure that a fo:block is open at this point.
  ensure_fo_block_open(fo_block_style);

  // Open the footnote.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:footnote");

  // Extract the note caller in the text body and erase it from the raw note.
  ustring caller_in_text = trim(rawnote.substr(0, 1));
  if (caller_in_text == "+") {
    caller_in_text = notecallers[stylepointer]->get_caller();
    if ((stylepointer->type == u2xtFootNoteStart) || (stylepointer->type == u2xtCrossreferenceStart)) {
      if (stylepointer->note_numbering_restart_type == nnrtPage) {
        caller_in_text = notecallers[stylepointer]->renumber_per_page_temporal_caller_text;
      }
    }
  } else if (caller_in_text == "-") {
    caller_in_text.clear();
  }
  rawnote.erase(0, 1);
  rawnote = trim(rawnote);

  // Write the footnote caller in the text.
  Usfm2XslFoStyle * style = stylepointer;
  open_fo_inline(style, NULL);
  xmlTextWriterWriteFormatString(writer, caller_in_text.c_str());
  close_possible_fo_inline(style);

  // Open the footnote body.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:footnote-body");

  // Open the list block for outlining the caller nicely with the footnote body.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:list-block");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "provisional-label-separation", "0pt");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "provisional-distance-between-starts", "18pt");

  // Open the list item.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:list-item");

  // Open the list item label: for the note caller.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:list-item-label");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end-indent", "label-end()");

  // Get the default paragraph style.
  Usfm2XslFoStyle * default_paragraph_style = get_default_paragraph_style_for_note(stylepointer);

  // Get the caller in the note.
  ustring caller_in_note = caller_in_text;
  if ((stylepointer->type == u2xtFootNoteStart) || (stylepointer->type == u2xtCrossreferenceStart)) {
    if (stylepointer->note_numbering_restart_type == nnrtPage) {
      caller_in_note = notecallers[stylepointer]->renumber_per_page_temporal_caller_note;
    }
  }

  // Insert the caller in the footnote body.
  if (!caller_in_note.empty()) {
    Usfm2XslFoStyle * blockstyle = default_paragraph_style;
    // Temporally set margins to zero; set them back to normal later.
    double leftmargin = blockstyle->leftmargin;
    double rightmargin = blockstyle->rightmargin;
    double firstlineindent = blockstyle->firstlineindent;
    blockstyle->leftmargin = 0;
    blockstyle->rightmargin = 0;
    blockstyle->firstlineindent = 0;
    open_fo_block(blockstyle, false);
    blockstyle->leftmargin = leftmargin;
    blockstyle->rightmargin = rightmargin;
    blockstyle->firstlineindent = firstlineindent;
    Usfm2XslFoStyle * inlinestyle = stylepointer;
    open_fo_inline(inlinestyle, blockstyle);
    xmlTextWriterWriteFormatString(writer, caller_in_note.c_str());
    close_possible_fo_inline(inlinestyle);
    close_possible_fo_block(blockstyle);
  }

  // Close the list item label.
  xmlTextWriterEndElement(writer);

  // Open the list item body: for the note body.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:list-item-body");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start-indent", "body-start()");

  // Open the default paragraph style for this note.
  default_paragraph_style = get_default_paragraph_style_for_note(stylepointer);
  Usfm2XslFoStyle * blockstyle = default_paragraph_style;
  open_fo_block(blockstyle, false);

  // Optionally add the full text of any references in this note.
  optionally_add_full_references(rawnote, stylepointer);

  // Write the footnote.
  Usfm2XslFoStyle * inlinestyle= NULL;
  unsigned int iteration = 0;
  while (!rawnote.empty()) {
    ustring marker;
    size_t marker_position;
    size_t marker_length;
    bool is_opener;
    bool processed = false;
    bool marker_found = usfm_search_marker(rawnote, marker, marker_position, marker_length, is_opener);
    if (marker_found) {
      if (marker_position == 0) {
        Usfm2XslFoStyle * stylepointer = marker_get_pointer_to_style(marker);
        if (stylepointer) {
          if ((stylepointer->type == u2xtFootEndNoteStandardContent) || (stylepointer->type == u2xtCrossreferenceStandardContent)) {
            close_possible_fo_inline(inlinestyle);
          } else if ((stylepointer->type == u2xtFootEndNoteContent) || (stylepointer->type == u2xtFootEndNoteContentWithEndmarker) || (stylepointer->type == u2xtCrossreferenceContent) || (stylepointer->type
              == u2xtCrossreferenceContentWithEndmarker)) {
            close_possible_fo_inline(inlinestyle);
            if (is_opener) {
              inlinestyle = stylepointer;
              open_fo_inline(inlinestyle, blockstyle);
            }
          } else if (stylepointer->type == u2xtFootEndNoteParagraph) {
            close_possible_fo_inline(inlinestyle);
            close_possible_fo_block(blockstyle);
            blockstyle = stylepointer;
            open_fo_block(blockstyle, false);
          }
          rawnote.erase(0, marker_length);
          processed = true;
        }
      }
    }
    iteration++;
    if (!processed) {
      output_text_fallback(rawnote, blockstyle);
    }
  }

  // Close possible inline style and block style.
  close_possible_fo_inline(inlinestyle);
  close_possible_fo_block(blockstyle);

  // Close the list item body, the list item, the list block, the footnote body, and the footnote.
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
}

Usfm2XslFoStyle * Usfm2XslFo::get_default_paragraph_style_for_note(Usfm2XslFoStyle * notestyle)
// Given a "notestyle", that is, the style of a note opener such as e.g. "f"
// for a footnote, it returns the default paragraph style for this note.
{
  // Go through all the styles looking for the appropriate default style to return.
  for (unsigned int i = 0; i < styles.size(); i++) {
    if ((notestyle->type == u2xtFootNoteStart) || (notestyle->type == u2xtEndNoteStart)) {
      if (styles[i].type == u2xtFootEndNoteStandardContent) {
        return &styles[i];
      }
    }
    if (notestyle->type == u2xtCrossreferenceStart) {
      if (styles[i].type == u2xtCrossreferenceStandardContent) {
        return &styles[i];
      }
    }
  }
  // If no style was found, return the default paragraph style.
  return marker_get_pointer_to_style(default_style());
}

ustring Usfm2XslFo::xslfofile() {
  return gw_build_filename(directories_get_temp(), "usfm2xslfo.fo");
}

ustring Usfm2XslFo::xepfile() {
  return xslfofile() + ".xep";
}

void Usfm2XslFo::display(GtkWidget * window)
// Display the pdf file.
// Give information about problems.
{
}

void Usfm2XslFo::rewrite_note_callers()
/*
 Modifies the note numbering in the intermediate xep filename if the 
 numbering restarts each page.
 Whether it restarts each page is indicated by a special character in the text,
 retrieved from "note_caller_numbering_per_page_pool ()".
 */
{
  // If no callers bail out.
  if (notecallers_per_page.empty())
    return;

  // Progress information.
  progresswindow->set_text("Renumbering notes");
  progresswindow->set_fraction(0);

  // Read all text.
  ReadText rt(xepfile(), true, false);

  // Mark all page boundaries.
  vector<unsigned int> page_beginning;
  vector<unsigned int> page_ending;
  {
    unsigned int begin = 0;
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      if (rt.lines[i].find("<xep:page") != string::npos)
        begin = i;
      if (rt.lines[i].find("</xep:page") != string::npos) {
        page_beginning.push_back(begin);
        page_ending.push_back(i);
      }
    }
  }

  // Progress.
  progresswindow->set_iterate(0, 1, page_beginning.size());

  // Go through each page.
  for (unsigned int pg = 0; pg < page_beginning.size(); pg++) {

    // Progress.
    progresswindow->iterate();

    // Go through all available note callers.
    for (unsigned int nc = 0; nc < notecallers_per_page.size(); nc++) {

      // Store information about the positions of the footnote and xref markers.
      vector <unsigned int> lines_with_note_markers_text;
      vector <size_t> note_markers_positions_text;
      vector <unsigned int> lines_with_note_markers_note;
      vector <size_t> note_markers_positions_note;
      for (unsigned int i2 = page_beginning[pg]; i2 < page_ending[pg]; i2++) {
        size_t position;
        position = rt.lines[i2].find(notecallers_per_page[nc]->renumber_per_page_temporal_caller_text);
        if (position != string::npos) {
          lines_with_note_markers_text.push_back(i2);
          note_markers_positions_text.push_back(position);
        }
        position = rt.lines[i2].find(notecallers_per_page[nc]->renumber_per_page_temporal_caller_note);
        if (position != string::npos) {
          lines_with_note_markers_note.push_back(i2);
          note_markers_positions_note.push_back(position);
        }
      }

      // Handle all positions with the note markers.
      size_t note_caller_length = 1;
      if (lines_with_note_markers_text.size() > 0) {
        notecallers_per_page[nc]->reset();
        for (unsigned int i2 = 0; i2 < lines_with_note_markers_text.size(); i2++) {
          rt.lines[lines_with_note_markers_text[i2]].replace(note_markers_positions_text[i2], note_caller_length, notecallers_per_page[nc]->get_caller());
        }
      }
      if (lines_with_note_markers_note.size() > 0) {
        notecallers_per_page[nc]->reset();
        for (unsigned int i2 = 0; i2 < lines_with_note_markers_note.size(); i2++) {
          rt.lines[lines_with_note_markers_note[i2]].replace(note_markers_positions_note[i2], note_caller_length, notecallers_per_page[nc]->get_caller());
        }
      }
    }
  }

  // Write all text.
  write_lines(xepfile(), rt.lines);
}

void Usfm2XslFo::note_callers_new_book()
// Signals a new book to the appropriate notes objects.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    if ((styles[i].type == u2xtFootNoteStart) || (styles[i].type == u2xtCrossreferenceStart)) {
      if (styles[i].note_numbering_restart_type == nnrtBook) {
        notecallers[&styles[i]]->reset();
      }
    }
  }
  note_callers_new_chapter();
}

void Usfm2XslFo::note_callers_new_chapter()
// Signal a new chapter to the appropriate notes objects.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    if ((styles[i].type == u2xtFootNoteStart) || (styles[i].type == u2xtCrossreferenceStart)) {
      if (styles[i].note_numbering_restart_type == nnrtChapter) {
        notecallers[&styles[i]]->reset();
      }
    }
  }
}

void Usfm2XslFo::add_print_portion(unsigned int book_in, vector<unsigned int> chapters_from_in, const vector<ustring>& verses_from_in, vector<unsigned int> chapters_to_in, const vector <ustring>& verses_to_in)
// Adds a portion to be printed.
// If no portions was set, everything will be printed.
{
  inrange.add_portion(book_in, chapters_from_in, verses_from_in, chapters_to_in, verses_to_in);
}

void Usfm2XslFo::set_new_book(unsigned int book_in) {
  book = book_in;
  set_new_chapter(0);
  inrange.set_book(book);
}

void Usfm2XslFo::set_new_chapter(unsigned int chapter_in) {
  chapter = chapter_in;
  set_new_verse("0");
  inrange.set_chapter(chapter);
}

void Usfm2XslFo::set_new_verse(const ustring& verse_in) {
  verse = verse_in;
  inrange.set_verse(verse);
}

void Usfm2XslFo::output_id_page_break(Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style)
// Looks in the style whether the book id should cause a page break.
{
  // Bail out if no page break to be given.
  if (!stylepointer->book_starts_new_page)
    return;

  // The first id in a project should not cause a page break, else XEP gives an error.
  if (id_page_break_count > 0) {
    output_page_break(fo_block_style, fo_inline_style, stylepointer->book_starts_odd_page);
  }

  // Increase break count.
  id_page_break_count++;
}

void Usfm2XslFo::output_other_page_break(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length) {
  // Erase the marker.
  line.erase(0, marker_length);

  // Write the page break.
  output_page_break(fo_block_style, fo_inline_style, false);
}

void Usfm2XslFo::output_page_break(Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, bool oddpage) {
  // Bail out if we're not to write new pages.
  if (do_not_allow_new_page)
    return;

  // Close any open styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Write the page break.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  ustring breaktext;
  if (oddpage)
    breaktext = "odd-page";
  else
    breaktext = "page";
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "break-after", breaktext.c_str());
  xmlTextWriterEndElement(writer);
}

void Usfm2XslFo::output_picture(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length)
// Prints a picture.
{
  // Get the actual bit that describes the picture; erase it from the line.
  // The picture comes in the form of, e.g. "|biblesociety.gif|col||||"
  ustring rawpicture = get_erase_code_till_next_marker(line, 0, marker_length, true);

  // Remove the ending marker too, normally "fig*".
  ustring endingmarker = usfm_get_full_closing_marker(stylepointer->marker);
  if (line.find(endingmarker) == 0) {
    line.erase(0, endingmarker.length());
  }

  // Parse the figure data into its pieces.
  Parse parse(rawpicture, false, "|");
  ustring desc; // Picture description in English. Does not get printed.
  ustring file; // Illustration filename. Does not get printed.
  ustring size; // Picture size. Does not get printed. Valid options:
  // col:  insert picture inline within current text column.
  // span: insert picture spanning text columns, at top or bottom of page.
  // Note: Bibledit puts it at the top due to constraints in xslfo.
  ustring loc; // Picture location/range. Does not get printed.
  ustring copy; // Picture copyright info. Will be used to give the appropriate picture credits.
  ustring cap; // Picture caption. This will be printed with the illustration.
  ustring ref; // Picture reference (e.g. Luke 19.5). This will be printed with the illustration.
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    if (i == 0)
      desc = parse.words[i];
    if (i == 1)
      file = parse.words[i];
    if (i == 2)
      size = parse.words[i];
    if (i == 3)
      loc = parse.words[i];
    if (i == 4)
      copy = parse.words[i]; // Copyright is not yet handled.
    if (i == 5)
      cap = parse.words[i];
    if (i == 6)
      ref = parse.words[i];
  }

  // Make an existing path to the picture.
  // If a relative path is given, it looks in the pictures data directory, then
  // in the package data directory.
  if (file.substr(0, 1) != G_DIR_SEPARATOR_S) {
    ustring file2;
    file2 = gw_build_filename(directories_get_pictures(), file);
    if (g_file_test(file2.c_str(), G_FILE_TEST_IS_REGULAR))
      file = file2;
    else {
      file2 = gw_build_filename(directories_get_package_data(), file);
      if (g_file_test(file2.c_str(), G_FILE_TEST_IS_REGULAR))
        file = file2;
    }
  }

  // If the picture file does not exist, quit.
  if (!g_file_test(file.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gw_warning("Picture " + file + " does not exist");
    return;
  }

  // Close any currently opened styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Open new default block with modified values.
  Usfm2XslFoStyle * style = marker_get_pointer_to_style(default_style());
  ustring default_style_justification = style->justification;
  double default_style_space_after = style->spaceafter;
  style->justification = CENTER;
  style->spaceafter = 5;
  open_fo_block(style, false);
  style->justification = default_style_justification;
  style->spaceafter = default_style_space_after;

  // Insert graphic, either in the column or spanning the columns at the top
  // of the page.  
  // Write caption and reference.
  if (size == "span") {
    xmlTextWriterStartElement(writer, BAD_CAST "fo:float");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "float", "before");
  }
  xmlTextWriterStartElement(writer, BAD_CAST "fo:external-graphic");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "src", "url('%s')", file.c_str());
  xmlTextWriterWriteAttribute(writer, BAD_CAST "content-width", BAD_CAST "scale-to-fit");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "content-height", BAD_CAST "100%");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "width", BAD_CAST "100%");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "scaling", BAD_CAST "uniform");
  xmlTextWriterEndElement(writer);
  if ((!cap.empty()) || (!ref.empty())) {
    xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
    xmlTextWriterWriteFormatString(writer, cap.c_str());
    xmlTextWriterWriteFormatString(writer, " ");
    xmlTextWriterWriteFormatString(writer, ref.c_str());
    xmlTextWriterEndElement(writer);
  }
  if (size == "span") {
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
  }

  // Close the block.
  close_possible_fo_block(fo_block_style);
}

void Usfm2XslFo::output_elastic(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length) {
  // Erase the marker from the line.
  line.erase(0, marker_length);

  // Close any currently opened styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Write new default block with the elastic.
  ensure_fo_block_open(fo_block_style);
  xmlTextWriterWriteFormatString(writer, ELASTIC_XEP);
  close_possible_fo_block(fo_block_style);
}

ustring Usfm2XslFo::toc_identifier(unsigned int book)
// Returns the identifier used for the book.
{
  // Spaces are not allowed in an identifier, neither can it start with a number.
  ustring toc_id = "toc_" + lowerCase(books_id_to_english(book));
  replace_text(toc_id, " ", "_");
  return toc_id;
}

void Usfm2XslFo::toc_collect_text(ustring& line, size_t marker_length, bool longtext, unsigned int book)
// Collects the texts for the table of contents.
{
  ustring label = get_erase_code_till_next_marker(line, 0, marker_length, true);
  if (longtext) {
    toc_texts_long[book] = label;
  } else {
    toc_texts_short[book] = label;
  }
}

void Usfm2XslFo::toc_insert_anchor(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length)
// Inserts an anchor in the text so the table of contents 
// can reference to its page number.
{
  // Erase code and data from the line.
  get_erase_code_till_next_marker(line, 0, marker_length, false);

  // Close any open styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Write the id in a new block.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "id", toc_identifier (book).c_str());
  xmlTextWriterEndElement(writer);
  close_possible_fo_block(fo_block_style);
}

void Usfm2XslFo::toc_insert_body(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length)
// Generate the table of contents.
{
  // Erase code and data from the line.
  get_erase_code_till_next_marker(line, 0, marker_length, false);

  // Close any open styles.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  // Go through all books.
  for (unsigned int bk = 0; bk < books.size(); bk++) {

    // Get the book's label.
    ustring label = toc_texts_long[books[bk]];
    if (label.empty())
      label = toc_texts_short[books[bk]];

    // If there is no label for this book, skip it.
    if (label.empty())
      continue;

    // Open a block for this label.
    Usfm2XslFoStyle * style = marker_get_pointer_to_style(default_style());
    ustring default_style_justification = style->justification;
    style->justification = "last-justify";
    fo_block_style = style;
    open_fo_block(style, false);
    style->justification = default_style_justification;
    xmlTextWriterWriteFormatString(writer, label.c_str());
    xmlTextWriterStartElement(writer, BAD_CAST "fo:leader");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "leader-pattern", "dots");
    xmlTextWriterEndElement(writer);
    xmlTextWriterStartElement(writer, BAD_CAST "fo:page-number-citation");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "ref-id", toc_identifier (books[bk]).c_str());
    xmlTextWriterEndElement(writer);
    close_possible_fo_block(fo_block_style);
  }
}

void Usfm2XslFo::dump_endnotes(Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style)
// Dumps the buffered endnotes, if there are any.
{
  // If there are no endnotes, bail out.
  if (buffered_endnote_texts.empty())
    return;

  // Reset the note caller(s).
  for (unsigned int nt = 0; nt < buffered_endnote_styles.size(); nt++) {
    notecallers[buffered_endnote_styles[nt]]->reset();
  }

  // Go through each note writing it out.
  for (unsigned int nt = 0; nt < buffered_endnote_texts.size(); nt++) {

    // Get the raw note.
    ustring rawnote = buffered_endnote_texts[nt];

    // Get the style for this endnote.
    Usfm2XslFoStyle * endnote_style = buffered_endnote_styles[nt];

    // Ensure the block is closed, and open a new one.
    close_possible_fo_inline(fo_inline_style);
    close_possible_fo_block(fo_block_style);
    ensure_fo_block_open(fo_block_style);

    // Open the list block for outlining the caller nicely with the footnote body.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:list-block");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "provisional-label-separation", "0pt");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "provisional-distance-between-starts", "18pt");

    // Open the list item.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:list-item");

    // Open the list item label: for the note caller.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:list-item-label");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "end-indent", "label-end()");

    // Get the default paragraph style.
    Usfm2XslFoStyle * default_paragraph_style = get_default_paragraph_style_for_note(endnote_style);

    // Extract the note caller and erase it from the raw note.
    ustring caller = trim(rawnote.substr(0, 1));
    if (caller == "+") {
      caller = notecallers[endnote_style]->get_caller();
    } else if (caller == "-") {
      caller.clear();
    }
    rawnote.erase(0, 1);
    rawnote = trim(rawnote);

    // Insert the caller in the footnote body.
    if (!caller.empty()) {
      Usfm2XslFoStyle * blockstyle = default_paragraph_style;
      // Temporally set margins to zero; set them back to normal later.
      double leftmargin = blockstyle->leftmargin;
      double rightmargin = blockstyle->rightmargin;
      double firstlineindent = blockstyle->firstlineindent;
      blockstyle->leftmargin = 0;
      blockstyle->rightmargin = 0;
      blockstyle->firstlineindent = 0;
      open_fo_block(blockstyle, false);
      blockstyle->leftmargin = leftmargin;
      blockstyle->rightmargin = rightmargin;
      blockstyle->firstlineindent = firstlineindent;
      Usfm2XslFoStyle * inlinestyle = endnote_style;
      open_fo_inline(inlinestyle, blockstyle);
      xmlTextWriterWriteFormatString(writer, caller.c_str());
      close_possible_fo_inline(inlinestyle);
      close_possible_fo_block(blockstyle);
    }

    // Close the list item label.
    xmlTextWriterEndElement(writer);

    // Open the list item body: for the note body.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:list-item-body");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "start-indent", "body-start()");

    // Open the default paragraph style for this note.
    default_paragraph_style = get_default_paragraph_style_for_note(endnote_style);
    Usfm2XslFoStyle * blockstyle = default_paragraph_style;
    open_fo_block(blockstyle, false);

    // Optionally add the full text of any references in this note.
    optionally_add_full_references(rawnote, endnote_style);

    // Write the endnote.
    Usfm2XslFoStyle * inlinestyle= NULL;
    unsigned int iteration = 0;
    while (!rawnote.empty()) {
      ustring marker;
      size_t marker_position;
      size_t marker_length;
      bool is_opener;
      bool processed = false;
      bool marker_found = usfm_search_marker(rawnote, marker, marker_position, marker_length, is_opener);
      if (marker_found) {
        if (marker_position == 0) {
          Usfm2XslFoStyle * stylepointer = marker_get_pointer_to_style(marker);
          if (stylepointer) {
            if (stylepointer->type == u2xtFootEndNoteStandardContent) {
              close_possible_fo_inline(inlinestyle);
            } else if ((stylepointer->type == u2xtFootEndNoteContent) || (stylepointer->type == u2xtFootEndNoteContentWithEndmarker)) {
              close_possible_fo_inline(inlinestyle);
              if (is_opener) {
                inlinestyle = stylepointer;
                open_fo_inline(inlinestyle, blockstyle);
              }
            } else if (stylepointer->type == u2xtFootEndNoteParagraph) {
              close_possible_fo_inline(inlinestyle);
              close_possible_fo_block(blockstyle);
              blockstyle = stylepointer;
              open_fo_block(blockstyle, false);
            }
            rawnote.erase(0, marker_length);
            processed = true;
          }
        }
      }
      iteration++;
      if (!processed) {
        output_text_fallback(rawnote, blockstyle);
      }
    }

    // Close possible inline style and block style.
    close_possible_fo_inline(inlinestyle);
    close_possible_fo_block(blockstyle);

    // Close the list item body, the list item, the list block.
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);

    // Close the block after the note has been written.
    close_possible_fo_inline(fo_inline_style);
    close_possible_fo_block(fo_block_style);
  }

  // Clear the buffers.
  buffered_endnote_texts.clear();
  buffered_endnote_styles.clear();
}

void Usfm2XslFo::buffer_endnote(ustring& line, Usfm2XslFoStyle * stylepointer, size_t marker_length, bool is_opener)
// Buffers the text of an endnote.
{
  // Erase the marker from the text.
  line.erase(0, marker_length);

  // Bail out if the opener and closer are not there.
  if (!is_opener)
    return;
  ustring endmarker = usfm_get_full_closing_marker(stylepointer->marker);
  size_t endmarkerpos = line.find(endmarker);
  if (endmarkerpos == string::npos)
    return;

  // Get raw note text and erase it from the input buffer.
  ustring rawnote(line.substr(0, endmarkerpos));
  line.erase(0, endmarkerpos + endmarker.length());

  // Bail out if we're not to print this portion.
  if (!inrange.in_range())
    return;

  // Bail out if this note is not to be printed.
  if (!stylepointer->print)
    return;

  // Bail out if there is no note body.
  if (rawnote.empty())
    return;

  // Buffer the note body and style.
  buffered_endnote_texts.push_back(rawnote);
  buffered_endnote_styles.push_back(stylepointer);
}

void Usfm2XslFo::output_text_insertion_deletion(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener) {
  // Erase the marker from the text.
  line.erase(0, marker_length);

  // Bail out if the opener and closer are not there.
  if (!is_opener)
    return;
  ustring endmarker = usfm_get_full_closing_marker(stylepointer->marker);
  size_t endmarkerpos = line.find(endmarker);
  if (endmarkerpos == string::npos)
    return;

  // Get raw bit inserted or deletedd and erase it from the input buffer.
  ustring rawtext(line.substr(0, endmarkerpos));
  line.erase(0, endmarkerpos + endmarker.length());

  // Bail out if we're not to print this portion.
  if (!inrange.in_range())
    return;

  // Bail out if there is no raw text.
  if (rawtext.empty())
    return;

  // Close any inline text, keeping it for later.
  Usfm2XslFoStyle * previous_inline_style = fo_inline_style;
  close_possible_fo_inline(fo_inline_style);

  // Make sure that a fo:block is open at this point.
  ensure_fo_block_open(fo_block_style);

  // Write the text with markup.
  xmlTextWriterStartElement(writer, BAD_CAST "fo:inline");
  if (stylepointer->type == u2xtInsertion) {
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-weight", "bold");
  } else {
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "text-decoration", "line-through");
  }
  xmlTextWriterWriteFormatString(writer, rawtext.c_str());
  xmlTextWriterEndElement(writer);

  // If an inline style was open before, reopen it again.
  if (previous_inline_style) {
    fo_inline_style = previous_inline_style;
    open_fo_inline(fo_inline_style, fo_block_style);
  }
}

void Usfm2XslFo::no_bold()
// Remove the bold property from all the styles.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    styles[i].bold = OFF;
  }
}

void Usfm2XslFo::no_space_before_or_after()
// Set the space before and after the paragraph in all styles to zero.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    styles[i].spacebefore = 0;
    styles[i].spaceafter = 0;
  }
}

void Usfm2XslFo::output_spacing_paragraph(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener) {
  // Erase the marker from the line.
  line.erase(0, marker_length);

  // Close any open style.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  if (is_opener) {
    // Insert the spacing paragraph.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
    // XSLFormatter is better than FOP in that it does honour space conditionality,
    // which is initially set at "discard" for the beginning of a 
    // reference area, as here. So to get the distance between the 
    // lines right, this is inserted: space-before.conditionality="retain".
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "space-before", "2mm");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "space-before.conditionality", "retain");
  } else {
    // Close spacing paragraph.
    xmlTextWriterEndElement(writer);
  }
}

gchar * Usfm2XslFo::keep_together_on_page_style() {
  return "_keep_on_page_";
}

void Usfm2XslFo::output_keep_on_page(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener) {
  // Erase the marker from the line.
  line.erase(0, marker_length);

  // Close any open style.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  if (is_opener) {
    // Insert the block keepings things on one page.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
    // We use keep-together.within-page="always", rather than
    // keep-together="always", as the latter one causes things to be 
    // kept together, in in a line, which causes the line to overflow
    // the right margin.
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "keep-together.within-page", "always");
  } else {
    // Close spacing paragraph.
    xmlTextWriterEndElement(writer);
  }
}

gchar * Usfm2XslFo::font_family_size_line_height_style() {
  return "_font_family_size_line_height_";
}

void Usfm2XslFo::output_font_family_size_line_height(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener)
// This function adds the font family, the font size, and the line height to the xslfo file.
{
  // Erase the marker from the line.
  ustring code = get_erase_code_till_next_marker(line, 0, marker_length, true);

  // Close any open style.
  close_possible_fo_inline(fo_inline_style);
  close_possible_fo_block(fo_block_style);

  if (is_opener) {
    // Insert the block with the font and line height information.
    xmlTextWriterStartElement(writer, BAD_CAST "fo:block");
    Parse parse(code, false);
    if (parse.words.size() >= 3) {
      ustring fontfamily;
      for (unsigned int i = 0; i < parse.words.size() - 2; i++) {
        if (i)
          fontfamily.append(" ");
        fontfamily.append(parse.words[i]);
      }
      xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-family", fontfamily.c_str());
      ustring fontsize = parse.words[parse.words.size () - 2];
      xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-size", "%spt", fontsize.c_str());
      ustring lineheight = parse.words[parse.words.size () - 1];
      if (lineheight != "100")
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "line-height", "%s%%", lineheight.c_str());
    }
  } else {
    // Close spacing paragraph.
    xmlTextWriterEndElement(writer);
  }
}

void Usfm2XslFo::no_new_page() {
  do_not_allow_new_page = true;
}

void Usfm2XslFo::set_include_full_references_with_notes() {
  include_full_references_with_notes = true;
}

void Usfm2XslFo::optionally_add_full_references(ustring& line, Usfm2XslFoStyle * stylepointer) {
  // Bail out if the full references are not to be added.
  if (!include_full_references_with_notes)
    return;

  // Get the project and its language.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  ustring language = settings->projectconfig (project)->language_get();

  // Extract references.
  ReferencesScanner refscanner(language, book, line);

  // Default style for this note.
  Usfm2XslFoStyle * default_note_style = get_default_paragraph_style_for_note(stylepointer);
  ustring default_note_markup = usfm_get_full_opening_marker(default_note_style->marker);

  // Add the full text of the references if there is any.
  for (unsigned int i = 0; i < refscanner.references.size(); i++) {
    line.append(default_note_markup);
    line.append(" ");
    line.append(refscanner.references[i].human_readable(language));
    line.append(" ");
    ustring text = project_retrieve_verse(project, refscanner.references[i].book, refscanner.references[i].chapter, refscanner.references[i].verse);
    if (text.empty()) {
      line.append("<empty>");
    } else {
      CategorizeLine cl(text);
      line.append(cl.verse);
    }
    line.append("\n");
  }
  gw_message(line);
}
