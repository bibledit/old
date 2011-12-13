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

#include "libraries.h"
#include "session_highlights.h"

SessionHighlights::SessionHighlights(const ustring & word_in, bool casesensitive_in, bool globbing_in, bool matchbegin_in, bool matchend_in, AreaType areatype_in, bool id_in, bool intro_in, bool heading_in, bool chapter_in, bool study_in, bool notes_in, bool xref_in, bool verse_in)
// This object stores highlighting information.
{
  word = word_in;
  casesensitive = casesensitive_in;
  globbing = globbing_in;
  matchbegin = matchbegin_in;
  matchend = matchend_in;
  areatype = areatype_in;
  id = id_in;
  intro = intro_in;
  heading = heading_in;
  chapter = chapter_in;
  study = study_in;
  notes = notes_in;
  xref = xref_in;
  verse = verse_in;
}

SessionHighlights::~SessionHighlights()
{
}
