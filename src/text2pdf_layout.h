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
#include "text2pdf_input.h"
#include "text2pdf_block.h"

class T2PLayoutContainer : public T2PArea
{
public:
  T2PLayoutContainer(PangoRectangle rectangle_in, T2PArea * parent_in, cairo_t *cairo);
  virtual ~T2PLayoutContainer();
  void print (cairo_t *cairo);
  void layout_text (const ustring& font, T2PInputParagraph * paragraph, unsigned int line_number, ustring& text);
  T2PArea * parent;
  void debug ();
private:
  PangoLayout * layout;
  void indentation_width_alignment (T2PInputParagraph * paragraph, bool first_line);
  void justify (T2PInputParagraph * paragraph, const ustring& line, bool last_line);
  void index_white_space (const ustring& text, vector<guint>& offsets);
};

#endif
