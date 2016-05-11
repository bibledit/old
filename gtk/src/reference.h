/*
** Copyright (©) 2003-2013 Teus Benschop, 2016 Matt Postiff
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

#ifndef INCLUDED_REFERENCE_H
#define INCLUDED_REFERENCE_H

#include "libraries.h"

class Reference
{
 private:
  unsigned int book;
  unsigned int chapter;
  ustring verse;  // this is not just a verse integer, but could be 16-18, for example
 public:
  Reference ();
  Reference (int dummy); // not sure why this was pre-existent, but I kept it for now
  Reference (unsigned int book_in, unsigned int chapter_in, const ustring& verse_in);

  inline unsigned int book_get() const { return book; }
  inline unsigned int chapter_get() const { return chapter; }
  inline ustring verse_get() const { return verse; }

  inline void book_set(unsigned int b) { book = b; }
  inline void chapter_set(unsigned int c) { chapter = c; }
  inline void verse_set(const ustring &v) { verse = v; }
  inline void verse_append(ustring ap) { verse.append(ap); }

  ustring human_readable (const ustring& language);
  bool equals(const Reference& reference);
  void assign(const Reference& reference);
  void assign(unsigned int book_in, unsigned int chapter_in, const ustring& verse_in);
  void clear(void);
};

#endif
