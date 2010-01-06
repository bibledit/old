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


#ifndef INCLUDED_XETEX_H
#define INCLUDED_XETEX_H


#include "libraries.h"


class XeTeX
{
public:
  XeTeX (int dummy);
  ~XeTeX ();
private:
  void initialize_variables ();

  // Work area.
public:
private:
  void create_work_area ();
  ustring working_directory;
  void place_ptx2pdf_macros ();
  void place_glw_sample ();

  // Configuration files.
public:
  void page_size_set(double width_centimeters, double height_centimeters);
  void page_margins_set(double inside_margin_centimeters, double outside_margin_centimeters, double top_margin_centimeters, double bottom_margin_centimeters);
private:
  double page_width_centimeters, page_height_centimeters;
  double inside_page_margin_centimeters, outside_page_margin_centimeters, top_page_margin_centimeters, bottom_page_margin_centimeters;
  vector <ustring> document_tex;
  void write_document_tex_file ();
  
  // Run xetex.
public:
  ustring run ();
private:

};


#endif
