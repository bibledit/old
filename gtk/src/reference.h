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


#ifndef INCLUDED_REFERENCE_H
#define INCLUDED_REFERENCE_H


#include "libraries.h"


class Reference
{
public:
  Reference (int dummy);
  Reference (unsigned int book_in, unsigned int chapter_in, const ustring& verse_in);
  unsigned int book;
  unsigned int chapter;
  ustring verse;
  ustring human_readable (const ustring& language);
  bool equals (const Reference& reference);
  void assign(const Reference& reference);
  };


#endif
