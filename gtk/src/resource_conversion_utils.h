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


#ifndef INCLUDED_RESOURCE_CONVERSION_UTILS_H
#define INCLUDED_RESOURCE_CONVERSION_UTILS_H


#include "libraries.h"
#include <glib.h>
#include "types.h"


const gchar * resource_conversion_type_to_text (ResourceConversionType type);
ResourceConversionType resource_conversion_text_to_type (const ustring& text);
const gchar * resource_conversion_anchor_prefix ();
ustring resource_conversion_create_anchor_chapter_verse (unsigned int chapter, unsigned int verse);
void resource_conversion_insert_anchors (vector <ustring>& lines, 
                                         const ustring& chapter_prefix, unsigned int chapter_pattern, const ustring& chapter_suffix,
                                         const ustring& verse_prefix,   unsigned int verse_pattern,   const ustring& verse_suffix);
void resource_conversion_remove_anchors (vector <ustring>& lines);


#endif
