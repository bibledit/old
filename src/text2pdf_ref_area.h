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

#ifndef INCLUDED_TEXT2PDF_REF_AREA_H
#define INCLUDED_TEXT2PDF_REF_AREA_H

#include "libraries.h"
#include <pango/pangocairo.h>
#include "text2pdf_area.h"
#include "text2pdf_block.h"

class T2PReferenceArea : public T2PArea
{
public:
  T2PReferenceArea(PangoRectangle rectangle_in);
  virtual ~T2PReferenceArea();
  T2PBlock * next_block ();
  void add_block(T2PBlock * block);
  void fit_blocks (deque <T2PBlock *>& input_blocks);
  void fit_column (deque <T2PBlock *>& input_blocks);
  void fit_columns (deque <T2PBlock *>& input_blocks, int column_count);
  int fitted_blocks_height_pango_units ();
  void balance_two_columns (int start_block, int start_height);
  void print(cairo_t *cairo);
private:
  vector <T2PBlock *> blocks;
};

#endif
