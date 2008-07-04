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

#include "text2pdf_area.h"

T2PArea::T2PArea(PangoRectangle initial_rectangle, T2PArea * parent_in)
// This is the base class for any area in which to lay out text.
{
  rectangle = initial_rectangle;
  parent = parent_in;
  type = tp2atArea;
}

T2PArea::~T2PArea() {
}

T2PReferenceArea::T2PReferenceArea(PangoRectangle initial_rectangle, T2PArea * parent_in) :
  T2PArea(initial_rectangle, parent_in)
// This is a reference area, e.g. a header, footer, or text area.
{
  type = tp2atReference;
}

T2PReferenceArea::~T2PReferenceArea() {
}

T2PPage::T2PPage(int page_number, int page_width, int page_height, int inside_margin, int outside_margin, int top_margin, int bottom_margin, int header_height, int footer_height)
/* This encapsulates one page.

 ------------------------------------------
 |  |           top margin             |  |
 |  ------------------------------------  |
 |  |           header                 |  |
 |  ------------------------------------  |
 |  |                                  |  |
 |  |                                  |  |
 |  |                                  |  |
 |  |                                  |  |
 |  |                                  |  |
 |  |           text                   |  |
 |  |                                  |  |
 |  |                                  |  |
 |  |                                  |  |
 |  |                                  |  |
 |  |                                  |  |
 |  ------------------------------------  |
 |  |           footer                 |  |
 |  ------------------------------------  |
 |  |           bottom margin          |  |
 ------------------------------------------

 */
{
  // Initialize variables.
  number = page_number;
  // Create the reference area for the header.
  {
    PangoRectangle rectangle;
    if ((page_number % 2) != 0)
      rectangle.x = inside_margin;
    else
      rectangle.x = outside_margin;
    rectangle.y = top_margin;
    rectangle.width = page_width - inside_margin - outside_margin;
    rectangle.height = header_height;
    header_reference_area = new T2PReferenceArea (rectangle, NULL);
  }

  // Create the reference area for the text.
  {
    PangoRectangle rectangle;
    if ((page_number % 2) != 0)
      rectangle.x = inside_margin;
    else
      rectangle.x = outside_margin;
    rectangle.y = top_margin + header_height;
    rectangle.width = page_width - inside_margin - outside_margin;
    rectangle.height = page_height - top_margin - header_height - footer_height - bottom_margin;
    text_reference_area = new T2PReferenceArea (rectangle, NULL);
  }

  // Create the reference area for the footer.
  {
    PangoRectangle rectangle;
    if ((page_number % 2) != 0)
      rectangle.x = inside_margin;
    else
      rectangle.x = outside_margin;
    rectangle.y = page_height - bottom_margin;
    rectangle.width = page_width - inside_margin - outside_margin;
    rectangle.height = footer_height;
    footer_reference_area = new T2PReferenceArea (rectangle, NULL);
  }
}

T2PPage::~T2PPage() {
  delete header_reference_area;
  delete text_reference_area;
  delete footer_reference_area;
}

T2PSingleColumn::T2PSingleColumn(PangoRectangle initial_rectangle, T2PArea * parent_in) :
  T2PArea(initial_rectangle, parent_in)
// This is used as a single column in which to lay out text.
{
  type = tp2atSingleColumn;
}

T2PSingleColumn::~T2PSingleColumn() {
}

T2PBalancedColumn::T2PBalancedColumn(PangoRectangle initial_rectangle, T2PArea * parent_in) :
  T2PArea(initial_rectangle, parent_in)
// This encapsulates two balanced columns.
{
  start_column = NULL;
  end_column = NULL;
  type = tp2atBalancedColumn;
}

T2PBalancedColumn::~T2PBalancedColumn() {
  if (start_column)
    delete start_column;
  if (end_column)
    delete end_column;
}

