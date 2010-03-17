/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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

#ifndef INCLUDED_TEXT2PDF_LAYOUT_H
#define INCLUDED_TEXT2PDF_LAYOUT_H

#include "libraries.h"
#include "text2pdf_area.h"
#include <pango/pangocairo.h>
#include "text2pdf_input.h"

class T2PLayoutContainer : public T2PArea
{
public:
  T2PLayoutContainer(PangoRectangle rectangle_in, T2PArea * parent_in, cairo_t *cairo);
  virtual ~T2PLayoutContainer();
  void print(cairo_t *cairo);
  void layout_text(T2PInputParagraph * paragraph, unsigned int line_number, string& text);
  void undo_layout_text();
  ustring text();
  vector <T2PInputParagraph *> note_paragraphs;
  void set_has_note();
  void layout_drop_caps(T2PInputParagraph * paragraph, double context_font_size);
private:
  T2PArea * parent;
  PangoLayout * layout;
  void indentation_width_margins_alignment(bool first_line, bool right_to_left);
  void justify(const string& line, bool last_line, PangoAttrList *attrs);
  void index_white_space(const string& text, vector<guint>& offsets);
  void set_font(PangoAttrList *attrs);
  void set_italic(PangoAttrList *attrs);
  void set_bold(PangoAttrList *attrs);
  void set_underline(PangoAttrList *attrs);
  void set_small_caps(PangoAttrList *attrs);
  void set_superscript(PangoAttrList *attrs);
  void set_colour(PangoAttrList *attrs);
  void set_strike_through(PangoAttrList *attrs);
  bool has_note;
  T2PInputParagraph * input_paragraph;
  string input_text;
};

#endif
