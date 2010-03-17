/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
#include "text2pdf_block.h"

T2PBlock::T2PBlock(PangoRectangle rectangle_in, int column_count_in):T2PArea(rectangle_in)
// This is used as one block of text that must be kept together.
{
  // Default type is text.
  type = t2pbtTextGeneral;

  // By default the block is not kept together with the next one.
  keep_with_next = false;

  // Columns, size.
  column_count = column_count_in;
  maximum_width_pango_units = rectangle.width;

  // Running header variables.
  left_running_chapter = 0;
  right_running_chapter = 0;
  suppress_header = false;
}

T2PBlock::~T2PBlock()
// Destructor.
{
  // Free the layout objects.
  for (unsigned int i = 0; i < layoutcontainers.size(); i++) {
    delete layoutcontainers[i];
  }
}

T2PLayoutContainer *T2PBlock::next_layout_container(cairo_t * cairo)
// Gets the next layout container object.
{
  T2PLayoutContainer *layoutcontainer = new T2PLayoutContainer(rectangle, this, cairo);
  layoutcontainers.push_back(layoutcontainer);
  return layoutcontainer;
}

void T2PBlock::store_layout_container_height(T2PLayoutContainer * layoutcontainer)
// Stores the layout container's height in the block.
{
  rectangle.height = layoutcontainer->rectangle.height;
}

void T2PBlock::store_layout_container_width(T2PLayoutContainer * layoutcontainer)
// Stores the layout container's width in the block.
{
  rectangle.width = layoutcontainer->rectangle.width;
}

void T2PBlock::store_layout_container_x(T2PLayoutContainer * layoutcontainer)
// Stores the layout container's x in the block.
{
  rectangle.x = layoutcontainer->rectangle.x;
}

void T2PBlock::print(cairo_t * cairo)
// Print the block.
{
  for (unsigned int l = 0; l < layoutcontainers.size(); l++) {
    T2PLayoutContainer *layoutcontainer = layoutcontainers[l];
    layoutcontainer->rectangle.x += rectangle.x;
    layoutcontainer->rectangle.y += rectangle.y;
    layoutcontainer->print(cairo);
  }
}

ustring T2PBlock::text()
// Shows block's text.
{
  ustring s;
  for (unsigned int l = 0; l < layoutcontainers.size(); l++) {
    if (l)
      s.append(" ");
    s.append(layoutcontainers[l]->text());
  }
  return s;
}

void T2PBlock::set_widow_orphan_data(int paragraph_line_number, bool last_line_of_paragraph)
// Sets data to be used later for widow and orphan control.
{
  if (paragraph_line_number == 0) {
    type = t2pbtTextParagraphFirstLine;
  }
  if (last_line_of_paragraph) {
    type = t2pbtTextParagraphLastLine;
  }
  if ((paragraph_line_number == 0) && last_line_of_paragraph) {
    type = t2pbtTextParagraphOnlyLine;
  }
}

