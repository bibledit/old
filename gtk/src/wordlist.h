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


#ifndef INCLUDED_WORDLIST_H
#define INCLUDED_WORDLIST_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"
#include "progresswindow.h"


enum WordlistType {wltGeneral, wltHebrew, wltGreek, wltIndex};


class Wordlist
{
public:
  Wordlist (WordlistType wordlist);
  ~Wordlist ();
  void run (vector<ustring>& allmessages);
private:
  ustring entry_opener, entry_closer;
  ustring list_opener, list_closer;
  bool use_asterisk, first_in_chapter;
  ustring project;
  ustring wordlistname;
  set <ustring> section_markers;
  ProgressWindow * progresswindow;
  unsigned int wordcount;
  set<ustring> words;
  void process_line (ustring& line, set<ustring>& section_entries);
  void insert_list (vector <ustring>& lines, unsigned int startlist, unsigned int endlist);
  void message (const ustring& message);
  vector <ustring> messages;
};


ustring wordlist_get_entry_style (const ustring& project, WordlistType type);


#endif
