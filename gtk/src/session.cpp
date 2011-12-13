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
#include "session.h"
#include "constants.h"
#include "sqlite_reader.h"
#include "directories.h"
#include "gwrappers.h"
#include "utilities.h"


Session::Session(int dummy)
// Stores settings valid only during one session. If the program stops, this
// information is lost - and that is what is wanted.
{
  // Initialize variables to a sane state.
  search_case_sensitive = false;
  search_current_book = false;
  search_current_chapter = false;
  search_globbing = false;
  search_start_word_match = false;
  search_end_word_match = false;
  searchresultstype = sstLoad;
  search_page = 0;
  checksorttype = cstSort0;
  check_include_verse_text = false;
  check_output_in_ot_order = false;
  area_type = atRaw;
  area_id = false;
  area_intro = false;
  area_heading = false;
  area_chapter = false;
  area_study = false;
  area_notes = false;
  area_xref = false;
  area_verse = false;
  line_cutter_for_hebrew_text_characters = 80;
  special_character_selection = 0;

  print_dialog_options_expanded = false;
  print_references_in_notes_in_full = false;
  print_crop_marks = false;
  
  bnmu_add_book = false;
  bnmu_add_chapter = false;
  bnmu_add_verse = true;
  bnmu_verse_prefix = ".";
  bnmu_verse_suffix = ".";
  import_raw_text_book = -1;
  import_raw_text_chapter = -1;
  project_notes_show_title = true;
  merge_action = 0;
}


Session::~Session()
{
}
