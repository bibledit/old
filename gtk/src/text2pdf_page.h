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

#ifndef INCLUDED_TEXT2PDF_PAGE_H
#define INCLUDED_TEXT2PDF_PAGE_H

#include "libraries.h"
#include <pango/pangocairo.h>
#include "text2pdf_ref_area.h"

class T2PPage
{
public:
  T2PPage(int page_number, int page_width, int page_height, int inside_margin, int outside_margin, int top_margin, int bottom_margin, int header_height, int footer_height, cairo_t *cairo_in, bool print_date_in, bool right_to_left_in);
  virtual ~T2PPage();
  void print();
  T2PReferenceArea * header_reference_area;
  T2PReferenceArea * text_reference_area;
  T2PReferenceArea * footer_reference_area;
private:
  int number;
  cairo_t *cairo;
  bool print_date;
  bool right_to_left;
};

#endif
