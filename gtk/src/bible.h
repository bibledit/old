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


#ifndef INCLUDED_BIBLE_H
#define INCLUDED_BIBLE_H


#include "libraries.h"
#include "reference.h"


void sort_references (vector <Reference> &references);
void decode_reference (const ustring & reference, ustring & book,
                       ustring & chapter, ustring & verse);
bool reference_discover(const Reference &oldRef, // inRef was unsigned int oldbook, unsigned int oldchapter, const ustring & oldverse
			const ustring &reference,
			Reference &newRef, // outRef was unsigned int &newbook, unsigned int &newchapter, ustring & newverse, 
			bool consult_memory = false);
#if 0
bool reference_discover (unsigned int oldbook, unsigned int oldchapter, 
                         const ustring& oldverse, const ustring& reference,
                         unsigned int& newbook, unsigned int& newchapter, ustring& newverse,
                         bool consult_memory = false);
#endif
unsigned int book_find_valid (const ustring & rawbook);
unsigned int reference_to_numerical_equivalent (const ustring& book, const ustring& chapter, const ustring& verse);
unsigned int reference_to_numerical_equivalent (const Reference& reference);
ustring book_chapter_verse_to_reference (const ustring& book, int chapter, const ustring& verse);
bool looks_like_verse (const ustring& text);
vector<int> verses_encode (const ustring& verse);
bool chapter_span_discover (const ustring& reference, ustring& chapter1, ustring& verse1, ustring& chapter2, ustring& verse2);
vector <unsigned int> verse_range_sequence (const ustring& verse);
vector <ustring> verse_range_sequence_v2 (ustring verse);


#endif
