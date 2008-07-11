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

T2PColumn::T2PColumn(PangoRectangle rectangle_in) :
  T2PArea(rectangle_in)
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

T2PBlock * T2PColumn::next_block()
// Obtains the next block in the column.
{
  // Only the width of the block is set here, because the other measures depend on factors not yet known at this stage.
  PangoRectangle block_rectangle;
  block_rectangle.x = 0;
  block_rectangle.y = 0;
  block_rectangle.width = rectangle.width;
  block_rectangle.height = 0;
  // Create a new block.
  T2PBlock * block;
  block = new T2PBlock (block_rectangle);
  // Store the block.
  blocks.push_back(block);
  // Return the block.
  return block;
}


void T2PColumn::add_block(T2PBlock * block)
{
  blocks.push_back(block);
}

void T2PColumn::fit_blocks(vector <T2PBlock *>& non_fitting_blocks)
// Fits the blocks into the column.
{
  // (Non-)fitting blocks.
  vector <T2PBlock *> fitting_blocks;
  non_fitting_blocks.clear();
  // Go through the available blocks to fit them in and sort them out.
  for (unsigned int blk = 0; blk < blocks.size(); blk++) {
    // The x value of the block will be the x value of the column.  
    blocks[blk]->rectangle.x = rectangle.x;
    // The y value of the block will be calculated based on stacked blocks before.
    blocks[blk]->rectangle.y = 0;
    if (blk > 0) {
      T2PBlock * previous_block= blocks[blk-1];
      blocks[blk]->rectangle.y = previous_block->rectangle.y + previous_block->rectangle.height;
    }
    // If the block doesn't fit, move it to the non-fitting ones.
    if (blocks[blk]->rectangle.y + blocks[blk]->rectangle.height > rectangle.height) {
      non_fitting_blocks.push_back(blocks[blk]);
    } else {
      fitting_blocks.push_back(blocks[blk]);
    }
  }
  // Store the fitting blocks, leaving out any that don't fit.
  blocks = fitting_blocks;
}

void T2PColumn::print(cairo_t *cairo)
// Print the column.
{
  // Go through each block in the column.
  for (unsigned int blk = 0; blk < blocks.size(); blk++) {
    T2PBlock * block = blocks[blk];
    // Normally the block's x and y values are relative to the parent's values.
    // This is faster for laying out the text. But for printing this needs to be adjusted.
    block->rectangle.x += rectangle.x;
    block->rectangle.y += rectangle.y;
    // Print this block.
    block->print(cairo);
  }
}
