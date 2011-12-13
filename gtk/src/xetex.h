/*
** Copyright (©) 2003-2012 Teus Benschop.
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
#include "types.h"


enum XeTeXScriptingEngineType {xtxsetGeneric, xtxsetArab};


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

  // Configuration files.
public:
private:
  vector <ustring> document_tex;
  void write_document_tex_file ();
  void write_stylesheet ();

	// The USFM data.
public:
  void add_book (unsigned int id, const vector <ustring>& data);
private:
  vector <unsigned int> book_ids;
  vector <VectorUstring> book_data;
  
  // Run xetex.
public:
  ustring run ();
private:

};


#endif
