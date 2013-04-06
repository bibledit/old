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

#ifndef INCLUDED_CATEGORIZE_H
#define INCLUDED_CATEGORIZE_H

#include "libraries.h"

class CategorizeChapterVerse
{
public:
  CategorizeChapterVerse(const vector<ustring>& lines);
  vector<unsigned int> chapter;
  vector<ustring> verse;
  vector<ustring> line;
private:
  void store(unsigned int chapternumber, const ustring& versenumber, const ustring& text);
};

class CategorizeLine
{
public:
  CategorizeLine(const ustring& data);
  ustring id;
  ustring intro;
  ustring head;
  ustring chap;
  ustring study;
  ustring note;
  ustring ref;
  ustring verse;
  bool is_id_marker(const ustring& marker);
  bool is_intro_marker(const ustring& marker);
  bool is_head_marker(const ustring& marker);
  bool is_chap_marker(const ustring& marker);
  bool is_study_marker(const ustring& marker);
  ustring footnote_opener(bool full);
  ustring footnote_closer();
  ustring endnote_opener(bool full);
  ustring endnote_closer();
  ustring xref_opener(bool full);
  ustring xref_closer();
  bool is_verse_marker(const ustring& marker);
  void remove_verse_number(const ustring& versenumber);
private:
  ustring previous_marker;
  void append_text(ustring& container, const ustring& text);
  void clear_out_any_marker(ustring& line);
};

#endif
