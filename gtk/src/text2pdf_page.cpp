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

#include "text2pdf_area.h"
#include "text2pdf_utils.h"
#include "text2pdf_ref_area.h"
#include "text2pdf_page.h"

T2PPage::T2PPage(int page_number, int page_width, int page_height, int inside_margin, int outside_margin, int top_margin, int bottom_margin, int header_height, int footer_height, cairo_t * cairo_in, bool print_date_in, bool right_to_left_in)
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
  cairo = cairo_in;
  print_date = print_date_in;
  right_to_left = right_to_left_in;
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
    header_reference_area = new T2PReferenceArea(rectangle, cairo, right_to_left);
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
    text_reference_area = new T2PReferenceArea(rectangle, cairo, right_to_left);
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
    footer_reference_area = new T2PReferenceArea(rectangle, cairo, right_to_left);
  }
}

T2PPage::~T2PPage()
// Destructor.
{
  delete header_reference_area;
  delete text_reference_area;
  delete footer_reference_area;
}

void T2PPage::print()
// Prints the page.
{
  // White background.
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_paint(cairo);

  // Headers.
  if (text_reference_area->has_content()) {
    header_reference_area->print(number, print_date, text_reference_area->left_running_header(), text_reference_area->right_running_header(), text_reference_area->suppress_headers(), text_reference_area->left_running_first_chapter(), text_reference_area->left_running_last_chapter(), text_reference_area->right_running_first_chapter(), text_reference_area->right_running_last_chapter());
  }
  // Print the reference areas.
  header_reference_area->print();
  text_reference_area->print();
  footer_reference_area->print();

  // Output this page.
  cairo_show_page(cairo);
}
