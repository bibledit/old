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


#ifndef INCLUDED_SESSION_HIGHLIGHTS_H
#define INCLUDED_SESSION_HIGHLIGHTS_H


#include "libraries.h"
#include "types.h"


class SessionHighlights
{
public:
  SessionHighlights (const ustring& word_in, 
                     bool casesensitive_in, bool globbing_in,
                     bool matchbegin_in, bool matchend_in,
                     AreaType areatype_in,
                     bool id_in, bool intro_in, bool heading_in, bool chapter_in,
                     bool study_in, bool notes_in, bool xref_in, bool verse_in);
  ~SessionHighlights ();
  ustring word;
  bool casesensitive;
  bool globbing;
  bool matchbegin;
  bool matchend;
  AreaType areatype;
  bool id;
  bool intro;
  bool heading;
  bool chapter;
  bool study;
  bool notes;
  bool xref;
  bool verse;
private:
};


#endif
