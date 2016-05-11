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


#include "usfm2xetex.h"
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
#include <glib/gi18n.h>

Usfm2XeTex::Usfm2XeTex (XeTeX2Pdf * xetex2pdf_in, bool show_progress)
{
  // Initialize variables.
  xetex2pdf = xetex2pdf_in;
  font_size = 12;
  line_height = 100;
  right_to_left = false;
  two_columns = false;
  even_page_count = false;
  chapter_number_in_running_header_at_left_pages = true;
  chapter_number_in_running_header_at_right_pages = true;
  print_date = false;
  usfm_buffer_pointer = 0;
  progresswindow = NULL;
  progress_event_id = 0;
  if (show_progress) {
    progresswindow = new ProgressWindow(_("Processing"), true);
    progress_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(signal_progress_timeout), gpointer(this), NULL);
  }
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
  numeral_localization = NULL;

  // Add a couple of extra styles.
  add_style(default_style(), u2xtParagraphNormalParagraph);
  add_style(ELASTIC_MARKER, u2xtElastic);
  add_style(font_family_size_line_height_style(), u2xtFontFamilySizeLineHeight);
}


Usfm2XeTex::~Usfm2XeTex()
{
  gw_destroy_source(progress_event_id);
  if (progresswindow)
    delete progresswindow;
  if (numeral_localization)
    delete (numeral_localization);
}


void Usfm2XeTex::add_usfm_code(const vector < ustring > &lines)
// Add some usfm code to the object's usfm input buffer.
{
  for (unsigned int i = 0; i < lines.size(); i++) {
    add_usfm_code(lines[i]);
  }
}


void Usfm2XeTex::add_usfm_code(const ustring & line)
// Add some usfm code to the object's usfm input buffer.
{
  usfm_code.push_back(line);
}


void Usfm2XeTex::preprocess()
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
            Usfm2XslFoStyle *stylepointer = marker_get_pointer_to_style(marker);
            if (stylepointer) {
              switch (stylepointer->type) {
              case u2xtIdentifierBook:
                {
                  // Extract the book from the id string.
                  ustring id_line = get_erase_code_till_next_marker(usfm_line, 0, marker_length, true);
                  if (id_line.length() >= 3) {
                    id_line = id_line.substr(0, 3);
                    book = books_paratext_to_id(id_line);
                    // Initialize the highest chapter to 0.
                    highest_chapter_number[book] = 0;
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
                  highest_chapter_number[book] = chapter;
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
              case u2xtElastic:
              case u2xtDumpEndnotes:
              case u2xtLineSpacing:
              case u2xtKeepOnPage:
              case u2xtFontFamilySizeLineHeight:
                {
                  break;
                }
              case u2xtChapterLabel:
                {
                  // Get / store the chapter label.
                  ustring label = get_erase_code_till_next_marker(usfm_line,
                                                                  0, marker_length, true);
                  ChapterLabel chapterlabel(book, chapter, label);
                  chapter_labels.push_back(chapterlabel);
                  // Data was processed.
                  processed = true;
                  break;
                }
              case u2xtPublishedChapterMarker:
                {
                  // Get / store the chapter label.
                  ustring label = get_erase_code_till_next_marker(usfm_line,
                                                                  0, marker_length, true);
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


void Usfm2XeTex::convert_from_usfm_to_text()
{
  // Cancel?
  if (cancel)
    return;

  // Format the body of text.
  usfm_buffer_pointer = 0;
  book = 0;
  Usfm2XslFoStyle *fo_block_style = NULL;
  Usfm2XslFoStyle *fo_inline_style = NULL;
  ustring marker;
  size_t marker_position;
  size_t marker_length;
  bool is_opener;
  bool marker_found;
  while (unprocessed_usfm_code_available()) {
    if (cancel)
      break;
    while (gtk_events_pending())
      gtk_main_iteration();
    usfm_line = get_next_chapter();
    while (!usfm_line.empty()) {
      bool processed = false;
      marker_found = usfm_search_marker(usfm_line, marker, marker_position, marker_length, is_opener);
      if (marker_found) {
        if (marker_position == 0) {
          Usfm2XslFoStyle *stylepointer = marker_get_pointer_to_style(marker);
          if (stylepointer) {
            switch (stylepointer->type) {
            case u2xtIdentifierBook:
              {
                ustring id_line = get_erase_code_till_next_marker(usfm_line, 0, marker_length, true);
                if (id_line.length() >= 3) {
                  id_line = id_line.substr(0, 3);
                  set_new_book(books_paratext_to_id(id_line));
                }
                note_callers_new_book();
                // Before writing data of the new book, see whether to dump 
                // the endnotes here, and the end of the previous book.
                if (endnote_position == eptAfterBook)
                  dump_endnotes(fo_block_style, fo_inline_style);
                output_id_page_break(stylepointer, fo_block_style, fo_inline_style);
                book_spans_columns = (books_id_to_type(book) == btFrontBackMatter);
                break;
              }
            case u2xtIdentifierComment:
              {
                get_erase_code_till_next_marker(usfm_line, marker_position, marker_length, true);
                break;
              }
            case u2xtIdentifierRunningHeader:
              {
                output_running_header(usfm_line, stylepointer, marker_length, book);
                break;
              }
            case u2xtIdentifierLongTOC:
              {
                toc_insert_referent(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtIdentifierShortTOC:
              {
                toc_insert_referent(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtIdentifierBookAbbreviation:
              {
                get_erase_code_till_next_marker(usfm_line, marker_position, marker_length, true);
                break;
              }
            case u2xtParagraphMainTitle:
              {
                output_text_starting_new_paragraph(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length, true);
                break;
              }
            case u2xtParagraphSubTitle:
              {
                output_text_starting_new_paragraph(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length, true);
                break;
              }
            case u2xtParagraphSectionHeading:
              {
                output_text_starting_new_paragraph(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length, true);
                break;
              }
            case u2xtParagraphNormalParagraph:
              {
                output_text_starting_new_paragraph(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length, false);
                output_chapter_number_try_at_first_verse(usfm_line, fo_block_style);
                verses_in_paragraph_count = 0;
                break;
              }
            case u2xtInlineText:
              {
                output_text_character_style(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length, is_opener);
                break;
              }
            case u2xtChapterNumber:
              {
                set_new_chapter(convert_to_int(get_erase_code_till_next_marker(usfm_line, 0, marker_length, true)));
                note_callers_new_chapter();
                output_chapter_number_try_normal(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length, book);
                break;
              }
            case u2xtVerseNumber:
              {
                verses_in_paragraph_count++;
                usfm_line.erase(0, marker_length);
                size_t position = usfm_line.find(" ");
                position = CLAMP(position, 0, usfm_line.length());
                set_new_verse(usfm_line.substr(0, position));
                usfm_line.erase(0, position);
                bool verse_was_written = output_verse_number(stylepointer, fo_block_style, fo_inline_style, marker_length);
                // Erase any spaces following the verse if the verse was not written.
                if (!verse_was_written) {
                  while (!usfm_line.empty() && (usfm_line.substr(0, 1) == " ")) {
                    usfm_line.erase(0, 1);
                  }
                }
                break;
              }
            case u2xtFootNoteStart:
            case u2xtCrossreferenceStart:
              {
                output_text_note(usfm_line, stylepointer, fo_block_style, marker_length, is_opener);
                break;
              }
            case u2xtEndNoteStart:
              {
                buffer_endnote(usfm_line, stylepointer, marker_length, is_opener);
                break;
              }
            case u2xtFootEndNoteStandardContent:
            case u2xtFootEndNoteContent:
            case u2xtFootEndNoteContentWithEndmarker:
            case u2xtFootEndNoteParagraph:
            case u2xtCrossreferenceStandardContent:
            case u2xtCrossreferenceContent:
            case u2xtCrossreferenceContentWithEndmarker:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtPublication:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtTableOfContents:
              {
                toc_insert_body(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtPreface:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtIntroduction:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtGlossary:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtConcordance:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtIndex:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtMapIndex:
              {
                output_text_fallback(usfm_line);
                break;
              }
            case u2xtCover:
              {
                output_other_page_break(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtSpine:
              {
                output_other_page_break(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtPicture:
              {
                output_picture(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtPageBreak:
              {
                output_other_page_break(usfm_line, stylepointer, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtTableElementRow:
              {
                output_text_table(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtTableElementHeading:
              {
                output_text_table(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtTableElementCell:
              {
                output_text_table(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtElastic:
              {
                output_elastic(usfm_line, fo_block_style, fo_inline_style, marker_length);
                break;
              }
            case u2xtDumpEndnotes:
              {
                get_erase_code_till_next_marker(usfm_line, 0, marker_length, false);
                if (endnote_position == eptAtMarker)
                  dump_endnotes(fo_block_style, fo_inline_style);
                break;
              }
            case u2xtLineSpacing:
              {
                output_spacing_paragraph(usfm_line, fo_block_style, fo_inline_style, marker_length, is_opener);
                break;
              }
            case u2xtKeepOnPage:
              {
                output_keep_on_page(usfm_line, fo_block_style, fo_inline_style, marker_length, is_opener);
                break;
              }
            case u2xtFontFamilySizeLineHeight:
              {
                output_font_family_size_line_height(usfm_line, fo_block_style, fo_inline_style, marker_length, is_opener);
                break;
              }
            case u2xtChapterLabel:
              {
                get_erase_code_till_next_marker(usfm_line, marker_position, marker_length, true);
                break;
              }
            case u2xtPublishedChapterMarker:
              {
                get_erase_code_till_next_marker(usfm_line, marker_position, marker_length, true);
                break;
              }
            case u2xtIdentifierCommentWithEndmarker:
              {
                if (is_opener) {
                  get_erase_code_till_next_marker(usfm_line, marker_position, marker_length, true);
                } else {
                  usfm_line.erase(marker_position, marker_length);
                }
                break;
              }
            case u2xtGeneralWordlistEntry:
            case u2xtHebrewWordlistEntry:
            case u2xtGreekWordlistEntry:
            case u2xtSubjectIndexEntry:
              {
                usfm_line.erase(marker_position, marker_length);
                if (!is_opener) {
                  usfm_line.insert(0, stylepointer->wordlist_entry_addition);
                }
                break;
              }
            default:
              {
                output_text_fallback(usfm_line);
                break;
              }
            }
            // This text was processed.
            processed = true;
          }
        }
      }
      // Do default output if the line was not processed.
      if (!processed) {
        output_text_fallback(usfm_line);
      }
    }
  }
  // Dump any unprocessed endnotes here. This also dumps them after the last book.
  dump_endnotes(fo_block_style, fo_inline_style);
}


void Usfm2XeTex::process()
{
  // Preprocess the usfm code.
  preprocess();

  // Create the note caller objects.
  create_note_callers();

  // Convert from usfm to text.
  convert_from_usfm_to_text();

  // Hide our own progressbar.
  if (progresswindow)
    progresswindow->hide();
  if (cancel)
    return;

  // Calculate the length of elastics.
  //XepElastics xep_elastics(""/*xepfile()*/);

  // Delete the note caller objects.
  destroy_note_callers();
}


void Usfm2XeTex::set_fonts(const vector < ustring > &fonts_in, double font_size_in)
{
  fonts = fonts_in;
  font_size = font_size_in;
}


void Usfm2XeTex::set_line_height(unsigned int line_height_in)
{
  line_height = line_height_in;
}


void Usfm2XeTex::set_right_to_left()
{
  right_to_left = true;
}


void Usfm2XeTex::set_two_columns()
{
  two_columns = true;
}


void Usfm2XeTex::set_even_page_count()
// This forces an even page count in the page sequence.
{
  even_page_count = true;
}


void Usfm2XeTex::set_print_date()
{
  print_date = true;
}


void Usfm2XeTex::add_styles(const vector < Usfm2XslFoStyle > &styles_in)
{
  for (unsigned int i = 0; i < styles_in.size(); i++) {
    styles.push_back(styles_in[i]);
  }
}


void Usfm2XeTex::add_style(const ustring & marker_in, Usfm2XslFoStyleType type_in)
{
  Usfm2XslFoStyle style(marker_in);
  style.type = type_in;
  styles.push_back(style);
}


bool Usfm2XeTex::unprocessed_usfm_code_available()
// Returns true if there still is some unprocessed usfm code available.
{
  return ((usfm_buffer_pointer) < usfm_code.size());
}


ustring Usfm2XeTex::get_next_chapter()
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
    ustring line = usfm_code[usfm_buffer_pointer];
    ustring marker = usfm_extract_marker(line);
    if (!first_line) {
      if (marker == chapter_marker) {
        return chapter;
      }
    }
    if (!first_line)
      chapter.append(" ");
    chapter.append(usfm_code[usfm_buffer_pointer]);
    first_line = false;
    usfm_buffer_pointer++;
    replace_text(chapter, "\n", " ");
  }
  return chapter;
}


void Usfm2XeTex::output_text_starting_new_paragraph(ustring & line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length, bool keep_with_next_paragraph)
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

  // Close possible paragraph.
  xetex2pdf->close_paragraph();

  // Set the new fo:block style.
  fo_block_style = stylepointer;
  open_paragraph(fo_block_style, keep_with_next_paragraph);
}


Usfm2XslFoStyle *Usfm2XeTex::marker_get_pointer_to_style(const ustring & marker)
// Returns the type of the marker.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    if (marker == styles[i].marker)
      return &styles[i];
  }
  return NULL;
}


void Usfm2XeTex::output_text_fallback(ustring & line)
/*
 This function outputs text that was not handled by any other method.
 It outputs the text till it finds the next marker, if there's one.
 */
{
  // Bail out if there's nothing.
  if (line.empty())
    return;

  // Get the text till the next marker.
  ustring marker;
  size_t marker_position;
  size_t marker_length;
  bool is_opener;
  bool marker_found = usfm_search_marker(line, marker, marker_position,
                                         marker_length, is_opener);
  ustring text;
  if (marker_found) {
    text = line.substr(0, marker_position);
    line.erase(0, marker_position);
    // Handle special case that the marker is at the start, which is an error case anyway.
    if (marker_position == 0) {
      text = line.substr(0, 1);
      line.erase(0, 1);
    }
  } else {
    text = line;
    line.clear();
  }

  // Write that text. 
  if (inrange.in_range()) {
    xetex2pdf->add_text(text);
  }
}


void Usfm2XeTex::open_paragraph(Usfm2XslFoStyle * style, bool keep_with_next_paragraph)
{
  // Open the paragraph.
  xetex2pdf->open_paragraph();
  // Set the properties.
  set_paragraph(style, keep_with_next_paragraph);
}


void Usfm2XeTex::set_paragraph(Usfm2XslFoStyle * style, bool keep_with_next_paragraph)
{
  // Fontsize
  xetex2pdf->paragraph_set_font_size(style->fontsize);

  // Italic
  if (style->italic == ON) {
    xetex2pdf->paragraph_set_italic(true);
  }
  // Bold
  if (style->bold == ON) {
    xetex2pdf->paragraph_set_bold(true);
  }
  // Underline
  if (style->underline == ON) {
    xetex2pdf->paragraph_set_underline(true);
  }
  // Smallcaps
  if (style->smallcaps == ON) {
    xetex2pdf->paragraph_set_small_caps(true);
  }
  // Justification
  if (style->justification == LEFT) {
    xetex2pdf->paragraph_set_alignment(t2patLeft);
  } else if (style->justification == CENTER) {
    xetex2pdf->paragraph_set_alignment(t2patCenter);
  } else if (style->justification == RIGHT) {
    xetex2pdf->paragraph_set_alignment(t2patRight);
  } else if (style->justification == JUSTIFIED) {
    xetex2pdf->paragraph_set_alignment(t2patJustified);
  }
  // Space before
  xetex2pdf->paragraph_set_space_before((int)style->spacebefore);

  // Space after
  xetex2pdf->paragraph_set_space_after((int)style->spaceafter);

  // Start indent (or left margin).
  xetex2pdf->paragraph_set_left_margin((int)style->leftmargin);

  // End indent (or right margin).
  xetex2pdf->paragraph_set_right_margin((int)style->rightmargin);

  // Text indent (or first line indent).
  xetex2pdf->paragraph_set_first_line_indent((int)style->firstlineindent);

  // Spanning of columns.
  if ((style->spancolumns) || book_spans_columns)
    xetex2pdf->paragraph_set_column_count(1);

  // Keeping with next paragraph.
  if (keep_with_next_paragraph)
    xetex2pdf->paragraph_set_keep_with_next();
}


gchar *Usfm2XeTex::default_style()
// The name of the default style.
{
  return (gchar *) "usfm2xetex_default_style";
}


void Usfm2XeTex::open_inline(Usfm2XslFoStyle * style, Usfm2XslFoStyle * block)
{
  // Variable for general use.
  ustring s;

  // Fontsize
  xetex2pdf->inline_set_font_size_percentage(style->fontpercentage);

  // Italic
  if (style->italic == ON)
    xetex2pdf->inline_set_italic(t2pmtOn);
  else if (style->italic == OFF)
    xetex2pdf->inline_set_italic(t2pmtOff);
  else if (style->italic == INHERIT)
    xetex2pdf->inline_set_italic(t2pmtInherit);
  else if (style->italic == TOGGLE) {
    xetex2pdf->inline_set_italic(t2pmtToggle);
  }
  // Bold
  if (style->bold == ON)
    xetex2pdf->inline_set_bold(t2pmtOn);
  else if (style->bold == OFF)
    xetex2pdf->inline_set_bold(t2pmtOff);
  else if (style->bold == INHERIT)
    xetex2pdf->inline_set_bold(t2pmtInherit);
  else if (style->bold == TOGGLE) {
    xetex2pdf->inline_set_bold(t2pmtToggle);
  }
  // Underline
  s.clear();
  if (style->underline == ON)
    xetex2pdf->inline_set_underline(t2pmtOn);
  else if (style->underline == OFF)
    xetex2pdf->inline_set_underline(t2pmtOff);
  else if (style->underline == INHERIT)
    xetex2pdf->inline_set_underline(t2pmtInherit);
  else if (style->underline == TOGGLE) {
    xetex2pdf->inline_set_underline(t2pmtToggle);
  }
  // Smallcaps
  if (style->smallcaps == ON)
    xetex2pdf->inline_set_small_caps(t2pmtOn);
  else if (style->smallcaps == OFF)
    xetex2pdf->inline_set_small_caps(t2pmtOff);
  else if (style->smallcaps == INHERIT)
    xetex2pdf->inline_set_small_caps(t2pmtInherit);
  else if (style->smallcaps == TOGGLE) {
    xetex2pdf->inline_set_small_caps(t2pmtToggle);
  }
  // Superscript.
  if (style->superscript) {
    xetex2pdf->inline_set_superscript();
  }
  // Color
  if (style->color) {
    xetex2pdf->inline_set_colour(style->color);
  }
}


void Usfm2XeTex::output_chapter_number_try_normal(ustring & line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length, unsigned int book)
// This function tries to output the chapter number at the normal position.
// Else it stores the data needed for outputting it later on at the first verse.
{
  // Bail out of this portion should not be printed.
  if (!inrange.in_range())
    return;

  // Close possible inline and block styles.  
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();
  fo_block_style = NULL;

  // Print no chapter number for books with only one chapter.
  if (highest_chapter_number[book] == 1)
    return;

  // Whether to print the chapter number here, or at first verse.
  chapter_number_to_output_at_first_verse.clear();
  bool at_first_verse = stylepointer->print_chapter_number_at_first_verse;
  if (at_first_verse) {
    chapter_number_to_output_at_first_verse = convert_to_string(chapter);
  }
  // Bail out if printing the chapter number later on, not here.
  if (at_first_verse)
    return;

  // Write the chapter number.
  // Normally this is the number as it is, 
  // but it can be modified if a published chapter marker is given.
  // Insert a possible chapter label too.
  // Localization is done too.
  fo_block_style = stylepointer;
  open_paragraph(fo_block_style, true);
  ustring chapterlabel;
  for (unsigned int i = 0; i < chapter_labels.size(); i++) {
    if (chapter_labels[i].book == book) {
      if ((chapter_labels[i].chapter == 0) || (chapter_labels[i].chapter == chapter)) {
        chapterlabel = chapter_labels[i].label;
      }
    }
  }
  if (!chapterlabel.empty()) {
    xetex2pdf->add_text(chapterlabel);
    xetex2pdf->add_text(" ");
  }
  ustring chapternumber = convert_to_string(chapter);
  for (unsigned int i = 0; i < published_chapter_markers.size(); i++) {
    if (book == published_chapter_markers[i].book) {
      if (chapter == published_chapter_markers[i].chapter) {
        chapternumber = published_chapter_markers[i].label;
      }
    }
  }
  ustring localization (chapternumber);
  if (numeral_localization) {
    localization = numeral_localization->latin2localization (chapternumber);
  }
  xetex2pdf->add_text(localization);
  xetex2pdf->close_paragraph();
  fo_block_style = NULL;
}


void Usfm2XeTex::output_chapter_number_try_at_first_verse(ustring line, Usfm2XslFoStyle * &fo_block_style)
{
  // Bail out if there is no chapter to write.
  if (chapter_number_to_output_at_first_verse.empty())
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
    Usfm2XslFoStyle *stylepointer = marker_get_pointer_to_style(marker);
    if (stylepointer) {
      if (stylepointer->type == u2xtVerseNumber) {
        // We're now at the point that the chapter number can be written.

        // Set the current paragraph to have no indent.
        xetex2pdf->paragraph_set_first_line_indent(0);

        // Search the chapter style.
        Usfm2XslFoStyle *chapter_style = NULL;
        for (unsigned int i = 0; i < styles.size(); i++) {
          if (styles[i].type == u2xtChapterNumber)
            chapter_style = &styles[i];
        }

        // Assemble chapter number.
        // Normally this is the number as it is, 
        // but it can be modified if a published chapter marker is given.
        ustring chapternumber = chapter_number_to_output_at_first_verse;
        for (unsigned int i = 0; i < published_chapter_markers.size(); i++) {
          if (book == published_chapter_markers[i].book) {
            if (convert_to_int(chapter_number_to_output_at_first_verse) == published_chapter_markers[i].chapter) {
              chapternumber = published_chapter_markers[i].label;
            }
          }
        }

        // Write the localized chapter number.
        if (chapter_style) {
          xetex2pdf->open_intrusion();
          set_paragraph(chapter_style, true);
          ustring localization (chapternumber);
          if (numeral_localization) {
            localization = numeral_localization->latin2localization (chapternumber);
          }
          xetex2pdf->add_text(localization);
          xetex2pdf->close_intrusion();
        }
        // The next verse number won't be written.
        printversenumber = false;

        // Done: clear the chapter number.
        chapter_number_to_output_at_first_verse.clear();
      }
    }
  }
}


void Usfm2XeTex::close_possible_inline(Usfm2XslFoStyle * &style)
// Close possible inline style.
{
  if (style) {
    xetex2pdf->inline_clear_font_size_percentage();
    xetex2pdf->inline_clear_italic();
    xetex2pdf->inline_clear_bold();
    xetex2pdf->inline_clear_underline();
    xetex2pdf->inline_clear_small_caps();
    xetex2pdf->inline_clear_superscript();
    xetex2pdf->inline_clear_colour();
    style = NULL;
  }
}


void Usfm2XeTex::output_running_header(ustring & line, Usfm2XslFoStyle * stylepointer, size_t marker_length, unsigned int book)
// This function outputs a running header.
{
  // Extract running header.
  ustring runningheader = get_erase_code_till_next_marker(line, 0,
                                                          marker_length, true);

  // Store left and/or right header.
  if (stylepointer->print_in_left_running_header) {
    xetex2pdf->set_running_header_left_page(runningheader);
  }
  if (stylepointer->print_in_right_running_header) {
    xetex2pdf->set_running_header_right_page(runningheader);
  }
}


bool Usfm2XeTex::signal_progress_timeout(gpointer user_data)
{
  ((Usfm2XeTex *) user_data)->signal_progress();
  return true;
}


void Usfm2XeTex::signal_progress()
{
  if (progresswindow) {
    progresswindow->pulse();
    if (progresswindow->cancel)
      cancel = true;
  }
}


bool Usfm2XeTex::output_verse_number(Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// Writes the verse number.
// Returns true if the verse was written.
{
  // Bail out if this portion is not printed.
  if (!inrange.in_range())
    return false;

  // Close possible inline style
  close_possible_inline(fo_inline_style);

  // If the verse is to restart the paragraph, handle that here.
  if (verses_in_paragraph_count > 1) {
    if (stylepointer->restart_paragraph) {
      Usfm2XslFoStyle *style = fo_block_style;
      xetex2pdf->close_paragraph();
      style = fo_block_style;
      open_paragraph(style, false);
    }
  }
  // Write any number different from 1.
  if (verse != "1")
    printversenumber = true;
  bool verse_number_written = printversenumber;

  // Write the localized verse number.
  if (printversenumber && stylepointer->print) {
    fo_inline_style = stylepointer;
    open_inline(fo_inline_style, fo_block_style);
    ustring localization (verse);
    if (numeral_localization) {
      localization = numeral_localization->latin2localization (verse);
    }
    xetex2pdf->add_text(localization);
    close_possible_inline(fo_inline_style);
  }
  // Write next verse number again.
  printversenumber = true;

  // Return whether it was written.
  return verse_number_written;
}


void Usfm2XeTex::output_text_character_style(ustring & line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length, bool is_opener)
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

  // Close possible inline style(s).
  close_possible_inline(fo_inline_style);

  // Set the new fo:inline style.
  if (is_opener) {
    fo_inline_style = stylepointer;
    open_inline(fo_inline_style, fo_block_style);
  }
}


void Usfm2XeTex::output_text_table(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// This function collects all table related data that is available in a row,
// and then processes that to output the complete table.
{

  // Pass 1: Collecting the raw usfm code bits.

  // Containers for all table related styles, and the texts belonging to them.
  vector < Usfm2XslFoStyle * >table_styles;
  vector < ustring > table_texts;

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
      bool marker_found = usfm_search_marker(line, marker,
                                             marker_position, marker_length, is_opener);
      if (marker_found) {
        if (marker_position == 0) {
          Usfm2XslFoStyle *stylepointer = marker_get_pointer_to_style(marker);
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
    }
    while (in_table && (!line.empty()));
  }

  // Bail out if there is nothing to do.
  if (table_styles.empty())
    return;

  // Bail out if we're not to print this portion.
  if (!inrange.in_range())
    return;

  // Pass 2: Assemble the table rows and cells.

  // Raw table data storage.
  vector < XslFoTableRow > rows_raw;

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
      Usfm2XslFoStyle *style = rows_raw[r].cells[c].style;
      if (style->table_column_number > (int)highest_column)
        highest_column = style->table_column_number;
    }
  }

  // Pass 4: Produce a tidied table.

  // This tidied table has the same amount of columns in each row.
  // It may have empty cells in cases there was no data for it.
  vector < XslFoTableRow > rows_tidy;
  for (unsigned int r = 0; r < rows_raw.size(); r++) {
    XslFoTableRow row(0);
    for (unsigned int c = 0; c < highest_column; c++) {
      XslFoTableCell cell(NULL, "");
      row.cells.push_back(cell);
    }
    for (unsigned int c = 0; c < rows_raw[r].cells.size(); c++) {
      Usfm2XslFoStyle *style = rows_raw[r].cells[c].style;
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
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  // Open the xml table.
  //xmlTextWriterStartElement(writer, BAD_CAST "fo:table");

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
    //xmlTextWriterStartElement(writer, BAD_CAST header_or_body.c_str());

    // Handle each cell.
    for (unsigned int c = 0; c < row.cells.size(); c++) {
      //xmlTextWriterStartElement(writer, BAD_CAST "fo:table-cell");
      vector < ustring > lines;
      Usfm2XslFoStyle *style = row.cells[c].style;
      if (style == NULL)
        style = marker_get_pointer_to_style(default_style());
      open_paragraph(style, false);
      lines.push_back(row.cells[c].text);
      for (unsigned int i = 0; i < lines.size(); i++) {
        //xmlTextWriterWriteString(writer, BAD_CAST lines[i].c_str());
      }
      xetex2pdf->close_paragraph();
    }

    // Close row.  
  }

  // Close the xml table.
}


void Usfm2XeTex::create_note_callers()
// This function goes through all available styles, and then creates
// the note caller objects from them.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    if ((styles[i].type == u2xtFootNoteStart) || (styles[i].type == u2xtEndNoteStart) || (styles[i].type == u2xtCrossreferenceStart)) {
      // Create the note caller.
      NoteCaller *notecaller = new NoteCaller(styles[i].note_numbering_type, styles[i].note_numbering_user_sequence);
      // Store it in the main map.
      notecallers[&styles[i]] = notecaller;
    }
  }
}


void Usfm2XeTex::destroy_note_callers()
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


void Usfm2XeTex::output_text_note(ustring & line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, size_t marker_length, bool is_opener)
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

  // Extract the note caller in the text body and erase it from the raw note.
  ustring caller_in_text = trim(rawnote.substr(0, 1));
  if (caller_in_text == "+") {
    caller_in_text = notecallers[stylepointer]->get_caller();
  } else if (caller_in_text == "-") {
    caller_in_text.clear();
  }
  rawnote.erase(0, 1);
  rawnote = trim(rawnote);

  // Write the footnote caller in the text. 
  // The stylesheet is not consulted, it is just put in superscript, as is common for notes.
  xetex2pdf->inline_set_superscript();
  xetex2pdf->add_text(caller_in_text);
  xetex2pdf->inline_clear_superscript();

  // Open the footnote.
  xetex2pdf->open_note();

  // Set the paragraph to the default paragraph style for this note.
  Usfm2XslFoStyle *default_paragraph_style = get_default_paragraph_style_for_note(stylepointer);
  set_paragraph(default_paragraph_style, false);
  Usfm2XslFoStyle *blockstyle = default_paragraph_style;

  // Get the caller in the note.
  ustring caller_in_note = caller_in_text;

  // Insert the caller in the footnote body, in the note's style.
  if (!caller_in_note.empty()) {
    Usfm2XslFoStyle *inlinestyle = stylepointer;
    open_inline(inlinestyle, default_paragraph_style);
    xetex2pdf->add_text(caller_in_note);
    close_possible_inline(inlinestyle);
    xetex2pdf->add_text(" ");
  }
  // Optionally add the full text of any references in this note.
  optionally_add_full_references(rawnote, stylepointer);

  // Write the footnote.
  Usfm2XslFoStyle *inlinestyle = NULL;
  unsigned int iteration = 0;
  while (!rawnote.empty()) {
    ustring marker;
    size_t marker_position;
    size_t marker_length;
    bool is_opener;
    bool processed = false;
    bool marker_found = usfm_search_marker(rawnote, marker,
                                           marker_position, marker_length, is_opener);
    if (marker_found) {
      if (marker_position == 0) {
        Usfm2XslFoStyle *stylepointer = marker_get_pointer_to_style(marker);
        if (stylepointer) {
          if ((stylepointer->type == u2xtFootEndNoteStandardContent) || (stylepointer->type == u2xtCrossreferenceStandardContent)) {
            close_possible_inline(inlinestyle);
          } else if ((stylepointer->type == u2xtFootEndNoteContent) || (stylepointer->type == u2xtFootEndNoteContentWithEndmarker) || (stylepointer->type == u2xtCrossreferenceContent) || (stylepointer->type == u2xtCrossreferenceContentWithEndmarker)) {
            close_possible_inline(inlinestyle);
            if (is_opener) {
              inlinestyle = stylepointer;
              open_inline(inlinestyle, blockstyle);
            }
          } else if (stylepointer->type == u2xtFootEndNoteParagraph) {
            close_possible_inline(inlinestyle);
            xetex2pdf->close_paragraph();
            blockstyle = stylepointer;
            open_paragraph(blockstyle, false);
          }
          rawnote.erase(0, marker_length);
          processed = true;
        }
      }
    }
    iteration++;
    if (!processed) {
      output_text_fallback(rawnote);
    }
  }

  // Close note.
  xetex2pdf->close_note();
}


Usfm2XslFoStyle *Usfm2XeTex::get_default_paragraph_style_for_note(Usfm2XslFoStyle * notestyle)
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


void Usfm2XeTex::note_callers_new_book()
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


void Usfm2XeTex::note_callers_new_chapter()
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


void Usfm2XeTex::add_print_portion(unsigned int book_in, vector < unsigned int >chapters_from_in, const vector < ustring > &verses_from_in, vector < unsigned int >chapters_to_in, const vector < ustring > &verses_to_in)
// Adds a portion to be printed.
// If no portions was set, everything will be printed.
{
  inrange.add_portion(book_in, chapters_from_in, verses_from_in, chapters_to_in, verses_to_in);
}


void Usfm2XeTex::set_new_book(unsigned int book_in)
{
  book = book_in;
  set_new_chapter(0);
  inrange.set_book(book);
  xetex2pdf->suppress_header_this_page();
}


void Usfm2XeTex::set_new_chapter(unsigned int chapter_in)
{
  chapter = chapter_in;
  unsigned int left = 0;
  unsigned int right = 0;
  if (chapter_number_in_running_header_at_left_pages) {
    left = chapter_in;
  }
  if (chapter_number_in_running_header_at_right_pages) {
    right = chapter_in;
  }
  inrange.set_chapter(chapter);
  if (inrange.in_range())
    xetex2pdf->set_running_chapter_number(left, right);
}


void Usfm2XeTex::set_new_verse(const ustring & verse_in)
{
  verse = verse_in;
  inrange.set_verse(verse);
}


void Usfm2XeTex::output_id_page_break(Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style)
// Looks in the style whether the book id should cause a page break.
{
  // Bail out if no page break to be given.
  if (!stylepointer->book_starts_new_page)
    return;

  // The first id in a project should not cause a page break.
  if (id_page_break_count > 0) {
    output_page_break(fo_block_style, fo_inline_style, stylepointer->book_starts_odd_page);
  }
  // Increase break count.
  id_page_break_count++;
}


void Usfm2XeTex::output_other_page_break(ustring & line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// Outputs a page break.
{
  // Erase the marker.
  line.erase(0, marker_length);

  // Write the page break.
  output_page_break(fo_block_style, fo_inline_style, false);
}


void Usfm2XeTex::output_page_break(Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, bool oddpage)
// Outputs a page break.
{
  // Bail out if we're not to write new pages.
  if (do_not_allow_new_page)
    return;

  // Close any open styles.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  // Write the page break.
  xetex2pdf->new_page(oddpage);
}


void Usfm2XeTex::output_picture(ustring & line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// Prints a picture.
{
  // Get the actual bit that describes the picture; erase it from the line.
  // The picture comes in the form of, e.g. "|biblesociety.gif|col||||"
  ustring desc;                 // Picture description in English. Does not get printed.
  ustring file;                 // Illustration filename. Does not get printed.
  ustring size;                 // Picture size. Does not get printed. Valid options:
  // col:  insert picture inline within current text column.
  // span: insert picture spanning text columns, at top or bottom of page.
  // Note: Bibledit puts it at the top due to constraints in xslfo.
  ustring loc;                  // Picture location/range. Does not get printed.
  ustring copy;                 // Picture copyright info. Will be used to give the appropriate picture credits.
  ustring cap;                  // Picture caption. This will be printed with the illustration.
  ustring ref;                  // Picture reference (e.g. Luke 19.5). This will be printed with the illustration.
  usfm_dissect_figure (line, stylepointer->marker, marker_length, desc, file, size, loc, copy, cap, ref);

  // Make an existing path to the picture.
  // If a relative path is given, it looks in the pictures data directory, then
  // in the package data directory.
  if (file.substr(0, 1) != G_DIR_SEPARATOR_S) {
    ustring file2;
    file2 = gw_build_filename(Directories->get_pictures(), file);
    if (g_file_test(file2.c_str(), G_FILE_TEST_IS_REGULAR))
      file = file2;
    else {
      file2 = gw_build_filename(Directories->get_package_data(), file);
      if (g_file_test(file2.c_str(), G_FILE_TEST_IS_REGULAR))
        file = file2;
    }
  }
  // If the picture file does not exist, quit.
  if (!g_file_test(file.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gw_warning(_("Picture does not exist: ") + file);
    return;
  }
  // Close any currently opened styles.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  // Open new default block with modified values.
  Usfm2XslFoStyle *style = marker_get_pointer_to_style(default_style());
  ustring default_style_justification = style->justification;
  double default_style_space_after = style->spaceafter;
  style->justification = CENTER;
  style->spaceafter = 5;
  open_paragraph(style, false);
  style->justification = default_style_justification;
  style->spaceafter = default_style_space_after;

  // Insert graphic, either in the column or spanning the columns at the top
  // of the page.  
  // Write caption and reference.
  if (size == "span") {
  }
  xetex2pdf->add_text(file);
  if ((!cap.empty()) || (!ref.empty())) {
    xetex2pdf->add_text(" ");
    xetex2pdf->add_text(cap);
    xetex2pdf->add_text(" ");
    xetex2pdf->add_text(ref);
  }
  if (size == "span") {
  }
  // Close the block.
  xetex2pdf->close_paragraph();
}


void Usfm2XeTex::output_elastic(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// Output the elastic command.
{
  // Erase the marker from the line.
  line.erase(0, marker_length);

  // Close any currently opened styles.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  // Write new default block with the elastic.
  xetex2pdf->open_paragraph();
  //xmlTextWriterWriteFormatString(writer, ELASTIC_XEP);
  xetex2pdf->close_paragraph();
}


void Usfm2XeTex::toc_collect_text(ustring & line, size_t marker_length, bool longtext, unsigned int book)
// Collects the texts for the table of contents.
{
  ustring label = get_erase_code_till_next_marker(line, 0, marker_length,
                                                  true);
  if (longtext) {
    toc_texts_long[book] = label;
  } else {
    toc_texts_short[book] = label;
  }
}


void Usfm2XeTex::toc_insert_referent(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// Inserts a referent in the text so the table of contents can refer to it to retrieve the page number.
{
  // Erase code and data from the line.
  get_erase_code_till_next_marker(line, 0, marker_length, false);

  // Write the referent.
  xetex2pdf->set_referent(books_id_to_english(book));
}


void Usfm2XeTex::toc_insert_body(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length)
// Generate the table of contents.
{
  // Erase code and data from the line.
  get_erase_code_till_next_marker(line, 0, marker_length, false);

  // Close any open styles.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

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
    Usfm2XslFoStyle *style = marker_get_pointer_to_style(default_style());
    fo_block_style = style;
    open_paragraph(style, false);
    xetex2pdf->set_reference(books_id_to_english(books[bk]));
    xetex2pdf->add_text(label);
    xetex2pdf->close_paragraph();
  }
}


void Usfm2XeTex::dump_endnotes(Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style)
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
    Usfm2XslFoStyle *endnote_style = buffered_endnote_styles[nt];

    // Ensure the block is closed, and open a new one.
    close_possible_inline(fo_inline_style);
    xetex2pdf->close_paragraph();
    xetex2pdf->open_paragraph();

    // Open the list block for outlining the caller nicely with the footnote body.

    // Open the list item.

    // Open the list item label: for the note caller.

    // Get the default paragraph style.
    Usfm2XslFoStyle *default_paragraph_style = get_default_paragraph_style_for_note(endnote_style);

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
      Usfm2XslFoStyle *blockstyle = default_paragraph_style;
      // Temporally set margins to zero; set them back to normal later.
      double leftmargin = blockstyle->leftmargin;
      double rightmargin = blockstyle->rightmargin;
      double firstlineindent = blockstyle->firstlineindent;
      blockstyle->leftmargin = 0;
      blockstyle->rightmargin = 0;
      blockstyle->firstlineindent = 0;
      open_paragraph(blockstyle, false);
      blockstyle->leftmargin = leftmargin;
      blockstyle->rightmargin = rightmargin;
      blockstyle->firstlineindent = firstlineindent;
      Usfm2XslFoStyle *inlinestyle = endnote_style;
      open_inline(inlinestyle, blockstyle);
      //xmlTextWriterWriteFormatString(writer, caller.c_str());
      close_possible_inline(inlinestyle);
      xetex2pdf->close_paragraph();
    }
    // Close the list item label.

    // Open the list item body: for the note body.

    // Open the default paragraph style for this note.
    default_paragraph_style = get_default_paragraph_style_for_note(endnote_style);
    Usfm2XslFoStyle *blockstyle = default_paragraph_style;
    open_paragraph(blockstyle, false);

    // Optionally add the full text of any references in this note.
    optionally_add_full_references(rawnote, endnote_style);

    // Write the endnote.
    Usfm2XslFoStyle *inlinestyle = NULL;
    unsigned int iteration = 0;
    while (!rawnote.empty()) {
      ustring marker;
      size_t marker_position;
      size_t marker_length;
      bool is_opener;
      bool processed = false;
      bool marker_found = usfm_search_marker(rawnote, marker,
                                             marker_position, marker_length, is_opener);
      if (marker_found) {
        if (marker_position == 0) {
          Usfm2XslFoStyle *stylepointer = marker_get_pointer_to_style(marker);
          if (stylepointer) {
            if (stylepointer->type == u2xtFootEndNoteStandardContent) {
              close_possible_inline(inlinestyle);
            } else if ((stylepointer->type == u2xtFootEndNoteContent) || (stylepointer->type == u2xtFootEndNoteContentWithEndmarker)) {
              close_possible_inline(inlinestyle);
              if (is_opener) {
                inlinestyle = stylepointer;
                open_inline(inlinestyle, blockstyle);
              }
            } else if (stylepointer->type == u2xtFootEndNoteParagraph) {
              close_possible_inline(inlinestyle);
              xetex2pdf->close_paragraph();
              blockstyle = stylepointer;
              open_paragraph(blockstyle, false);
            }
            rawnote.erase(0, marker_length);
            processed = true;
          }
        }
      }
      iteration++;
      if (!processed) {
        output_text_fallback(rawnote);
      }
    }

    // Close possible inline style and block style.
    close_possible_inline(inlinestyle);
    xetex2pdf->close_paragraph();

    // Close the list item body, the list item, the list block.

    // Close the block after the note has been written.
    close_possible_inline(fo_inline_style);
    xetex2pdf->close_paragraph();
  }

  // Clear the buffers.
  buffered_endnote_texts.clear();
  buffered_endnote_styles.clear();
}


void Usfm2XeTex::buffer_endnote(ustring & line, Usfm2XslFoStyle * stylepointer, size_t marker_length, bool is_opener)
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


void Usfm2XeTex::no_bold()
// Remove the bold property from all the styles.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    styles[i].bold = OFF;
  }
}


void Usfm2XeTex::no_space_before_or_after()
// Set the space before and after the paragraph in all styles to zero.
{
  for (unsigned int i = 0; i < styles.size(); i++) {
    styles[i].spacebefore = 0;
    styles[i].spaceafter = 0;
  }
}


void Usfm2XeTex::output_spacing_paragraph(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length, bool is_opener)
{
  // Erase the marker from the line.
  line.erase(0, marker_length);

  // Close any open style.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  if (is_opener) {
    // Insert the spacing paragraph.
  } else {
    // Close spacing paragraph.
  }
}


void Usfm2XeTex::output_keep_on_page(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length, bool is_opener)
{
  // Erase the marker from the line.
  line.erase(0, marker_length);

  // Close any open style.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  if (is_opener) {
    // Insert the block keepings things on one page.
    // We use keep-together.within-page="always", rather than
    // keep-together="always", as the latter one causes things to be 
    // kept together, in in a line, which causes the line to overflow
    // the right margin.
  } else {
    // Close spacing paragraph.
  }
}


gchar *Usfm2XeTex::font_family_size_line_height_style()
{
  return (gchar *) "_font_family_size_line_height_";
}


void Usfm2XeTex::output_font_family_size_line_height(ustring & line, Usfm2XslFoStyle * &fo_block_style, Usfm2XslFoStyle * &fo_inline_style, size_t marker_length, bool is_opener)
// This function adds the font family, the font size, and the line height to the xslfo file.
{
  // Erase the marker from the line.
  ustring code = get_erase_code_till_next_marker(line, 0, marker_length, true);

  // Close any open style.
  close_possible_inline(fo_inline_style);
  xetex2pdf->close_paragraph();

  if (is_opener) {
    // Insert the block with the font and line height information.
    Parse parse(code, false);
    if (parse.words.size() >= 3) {
      ustring fontfamily;
      for (unsigned int i = 0; i < parse.words.size() - 2; i++) {
        if (i)
          fontfamily.append(" ");
        fontfamily.append(parse.words[i]);
      }
      //xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-family", fontfamily.c_str());
      ustring fontsize = parse.words[parse.words.size() - 2];
      //xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "font-size", "%spt", fontsize.c_str());
      ustring lineheight = parse.words[parse.words.size() - 1];
      if (lineheight != "100") ;
      //xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "line-height", "%s%%", lineheight.c_str());
    }
  } else {
    // Close spacing paragraph.
  }
}


void Usfm2XeTex::no_new_page()
{
  do_not_allow_new_page = true;
}


void Usfm2XeTex::set_include_full_references_with_notes()
{
  include_full_references_with_notes = true;
}


void Usfm2XeTex::optionally_add_full_references(ustring & line, Usfm2XslFoStyle * stylepointer)
{
  // Bail out if the full references are not to be added.
  if (!include_full_references_with_notes)
    return;

  // Get the project and its language.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ustring language = settings->projectconfig(project)->language_get();

  // Extract references.
  ReferencesScanner refscanner(language, book, line);

  // Add the full text of the references if there is any.
  for (unsigned int i = 0; i < refscanner.references.size(); i++) {
    line.append(" ");
    line.append(refscanner.references[i].human_readable(language));
    line.append(" ");
    ustring text = project_retrieve_verse(project, refscanner.references[i]);
    if (text.empty()) {
      line.append("<empty>");
    } else {
      CategorizeLine cl(text);
      line.append(cl.verse);
    }
  }
}


void Usfm2XeTex::set_language (const ustring& language)
// Sets the language. Used for localization operations.
{
  if (numeral_localization) 
    delete numeral_localization;
  numeral_localization = new NumeralLocalization (language);
}


