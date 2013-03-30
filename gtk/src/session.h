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


#ifndef INCLUDED_SESSION_H
#define INCLUDED_SESSION_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"
#include "session_highlights.h"


class Session
// Stores settings valid only during one session.
  {
public:
  Session(int dummy);
  ~Session();
  // Search / Replace
  ustring searchword;
  ustring replaceword;
  bool search_case_sensitive;
  bool search_current_book;
  bool search_current_chapter;
  bool search_globbing;
  bool search_start_word_match;
  bool search_end_word_match;
  SearchResultsType searchresultstype;
  int search_page;
  // Book selection.
  set<unsigned int> selected_books;
  // References highlighting.
  vector<SessionHighlights> highlights;
  // Import / export.
  vector <ustring> import_references_searchwords;
  // Entry completion.
  vector<ustring> completion_search;
  vector<ustring> completion_replace;
  vector<ustring> completion_goto;
  // Checks.
  CheckSortType checksorttype;
  bool check_include_verse_text;
  bool check_include_second_project;
  bool check_output_in_ot_order;
  // Area selection.
  AreaType area_type;
  bool area_id;
  bool area_intro;
  bool area_heading;
  bool area_chapter;
  bool area_study;
  bool area_notes;
  bool area_xref;
  bool area_verse;
  // Printing
  vector<ustring> additional_printing_projects;
  bool print_dialog_options_expanded;
  bool print_references_in_notes_in_full;
  bool print_crop_marks;
  // Tools
  int line_cutter_for_hebrew_text_characters;
  // Insert special character.
  int special_character_selection;
  // Bible notes mass update.
  bool bnmu_add_book;
  bool bnmu_add_chapter;
  bool bnmu_add_verse;
  ustring bnmu_verse_prefix;
  ustring bnmu_verse_suffix;
  // Import raw text.
  int import_raw_text_book;
  int import_raw_text_chapter;
  // Floating windows.
  vector <gpointer> open_floating_windows;
  // Project notes.
  bool project_notes_show_title;
  // Styles.
  ustring selected_style;
  // Merge.
  unsigned int merge_action;
  // Question dialogs.
  map <int, bool> yes_no_always_dialog_enabled;
  map <int, int> yes_no_always_dialog_response;
private:
};


#endif
