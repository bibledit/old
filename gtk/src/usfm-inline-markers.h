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


#ifndef INCLUDED_USFM_INLINE_MARKERS_H
#define INCLUDED_USFM_INLINE_MARKERS_H


#include "libraries.h"
#include "usfm.h"


class UsfmInlineMarkers
{
public:
  UsfmInlineMarkers (const Usfm& usfm);
  ~UsfmInlineMarkers ();
  vector<ustring> markers;
  vector<ustring> opening_markers;
  vector<ustring> closing_markers;
  string opening_sword_markup (int index);
  string closing_sword_markup;
  ustring opening_odt_markup (int index);
  ustring closing_odt_markup;
private:
  vector<ustring> italic;
  vector<ustring> bold;
  vector<ustring> underline;
  vector<ustring> smallcaps;
  vector<bool> superscript;
  vector<unsigned int> color;
};


#endif
