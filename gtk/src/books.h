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


#ifndef INCLUDED_BOOKS_H
#define INCLUDED_BOOKS_H


#include "libraries.h"
#include "bookdata.h"

void books_init(void);

void books_order (const ustring& project, vector<unsigned int>& books);
void books_standard_order (vector<unsigned int>& books);

unsigned int books_name_to_id (const ustring& language, const ustring& book);
unsigned int books_abbreviation_to_id (const ustring& language, const ustring& abbreviation);
unsigned int books_abbreviation_to_id_loose (const ustring& language, const ustring& abbreviation);
unsigned int books_paratext_to_id (const ustring& paratext);
unsigned int books_bibleworks_to_id (const ustring& bibleworks);
unsigned int books_osis_to_id (const ustring& osis);
unsigned int books_english_to_id (const ustring& english);
unsigned int books_online_bible_to_id (const ustring& onlinebible);
ustring books_id_to_name (const ustring& language, unsigned int id);
ustring books_id_to_abbreviation (const ustring& language, unsigned int id);
ustring books_id_to_paratext (unsigned int id);
ustring books_id_to_bibleworks (unsigned int id);
ustring books_id_to_osis (unsigned int id);
ustring books_id_to_english (unsigned int id);
ustring books_id_to_online_bible (unsigned int id);

BookType books_id_to_type (unsigned int id);
vector <unsigned int> books_type_to_ids (BookType type);

bool books_id_to_one_chapter (unsigned int id);


#endif
