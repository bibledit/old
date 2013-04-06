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


#ifndef INCLUDED_USFM2XETEX_H
#define INCLUDED_USFM2XETEX_H


#include "libraries.h"
#include <libxml/xmlwriter.h>
#include "usfm2xslfo_utils.h"
#include "progresswindow.h"
#include "notecaller.h"
#include "portion_utils.h"
#include "xetex2pdf.h"
#include "localizednumerals.h"


class Usfm2XeTex
{
public:
  Usfm2XeTex(XeTeX2Pdf * xetex2pdf_in, bool show_progress);
  ~Usfm2XeTex();

  void add_styles(const vector <Usfm2XslFoStyle>& styles_in);
  void add_style(const ustring& marker_in, Usfm2XslFoStyleType type_in);
  gchar * default_style();
  gchar * font_family_size_line_height_style();
  void no_bold();
  void no_space_before_or_after();
  void no_new_page();

  void add_usfm_code(const vector <ustring>& lines);
  void add_usfm_code(const ustring& line);
  void set_fonts(const vector <ustring>& fonts_in, double font_size_in);
  void set_line_height(unsigned int line_height_in);
  void set_right_to_left();
  void set_margins(double top_cm, double bottom_cm, double inner_cm, double outer_cm);
  void set_two_columns();
  void set_even_page_count();
  void set_print_date();
  void set_include_full_references_with_notes();

  void add_print_portion(unsigned int book_in, vector<unsigned int> chapters_from_in, const vector<ustring>& verses_from_in, vector<unsigned int> chapters_to_in, const vector <ustring>& verses_to_in);

  void set_language (const ustring& language);
  
  void process();

private:
  // Usfm code management.
  vector <ustring> usfm_code;
  ustring usfm_line;
  unsigned int usfm_buffer_pointer;
  bool unprocessed_usfm_code_available();
  ustring get_next_chapter();

  // Styles, sizes, etc.
  vector <Usfm2XslFoStyle> styles;
  vector <ustring> fonts;
  double font_size;
  unsigned int line_height;
  bool right_to_left;
  bool two_columns;
  bool even_page_count;
  bool chapter_number_in_running_header_at_left_pages;
  bool chapter_number_in_running_header_at_right_pages;
  bool print_date;
  Usfm2XslFoStyle * marker_get_pointer_to_style(const ustring& marker);
  bool printversenumber;
  bool book_spans_columns;

  // Output.
  XeTeX2Pdf * xetex2pdf;

  // Basic xsl-fo management.
  void write_header_content();
  void open_paragraph(Usfm2XslFoStyle * style, bool keep_with_next_paragraph);
  void set_paragraph(Usfm2XslFoStyle * style, bool keep_with_next_paragraph);
  void open_inline(Usfm2XslFoStyle * style, Usfm2XslFoStyle * block);
  void close_possible_inline(Usfm2XslFoStyle * & style);

  // Preprocessing.
  void preprocess();

  // General processing.
  void convert_from_usfm_to_text();
  void output_text_starting_new_paragraph(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool keep_with_next_paragraph);
  void output_text_fallback(ustring& line);
  void output_text_character_style(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
  void output_elastic(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void output_spacing_paragraph(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
  void output_keep_on_page(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
  void output_font_family_size_line_height(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);

  // Chapter and running header processing.
  void output_chapter_number_try_normal(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, unsigned int book);
  ustring chapter_number_to_output_at_first_verse;
  void output_chapter_number_try_at_first_verse(ustring line, Usfm2XslFoStyle * & fo_block_style);
  map <unsigned int, unsigned int> highest_chapter_number;
  void output_running_header(ustring& line, Usfm2XslFoStyle * stylepointer, size_t marker_length, unsigned int book);
  vector <ChapterLabel> chapter_labels;
  vector <ChapterLabel> published_chapter_markers;

  // Progress windows.
  ProgressWindow * progresswindow;
  guint progress_event_id;
  static bool signal_progress_timeout(gpointer user_data);
  void signal_progress();
  bool cancel;

  // Table processing.
  void output_text_table(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);

  // Note processing.
  map <Usfm2XslFoStyle *, NoteCaller *> notecallers;
  vector <NoteCaller *> notecallers_per_page;
  void create_note_callers();
  void destroy_note_callers();
  void output_text_note(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, size_t marker_length, bool is_opener);
  Usfm2XslFoStyle * get_default_paragraph_style_for_note(Usfm2XslFoStyle * notestyle);
  void note_callers_new_book();
  void note_callers_new_chapter();
  void buffer_endnote(ustring& line, Usfm2XslFoStyle * stylepointer, size_t marker_length, bool is_opener);
  vector <ustring> buffered_endnote_texts;
  vector <Usfm2XslFoStyle *> buffered_endnote_styles;
  EndnotePositionType endnote_position;
  void dump_endnotes(Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style);
  bool include_full_references_with_notes;
  void optionally_add_full_references(ustring& line, Usfm2XslFoStyle * stylepointer);

  // Reference tracking and portion selection.
  unsigned int book;
  unsigned int chapter;
  ustring verse;
  void set_new_book(unsigned int book_in);
  void set_new_chapter(unsigned int chapter_in);
  void set_new_verse(const ustring& verse_in);
  WithinReferencesRange inrange;

  // Page breaks.
  unsigned int id_page_break_count;
  void output_id_page_break(Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style);
  void output_other_page_break(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void output_page_break(Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, bool oddpage);
  bool do_not_allow_new_page;

  // Pictures.
  void output_picture(ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);

  // Table of contents.
  vector <unsigned int> books;
  map <unsigned int, ustring> toc_texts_short;
  map <unsigned int, ustring> toc_texts_long;
  void toc_collect_text(ustring& line, size_t marker_length, bool longtext, unsigned int book);
  void toc_insert_referent(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void toc_insert_body(ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);

  // Verse processing.
  bool output_verse_number(Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  unsigned int verses_in_paragraph_count;
  
  // Localizations.
  NumeralLocalization * numeral_localization;
};


#endif
