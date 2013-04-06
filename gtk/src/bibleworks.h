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


#ifndef INCLUDED_BIBLEWORKS_H
#define INCLUDED_BIBLEWORKS_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"
#include "reference.h"


ustring bibleworks_exported_file_get_bookname (const ustring& filename);
bool bibleworks_clipboard_file_okay (const ustring& filename);
unsigned int bibleworks_clipboard_file_line_get_extract_book_id (ustring& line);
void import_check_bibleworks_file (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages);
void import_bibleworks_text_file (const ustring& file, const ustring& bible, vector <ustring>& messages);
void check_bibleworks_source_language (vector <ustring>& filenames, vector <ustring>& messages);
ustring convert_bibleworks_greek (ustring line);
bool bibleworks_define_parsing (ustring parsing, ustring& definition);
bool bibleworks_is_running ();
bool bibleworks_reference_get_decode (ustring response, Reference& reference);


#endif
