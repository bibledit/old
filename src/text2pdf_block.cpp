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

T2PBlock::T2PBlock(PangoRectangle initial_rectangle) :
  T2PArea(initial_rectangle)
// This is used as one block of text that must be kept together.
{
}

T2PBlock::~T2PBlock()
// Destructor.
{
  // Free the layout objects.
  for (unsigned int i = 0; i < layouts.size(); i++) {
    delete layouts[i];
  }
}

