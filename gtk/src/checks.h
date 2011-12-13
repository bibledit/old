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


#ifndef INCLUDED_CHECKS_H
#define INCLUDED_CHECKS_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"
#include "ot-nt-parallels.h"
#include "windowreferences.h"


void checks_output_references_comments (const vector<ustring>& references, const vector<ustring>& comments);
void checks_display_references_comments (vector<ustring>& references, vector<ustring>& comments, WindowReferences * references_window);


class DisplayCheckingResults
{
public:
  DisplayCheckingResults (const ustring& title);
  void word_inventory (const vector<ustring>& words, const vector<unsigned int>& count,
                       int total, int unique, int filteredtotal, int filteredunique,
                       int excludelimit);
  void usfm_count (const vector<ustring>& usfms, const vector<unsigned int>& count, const ustring& stylesheet);
  void character_count (const vector<ustring>& chars, const vector<unsigned int>& count);
  void references_inventory (const vector <ustring>& verses, const vector <ustring>& references);
  void ot_quotations_in_nt (const vector <ustring> nt_refs, const vector <ustring> nt_texts, const vector <VectorUstring>& ot_refs, const vector <VectorUstring>& ot_texts, const gchar * mainheading);
  void parallel_passages (const vector <OtNtParallelDataSection>& data, const gchar * mainheading);
private:
  vector <ustring> html;
  void start (const ustring& title);
  void heading (unsigned int level, const ustring& text);
  void paragraph (const ustring& text);
  void open_table ();
  void close_table ();
  void open_table_column ();
  void close_table_column ();
  void add_table_cell (const ustring& text, bool bold, bool right_aligned, bool translate_entities = true);
  void finalize ();
  ustring search_whole_words (const ustring& text, const ustring& word);
};


ustring get_non_alphanumericals (const ustring& s);


class CollectCheckingResults
{
public:
  CollectCheckingResults (int dummy);
  void add (const vector<ustring>& refs, const vector<ustring>& cmts);
  vector <ustring> references;
  vector <ustring> comments;
private:
};


#endif
