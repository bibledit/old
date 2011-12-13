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

#ifndef INCLUDED_TEXT2PDF_BIG_BLOCK_H
#define INCLUDED_TEXT2PDF_BIG_BLOCK_H

#include "libraries.h"
#include "text2pdf_block.h"

class T2PBigBlock
{
public:
  T2PBigBlock(int column_count_in);
  virtual ~T2PBigBlock();
  vector <T2PBlock *> blocks;
  void calculate_height();
  int height (int reference_y);
  void set_blocks_x (int x);
  void set_blocks_y (int reference_y);
  int column_count;
private:
  int total_height;
  int height_first_block;
};

#endif
