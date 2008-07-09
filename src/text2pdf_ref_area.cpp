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

#include "text2pdf_utils.h"
#include "text2pdf_ref_area.h"

T2PReferenceArea::T2PReferenceArea(PangoRectangle initial_rectangle) :
  T2PArea(initial_rectangle)
// This is a reference area, e.g. a header, footer, or text area.
{
}

T2PReferenceArea::~T2PReferenceArea()
// Destructor.
{
  for (unsigned int i = 0; i < columns.size(); i++) {
    delete columns[i];
  }
}

T2PColumn * T2PReferenceArea::next_column ()
// Creates and stores the next available column.
{
  PangoRectangle column_rectangle;
  column_rectangle.x = rectangle.x;
  column_rectangle.y = rectangle.y;
  column_rectangle.width = rectangle.width;
  column_rectangle.height = rectangle.height;
  T2PColumn * column = new T2PColumn (column_rectangle);
  columns.push_back (column);
  return column;
}
