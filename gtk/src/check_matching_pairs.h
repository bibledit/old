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


#ifndef INCLUDED_CHECK_MATCHING_PAIRS_H
#define INCLUDED_CHECK_MATCHING_PAIRS_H


#include "libraries.h"
#include "progresswindow.h"


class MatchingPairOpener
{
public:
  MatchingPairOpener (const ustring& char_in, gunichar unichar_in,
                      unsigned int book_in, int chapter_in, const ustring& verse_in,
                      const ustring& context_in);
  ustring character;
  gunichar unichar;
  unsigned int book;
  int chapter;
  ustring verse;
  ustring context;
private:
};


class CheckMatchingPairs
{
public:
  CheckMatchingPairs (const ustring& project, 
                      const vector<unsigned int>& books, 
                      const ustring& ignore,
                      bool gui);
  ~CheckMatchingPairs ();
  vector<ustring> references;
  vector<ustring> comments;
  bool cancelled;
private:
  ProgressWindow * progresswindow;
  set<gunichar> ignores;
  set<gunichar> gopeners;
  set<gunichar> gclosers;
  unsigned int book;
  int chapter;
  ustring verse;
  vector<MatchingPairOpener> openers;
  ustring get_context (ustring& line, unsigned int offset);
  void check_matched_pairs (ustring& text);
  void check_pairs_clean ();
  void message (unsigned int book, unsigned int chapter, const ustring& verse, const ustring& message);
};


#endif
