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


#ifndef INCLUDED_ONLINE_BIBLE_H
#define INCLUDED_ONLINE_BIBLE_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"


void online_bible_check_file (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages);
bool online_bible_file_looks_good (const ustring& filename);
void onlinebible (map <ustring, unsigned int> bookmap);
bool online_bible_parse_reference (ustring line, unsigned int& book, unsigned int& chapter, unsigned int& verse, map <ustring, unsigned int>& bookmap);
vector <ustring> online_bible_file_divide (const ustring& inputfile, map <ustring, unsigned int> bookmap);
bool online_bible_define_parsing (ustring parsing, ustring& definition);
bool online_bible_is_running ();


#endif
