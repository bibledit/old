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


#ifndef INCLUDED_SWORD_NOTE_H
#define INCLUDED_SWORD_NOTE_H


#include "libraries.h"
#include "usfm.h"
#include "notecaller.h"


class SwordNote
{
public:
  SwordNote (const Usfm& usfm, bool show);
  ~SwordNote ();
  void new_book ();
  void new_chapter ();
  void transform (ustring& line);
private:
  bool myshow;
  NoteNumberingRestartType footnote_numbering_restart;
  NoteNumberingRestartType endnote_numbering_restart;
  NoteNumberingRestartType xref_numbering_restart;
  ustring footnote_opener;
  ustring footnote_closer;
  ustring endnote_opener;
  ustring endnote_closer;
  ustring xref_opener;
  ustring xref_closer;
  vector<ustring> content_marker;
  set<ustring> footnote_markers;
  ustring standardparagraph_marker_open;
  ustring standardparagraph_marker_close;
  ustring extraparagraph_marker_open;
  ustring extraparagraph_marker_close;
  NoteCaller * footnotecaller;
  NoteCaller * endnotecaller;
  NoteCaller * xrefcaller;
  void transform2 (ustring& opener, ustring& closer, NoteCaller * caller, ustring& line);
  ustring transform_main_parts (NoteCaller * caller, const ustring& line);
};


#endif
