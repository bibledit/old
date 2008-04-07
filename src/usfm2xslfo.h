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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_USFM2XSLFO_H
#define INCLUDED_USFM2XSLFO_H


#include "libraries.h"
#include <libxml/xmlwriter.h>
#include "usfm2xslfo_utils.h"
#include "progresswindow.h"
#include "notecaller.h"


class Usfm2XslFo
{
public:
  Usfm2XslFo (const ustring& pdfoutputfile);
  ~Usfm2XslFo ();

  void add_styles (const vector <Usfm2XslFoStyle>& styles_in);
  void add_style (const ustring& marker_in, Usfm2XslFoStyleType type_in);
  gchar * default_style ();
  gchar * column_balancing_style ();
  gchar * line_spacing_style ();
  gchar * keep_together_on_page_style ();
  gchar * font_family_size_line_height_style ();
  void no_bold ();
  void no_space_before_or_after ();
  void no_new_page ();

  void add_usfm_code (const vector <ustring>& lines);
  void add_usfm_code (const ustring& line);
  void set_fonts (const vector <ustring>& fonts_in, double font_size_in);
  void set_line_height (unsigned int line_height_in);
  void set_right_to_left ();
  void set_page_size (double width_cm, double height_cm);
  void set_margins (double top_cm, double bottom_cm, double inner_cm, double outer_cm);
  void set_two_columns ();
  void set_even_page_count ();
  void set_print_date ();

  void add_print_portion (const XslFoPortion& portion);

  void process ();

  void display (GtkWidget * window);

private:
  // Usfm code management.
  vector <ustring> usfm_code;
  ustring usfm_line;
  unsigned int usfm_buffer_pointer;
  ustring get_erase_code_till_next_marker (ustring& line, size_t marker_position, size_t marker_length, bool trimcode);
  bool unprocessed_usfm_code_available ();
  ustring get_next_chapter ();

  // Styles, sizes, etc.
  vector <Usfm2XslFoStyle> styles;
  vector <ustring> fonts;
  double font_size;
  unsigned int line_height;
  bool right_to_left;
  double page_width, page_height;
  double top_margin, bottom_margin, inner_margin, outer_margin;
  bool two_columns;
  bool even_page_count;
  bool chapter_number_in_running_header_at_left_pages;
  bool chapter_number_in_running_header_at_right_pages;
  bool print_date;
  Usfm2XslFoStyle * marker_get_pointer_to_style (const ustring& marker);
  bool printversenumber;
  bool book_spans_columns;

  // Output.
  xmlBufferPtr buffer;
  xmlTextWriterPtr writer;
  ustring xslfofile ();
  ustring xepfile ();
  ustring pdffile;

  // Basic xsl-fo management.
  void write_root_open ();
  void write_layout_master_set ();
  void write_page_sequence_open ();
  void write_static_content ();
  void write_flow_open ();
  void open_fo_block (Usfm2XslFoStyle * style, bool keep_with_next_paragraph);
  void ensure_fo_block_open (Usfm2XslFoStyle * & style);
  void close_possible_fo_block (Usfm2XslFoStyle * & style);
  void open_fo_inline (Usfm2XslFoStyle * style, Usfm2XslFoStyle * block);
  void close_possible_fo_inline (Usfm2XslFoStyle * & style);

  // Preprocessing.
  void preprocess ();
  
  // General processing.
  void convert_from_usfm_to_xslfo ();
  void output_text_starting_new_paragraph (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool keep_with_next_paragraph);
  void output_text_fallback (ustring& line, Usfm2XslFoStyle * & fo_block_style);
  void output_text_running_header (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, unsigned int book);
  void output_verse_number (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void output_text_character_style (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
  void output_column_balancer (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void output_elastic (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void output_spacing_paragraph  (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
  void output_keep_on_page  (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
  void output_font_family_size_line_height  (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);

  // Chapter and running header processing.
  void output_chapter_number_try_normal (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, unsigned int book);
  unsigned int chapter_number_to_output_at_first_verse;
  void output_chapter_number_try_at_first_verse (ustring line, Usfm2XslFoStyle * & fo_block_style);
  map <unsigned int, unsigned int> highest_chapter_number;
  map <unsigned int, ustring> book_header_left;
  map <unsigned int, ustring> book_header_right;
  void collect_running_headers (ustring& line, Usfm2XslFoStyle * stylepointer, size_t marker_length, unsigned int book);
  vector <ChapterLabel> chapter_labels;
  vector <ChapterLabel> published_chapter_markers;
  
  // Progress windows.
  ProgressWindow * progresswindow;
  guint progress_event_id;
  static bool signal_progress_timeout (gpointer user_data);
  void signal_progress ();
  bool cancel;

  // Table processing.
  void output_text_table (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  
  // Note processing.
  map <Usfm2XslFoStyle *, NoteCaller *> notecallers;
  vector <NoteCaller *> notecallers_per_page;
  void create_note_callers ();
  void destroy_note_callers ();
  void output_text_note (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, size_t marker_length, bool is_opener);
  Usfm2XslFoStyle * get_default_paragraph_style_for_note (Usfm2XslFoStyle * notestyle);
  void rewrite_note_callers ();
  void note_callers_new_book ();
  void note_callers_new_chapter ();
  void buffer_endnote (ustring& line, Usfm2XslFoStyle * stylepointer, size_t marker_length, bool is_opener);
  vector <ustring> buffered_endnote_texts;
  vector <Usfm2XslFoStyle *> buffered_endnote_styles;
  EndnotePositionType endnote_position;
  void dump_endnotes (Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style);
  
  // Portion selection.
  vector <XslFoPortion> portions;
  bool print_this_portion;
  unsigned int book;
  unsigned int chapter;
  ustring verse;
  void portion_new_book (unsigned int book_in);
  void portion_new_chapter (unsigned int chapter_in);
  void portion_new_verse (const ustring& verse_in);
  void portion_check ();
  
  // Page breaks.
  unsigned int id_page_break_count;
  void output_id_page_break (Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style);
  void output_other_page_break  (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void output_page_break  (Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, bool oddpage);
  bool do_not_allow_new_page;
  
  // Pictures.
  void output_picture (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  
  // Table of contents.
  vector <unsigned int> books;
  map <unsigned int, ustring> toc_texts_short;
  map <unsigned int, ustring> toc_texts_long;
  ustring toc_identifier (unsigned int book);
  void toc_collect_text (ustring& line, size_t marker_length, bool longtext, unsigned int book);
  void toc_insert_anchor (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  void toc_insert_body  (ustring& line, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length);
  
  // Insertions / deletions
  void output_text_insertion_deletion (ustring& line, Usfm2XslFoStyle * stylepointer, Usfm2XslFoStyle * & fo_block_style, Usfm2XslFoStyle * & fo_inline_style, size_t marker_length, bool is_opener);
};


#endif
