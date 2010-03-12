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


#ifndef INCLUDED_CHECK_SENTENCE_STRUCTURE_H
#define INCLUDED_CHECK_SENTENCE_STRUCTURE_H


#include "libraries.h"
#include <gtk/gtk.h>


enum SentenceStructureType { sstHeading, sstParagraph, sstNote };


class CheckSentenceStructure
{
public:
  CheckSentenceStructure (const ustring& project, const vector<unsigned int>& books, bool gui);
  ~CheckSentenceStructure ();
  vector <ustring> references;
  vector <ustring> comments;
  bool cancelled;
private:
  unsigned int book;
  unsigned int chapter;
  void classify_styles (const ustring& project);
  void create_blocks (ustring& text);
  void check (SentenceStructureType type, ustring text, vector <ustring>& versenumbers, vector <size_t>& verseoffsets);
  void message (vector <ustring>& versenumbers, vector <size_t>& verseoffsets, GtkTextIter * iter, const ustring& message);
  vector <ustring> messages;
  set <ustring> textdeleters;
  set <ustring> headingstarters;
  set <ustring> paragraphstarters;
  set <ustring> versemarkers;
  set <ustring> markerdeleters;
  set <ustring> notestarters;
};


#endif
