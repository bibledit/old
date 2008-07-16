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
#include "text2pdf_utils.h"
#include "text2pdf_ref_area.h"
#include "text2pdf_block.h"

T2PBlock::T2PBlock(PangoRectangle rectangle_in, int column_count_in, int column_spacing_pango_units) :
  T2PArea(rectangle_in)
// This is used as one block of text that must be kept together.
{
  // Column handling.
  // If there's more than one colum, the width of the block gets smaller.
  column_count = CLAMP (column_count_in, 1, 2);
  if (column_count > 1) {
    rectangle.width /= column_count;
    rectangle.width -= (column_count - 1) * column_spacing_pango_units;
  }
}

T2PBlock::~T2PBlock()
// Destructor.
{
  // Free the layout objects.
  for (unsigned int i = 0; i < layoutcontainers.size(); i++) {
    delete layoutcontainers[i];
  }
}

T2PLayoutContainer * T2PBlock::next_layout_container (cairo_t *cairo)
// Gets the next layout container object.
{
  T2PLayoutContainer * layoutcontainer = new T2PLayoutContainer (rectangle, this, cairo);
  layoutcontainers.push_back(layoutcontainer);
  return layoutcontainer;
}

void T2PBlock::refit_layout_container (T2PLayoutContainer * layoutcontainer)
// Refits the layout container in the block.
{
  rectangle.height = layoutcontainer->rectangle.height;
}

void T2PBlock::print(cairo_t *cairo)
// Print the block.
{
  for (unsigned int l = 0; l < layoutcontainers.size(); l++) {
    T2PLayoutContainer * layoutcontainer = layoutcontainers[l];
    layoutcontainer->rectangle.x += rectangle.x;
    layoutcontainer->rectangle.y += rectangle.y;
    layoutcontainer->print (cairo);
  }
}

void T2PBlock::debug()
// Show debug information.
{
  for (unsigned int l = 0; l < layoutcontainers.size(); l++) {
    layoutcontainers[l]->debug();
  }
}
