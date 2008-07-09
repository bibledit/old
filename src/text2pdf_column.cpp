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
#include "text2pdf_column.h"

T2PColumn::T2PColumn(PangoRectangle initial_rectangle) :
  T2PArea(initial_rectangle)
// This is used as a column in which to lay out text.
{
}

T2PColumn::~T2PColumn()
// Destructor.
{
  for (unsigned int i = 0; i < blocks.size(); i++) {
    delete blocks[i];
  }
}

void T2PColumn::add_block(T2PBlock * block)
// Adds a block to the column.
{
  // The x value of the block will be the x value of the column.  
  block->rectangle.x = rectangle.x;
  // The y value of the block will be calculated based upon any previously stacked blocks.
  int stacked_y = rectangle.y;
  T2PBlock * last_stacked_block= NULL;
  if (!blocks.empty()) {
    last_stacked_block = blocks[blocks.size() - 1];
    stacked_y = last_stacked_block->rectangle.y + last_stacked_block->rectangle.height;
  }
  block->rectangle.y = stacked_y;
  // Store the block.
  blocks.push_back(block);
}

