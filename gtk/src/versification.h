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


#ifndef INCLUDED_VERSIFICATION_H
#define INCLUDED_VERSIFICATION_H


#include "libraries.h"


void versification_create_book_template (const ustring& versification, unsigned int book, vector<ustring>& booktemplate);
void versification_create_chapter_template (const ustring& versification, unsigned int book, unsigned int chapter, vector<ustring>& chaptertemplate);
vector <unsigned int> versification_get_chapters (const ustring& versification, unsigned int book);
ustring versification_get_last_verse (const ustring& versification, unsigned int book, unsigned int chapter);


#endif
