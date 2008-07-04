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

#ifndef INCLUDED_USFM2PDF_H
#define INCLUDED_USFM2PDF_H

#include "libraries.h"
#include <pango/pangocairo.h>
#include "text2pdf_utils.h"
#include "text2pdf_area.h"

class Text2Pdf
{
public:
  Text2Pdf(const ustring& pdf_file);
  ~Text2Pdf();

  // Initialization.
public:
private:
  void initialize_variables();

  // Cairo data.
public:
private:
  cairo_t *cairo;
  cairo_status_t status;
  cairo_surface_t *surface;

  // Areas and their sizes.
public:
  void page_size_set(double width_centimeters, double height_centimeters);
  void page_margins_set(double inside_margin_centimeters, double outside_margin_centimeters, double top_margin_centimeters, double bottom_margin_centimeters);
  void header_height_set(double size_centimeters);
  void footer_height_set(double size_centimeters);
  void column_spacing_set(double spacing_centimeters);
private:
  int page_width_pango_units, page_height_pango_units;
  int inside_margin_pango_units, outside_margin_pango_units, top_margin_pango_units, bottom_margin_pango_units;
  int header_height_pango_units;
  int footer_height_pango_units;
  int column_spacing_pango_units;
  vector <T2PPage *> pages;
  
  // Running.  
public:
  void run();
private:
};

#endif
