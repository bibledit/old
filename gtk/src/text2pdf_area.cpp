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

#include "text2pdf_area.h"
#include "text2pdf_utils.h"
#include "tiny_utilities.h"

T2PArea::T2PArea(PangoRectangle rectangle_in)
// This is the base class for any area in which to lay out text.
{
  rectangle = rectangle_in;
}

T2PArea::~T2PArea()
// Destructor.
{
}

ustring T2PArea::rectangle2text()
// Gives the rectangle as text. For debugging purposes.
{
  ustring text;
  text.append("x=" + convert_to_string(pango_units_to_millimeters(rectangle.x)));
  text.append(" y=" + convert_to_string(pango_units_to_millimeters(rectangle.y)));
  text.append(" width=" + convert_to_string(pango_units_to_millimeters(rectangle.width)));
  text.append(" height=" + convert_to_string(pango_units_to_millimeters(rectangle.height)));
  return text;
}
