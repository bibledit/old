/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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

#include "text2pdf_big_block.h"
#include "text2pdf_utils.h"

T2PBigBlock::T2PBigBlock(int column_count_in)
// This is used as one lot of blocks that must be kept together.
{
  total_height = 0;
  height_first_block = 0;
  column_count = column_count_in;
}

T2PBigBlock::~T2PBigBlock()
// Destructor.
{
}

void T2PBigBlock::calculate_height()
// Calculates the total height of the blocks, and stores the data internally.
{
  for (unsigned int i = 0; i < blocks.size(); i++) {
    if (i == 0) {
      height_first_block = blocks[i]->rectangle.height;
    }
    total_height += blocks[i]->rectangle.height;
  }
}

int T2PBigBlock::height(int reference_y)
// Get the total height of the blocks.
{
  int height = total_height;
  if (!blocks.empty()) {
    if ((reference_y == 0) && (blocks[0]->type == t2pbtSpaceBeforeParagraph)) {
      height -= height_first_block;
    }
  }
  return height;
}

void T2PBigBlock::set_blocks_x(int x)
// Writes the x value to each block.
{
  for (unsigned int i = 0; i < blocks.size(); i++) {
    blocks[i]->rectangle.x = x;
  }
}

void T2PBigBlock::set_blocks_y(int reference_y)
// Writes the y values to each block, starting at the reference y.
{
  int y = reference_y;
  for (unsigned int i = 0; i < blocks.size(); i++) {
    T2PBlock *block = blocks[i];
    block->rectangle.y = y;
    bool take_block_into_account = true;
    if ((i == 0) && (reference_y == 0) && (block->type == t2pbtSpaceBeforeParagraph)) {
      take_block_into_account = false;
    }
    if (take_block_into_account) {
      y += block->rectangle.height;
    }
  }
}
