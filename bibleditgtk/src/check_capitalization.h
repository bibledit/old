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


#ifndef INCLUDED_CHECK_CAPITALIZATION_H
#define INCLUDED_CHECK_CAPITALIZATION_H


#include "libraries.h"
#include "progresswindow.h"


class CheckCapitalization
{
public:
  CheckCapitalization (const ustring& project, 
                       const vector<unsigned int>& books, 
                       const ustring& punctuation_followed_by_capitals,
                       const ustring& ignore_lowercase_following,
                       const ustring& abbreviations_filename,
                       bool any_prefixes,
                       const ustring& uncapitalized_prefixes_filename,
                       const ustring& capitalized_suffixes_filename,
                       bool gui);
  ~CheckCapitalization ();
  vector<ustring> references;
  vector<ustring> comments;
  bool cancelled;
private:
  ProgressWindow * progresswindow;
  unsigned int book;
  unsigned int chapter;
  ustring verse;
  vector<int> verse_chapter;
  vector<ustring> verse_verse;
  vector<size_t> verse_pointer;
  ustring verse_text;
  set<gunichar> punctuation_followed_by_capitals_set;
  set<gunichar> ignore_lower_case_following_set;
  set<ustring> abbreviations;
  bool allow_any_uncapitalized_prefixes;
set<ustring> uncapitalized_prefixes;
  set<ustring> capitalized_suffixes;
  void check_capitalization (vector<int>& chapters, vector<ustring>& verses,
                             ustring& text, vector<size_t>& pointers,
                             bool end_check);
  void message (unsigned int book, unsigned int chapter, const ustring& verse, const ustring& message);
  bool is_reference (GtkTextIter iter);
  void get_chapter_and_verse (vector<int>& chapters, vector<ustring>& verses, 
                              vector<size_t>& pointers, GtkTextIter iter,
                              int& chapter, ustring& verse);
  ustring get_context (GtkTextIter iter);
  void mixed_capitalization_message (ustring& word);
  void check_suspicious_capitalization (ustring& text);
};


#endif
