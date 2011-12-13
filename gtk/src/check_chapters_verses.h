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


#ifndef INCLUDED_CHECK_CHAPTERS_VERSES_H
#define INCLUDED_CHECK_CHAPTERS_VERSES_H


#include "libraries.h"
#include "progresswindow.h"


class CheckChaptersVerses
{
public:
  CheckChaptersVerses (const ustring& project, const vector<unsigned int>& books, bool gui);
  ~CheckChaptersVerses ();
  vector<ustring> references;
  vector<ustring> comments;
  bool cancelled;
private:
  ProgressWindow * progresswindow;
  ustring myproject;
  ustring myversification;
  unsigned int highest_chapter;
  unsigned int highest_verse;
  bool first_chapter_found;
  int previous_chapter;
  void highest_chapter_get (unsigned int book);
  void highest_verse_get (unsigned int book, unsigned int chapter);
  void new_chapter_check (unsigned int book, unsigned int chapter);
  void last_chapter_check (unsigned int book, vector<unsigned int>& chapters);
  void verses_check (unsigned int book, unsigned int chapter, const vector<ustring>& verses);
  void message (unsigned int book, unsigned int chapter, const ustring& verse, const ustring& message);
  void store_expanded_verse (const ustring& verse, unsigned int verses_pointer,
                             vector<unsigned int>& expanded_verses, vector<unsigned int>& verses_pointers);
};


#endif
