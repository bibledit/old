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


#ifndef INCLUDED_REFERENCEUTILS_H
#define INCLUDED_REFERENCEUTILS_H


#include "libraries.h"
#include "reference.h"


vector<ustring> references_hidden_ones_load (const ustring& project);
void references_hidden_ones_save (const ustring& project, vector <ustring>& references);
bool text_contains_reference (const ustring& text);
bool text_starts_chapter_verse (const ustring& text);


class ReferencesScanner
{
public:
  ReferencesScanner (const ustring& language, int book, const ustring& text);
  vector <Reference> references;
private:
  // Available book names.
  size_t maximum_book_name_length;
  vector <ustring> all_book_names;
  map <ustring, unsigned int> names_or_abbreviations_to_books;

  // Scanner and data collection.
  ustring current_ustring_token;
  gunichar current_unichar_token;
  ustring preceding_text;
  ustring chapter_verse_text;
  bool in_chapter_verse_state;

  // Interpreter.
  unsigned int current_book;
  void interprete ();
  unsigned int interprete_book ();
  void interprete_chapters_verses ();
  void interprete_single_text (const ustring& text);
  void interprete_verses (const ustring& chapter, const ustring& verse);

};


#endif
