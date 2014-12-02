/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef INCLUDED_FILTER_PASSAGE_H
#define INCLUDED_FILTER_PASSAGE_H


#include <config/libraries.h>


class Passage
{
public:
  Passage ();
  Passage (string bible_in, int book_in, int chapter_in, string verse_in);
  string bible;
  int book;
  int chapter;
  string verse;
  bool equal (Passage & passage);
};


string filter_passage_display (int book, int chapter, string verse);


#endif
