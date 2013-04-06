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

#ifndef INCLUDED_TEXT2PDF_BLOCK_H
#define INCLUDED_TEXT2PDF_BLOCK_H

#include "libraries.h"
#include "text2pdf_area.h"
#include <pango/pangocairo.h>
#include "text2pdf_layout.h"

enum T2PBlockType { t2pbtTextGeneral, t2pbtTextParagraphFirstLine, t2pbtTextParagraphLastLine, t2pbtTextParagraphOnlyLine,
                    t2pbtTextIntrusion,
                    t2pbtSpaceBeforeParagraph, t2pbtSpaceAfterParagraph,
                    t2pbtNewPage, t2pbtNewOddPage};

class T2PBlock : public T2PArea
{
public:
  T2PBlock(PangoRectangle rectangle_in, int column_count_in);
  virtual ~T2PBlock();
  vector <T2PLayoutContainer *> layoutcontainers;
  T2PLayoutContainer * next_layout_container (cairo_t *cairo);
  void store_layout_container_height (T2PLayoutContainer * layoutcontainer);
  void store_layout_container_width (T2PLayoutContainer * layoutcontainer);
  void store_layout_container_x (T2PLayoutContainer * layoutcontainer);
  void set_widow_orphan_data (int paragraph_line_number, bool last_line_of_paragraph);
  void print(cairo_t *cairo);
  ustring text();
  int column_count;
  T2PBlockType type;
  bool keep_with_next;
  int maximum_width_pango_units;
  ustring left_running_header;
  ustring right_running_header;
  unsigned int left_running_chapter;
  unsigned int right_running_chapter;
  bool suppress_header;
  ustring reference;
  ustring referent;
private:
};

#endif
