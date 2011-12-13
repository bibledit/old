/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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

#ifndef INCLUDED_TEXT2PDF_REF_AREA_H
#define INCLUDED_TEXT2PDF_REF_AREA_H

#include "libraries.h"
#include <pango/pangocairo.h>
#include "text2pdf_area.h"
#include "text2pdf_block.h"
#include "text2pdf_big_block.h"

class T2PReferenceArea : public T2PArea
{
public:
  T2PReferenceArea(PangoRectangle rectangle_in, cairo_t *cairo_in, bool right_to_left_in);
  virtual ~T2PReferenceArea();
  void fit_blocks(deque <T2PBlock *>& input_blocks, int column_spacing_pango_units_in);
  bool has_content();
  void print();
  void print(unsigned int page_number, bool print_date, const ustring& left_running_header, const ustring& right_running_header, bool suppress_header, unsigned int left_first_chapter, unsigned int left_last_chapter, unsigned int right_first_chapter, unsigned int right_last_chapter);
  ustring left_running_header();
  ustring right_running_header();
  unsigned int left_running_first_chapter();
  unsigned int right_running_first_chapter();
  unsigned int left_running_last_chapter();
  unsigned int right_running_last_chapter();
  bool suppress_headers();
  bool start_new_odd_page;
  void get_referents(map <ustring, ustring>& referents, unsigned int page_number);
  void match_and_expand_references_and_referents(map <ustring, ustring>& referents);
private:
  cairo_t *cairo;
  deque <T2PBlock *> body_blocks;
  int start_stacking_y;
  int column_spacing_pango_units;
  void fit_column(deque <T2PBlock *>& input_blocks);
  void fit_columns(deque <T2PBlock *>& input_blocks, int column_count);
  T2PBigBlock get_next_big_block_to_be_kept_together(deque <T2PBlock *>& input_blocks, int column_count);
  int get_column_height(deque <T2PBigBlock>& column, int reference_y);
  void balance_columns(deque <T2PBigBlock>& first_column, deque <T2PBigBlock>& last_column);
  int balance_last_column_higher_than_or_equal_to_first_column(deque <T2PBigBlock>& first_column, deque <T2PBigBlock>& last_column);
  int balance_first_column_higher_than_or_equal_to_last_column(deque <T2PBigBlock>& first_column, deque <T2PBigBlock>& last_column);
  deque <T2PLayoutContainer *> note_layout_containers;
  PangoRectangle get_next_free_note_rectangle();
  int get_note_height();
  ustring produce_running_header(const ustring& header, bool suppress, unsigned int first_chapter, unsigned int last_chapter);
  bool new_page_input_block_encountered(deque <T2PBlock *>& input_blocks, bool set_flag);
  bool start_new_page;
  deque <T2PLayoutContainer *> table_of_contents_layout_containers;
  
  // Right to left script.
  bool right_to_left;

};

#endif
