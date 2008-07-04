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

#include "text2pdf_text.h"

T2PText::T2PText(T2PText * parent_in)
// This is the base class for any area in which to lay out text.
{
  parent = parent_in;
  type = tp2ttParagraph;
}

T2PText::~T2PText() {
}

/*
T2PReferenceArea::T2PReferenceArea(PangoRectangle initial_rectangle, T2PArea * parent_in) :
  T2PArea(initial_rectangle, parent_in)
// This is a reference area, e.g. a header, footer, or text area.
{
  type = tp2atReference;
}

T2PReferenceArea::~T2PReferenceArea() {
}

*/

