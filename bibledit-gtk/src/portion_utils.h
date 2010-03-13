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


#ifndef INCLUDED_PORTION_UTILS_H
#define INCLUDED_PORTION_UTILS_H


#include "libraries.h"


void select_portion_get_values (const ustring& project, unsigned int book, const ustring& portion,
                                vector <unsigned int>& chapters_from, vector <ustring>& verses_from,
                                vector <unsigned int>& chapters_to, vector <ustring>& verses_to);
ustring select_portion_get_label (const ustring& project, unsigned int book,
                                  unsigned int chapter_from, const ustring& verse_from,
                                  unsigned int chapter_to, const ustring& verse_to);
vector <ustring> select_portion_get_portions (const ustring& portion);
ustring select_portion_get_portion (const vector <ustring>& portions);


class WithinReferencesRange
{
public:
  WithinReferencesRange ();
  WithinReferencesRange (int dummy);
  void add_portion (unsigned int book_in, vector<unsigned int> chapters_from_in, const vector<ustring>& verses_from_in, vector<unsigned int> chapters_to_in, const vector <ustring>& verses_to_in);
  void set_book (unsigned int book);
  void set_chapter (unsigned int chapter);
  void set_verse (const ustring& verse);
  bool in_range ();
  void verify_range ();
private:
  vector <unsigned int> books;
  vector <unsigned int> chapters_from;
  vector <ustring>      verses_from;
  vector <unsigned int> chapters_to;
  vector <ustring>      verses_to;
  unsigned int mybook;
  unsigned int mychapter;
  ustring myverse;
  vector <bool> in_range_flags;
};


#endif
