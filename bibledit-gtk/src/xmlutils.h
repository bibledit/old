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

#ifndef INCLUDED_XMLUTILS_H
#define INCLUDED_XMLUTILS_H


#include "libraries.h"
#include <glib.h>


void xml_sort_positions (vector <size_t>& positions, vector <size_t>& lengths);
void xml_combine_overlaps (vector <size_t>& positions, vector <size_t>& lengths);
void xml_html_insert_emphasis (ustring& line, vector <size_t>& positions, vector <size_t>& lengths);
void xml_handle_entities (ustring& line, vector <size_t> * positions);
void xml_positions_push_up (size_t position, size_t up, vector <size_t>& positions);
ustring xml_text_embed_in_tags (int level, const ustring tag, const ustring& text);
ustring xml_tag (int level, const ustring tag, bool closing);


#endif
