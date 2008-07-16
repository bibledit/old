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

#ifndef INCLUDED_TEXT2PDF_H
#define INCLUDED_TEXT2PDF_H

#include "libraries.h"
#include <pango/pangocairo.h>
#include "text2pdf_utils.h"
#include "text2pdf_area.h"
#include "text2pdf_block.h"
#include "text2pdf_ref_area.h"
#include "text2pdf_input.h"
#include "text2pdf_page.h"

class Text2Pdf
{
public:
  Text2Pdf(const ustring& pdf_file);
  ~Text2Pdf();

  // Initialization.
public:
private:
  void initialize_variables();

  // Cairo data.
public:

private:
  cairo_t *cairo;
  cairo_status_t status;
  cairo_surface_t *surface;

  // Input data.
public:
  void open_paragraph(int first_line_indent_mm, T2PAlignmentType alignment, unsigned int column_count);
  void open_paragraph();
  T2PInputParagraph * input_paragraph;
  void close_paragraph();
  void open_inline();
  void close_inline();
  void add_text(const ustring& text);
private:
  vector <T2PInput *> input_data;
  deque <T2PBlock *> input_blocks;

  // Areas and their sizes.
public:
  void page_size_set(double width_centimeters, double height_centimeters);
  void page_margins_set(double inside_margin_centimeters, double outside_margin_centimeters, double top_margin_centimeters, double bottom_margin_centimeters);
  void header_height_set(double size_centimeters);
  void footer_height_set(double size_centimeters);
  void column_spacing_set(double spacing_centimeters);
private:
  int page_width_pango_units, page_height_pango_units;
  int inside_margin_pango_units, outside_margin_pango_units, top_margin_pango_units, bottom_margin_pango_units;
  int header_height_pango_units;
  int footer_height_pango_units;
  int column_spacing_pango_units;

  // Fonts.
public:
  void set_font(const ustring& font_in);
private:
  ustring font;

  // Running.
public:
  void run();
private:
  void run_input(vector <T2PInput *>& input);
  T2PPage * page;
  vector <T2PPage *> pages;
  T2PBlock * block;
  T2PLayoutContainer * layoutcontainer;
  void lay_out_paragraph(const ustring& paragraph);
  void get_next_layout_container();
  void fit_blocks_on_pages();
  void next_page();

  // Output.
public:
  void view();
private:
  ustring pdffile;
};

#endif
