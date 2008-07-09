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

#ifndef INCLUDED_TEXT2PDF_LAYOUT_H
#define INCLUDED_TEXT2PDF_LAYOUT_H

#include "libraries.h"
#include "text2pdf_area.h"
#include <pango/pangocairo.h>

class T2PLayoutContainer : public T2PArea
{
public:
  T2PLayoutContainer(PangoRectangle initial_rectangle, PangoLayout * layout_in);
  virtual ~T2PLayoutContainer();
  void print (cairo_t *cairo, int x_pango_units, int y_pango_units);
private:
  PangoLayout * layout;
};

#endif
