/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_CHECK_SENTENCE_STRUCTURE_H
#define INCLUDED_CHECK_SENTENCE_STRUCTURE_H


#include "libraries.h"
#include <gtk/gtk.h>


class CheckSentenceStructure
{
public:
  CheckSentenceStructure (const ustring& project, 
                       const vector<unsigned int>& books, 
                       bool gui);
  ~CheckSentenceStructure ();
  vector<ustring> references;
  vector<ustring> comments;
  bool cancelled;
private:
  unsigned int book;
  unsigned int chapter;
  void classify_styles (const ustring& project);
  void check (ustring text);
  void message (const ustring& verse, const ustring& message);
};


class SentenceStructureBlock
{
public:
  SentenceStructureBlock (int dummy);
  ~SentenceStructureBlock ();
  GtkTextBuffer * textbuffer;
  vector <unsigned int> offsets;
  vector <ustring> verses;
private:
};


#endif
