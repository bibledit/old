/*
 ** Copyright (©) 2016 Matt Postiff.
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

#ifndef INCLUDED_CHAPTERVIEW_H
#define INCLUDED_CHAPTERVIEW_H

#include "reference.h"
#include "editor_aids.h"

// The various views that we support

enum viewType { vtNone, vtFormatted, vtUSFM, vtExperimental };

// Define the interface that must be implemented for a chapter-based
// text view of the Bible. Right now we have a usfmview and a somewhat
// formatted view called Editor2. That may change or expand in the
// future to include a print layout view or other types of views that
// I can't think of right now. MAP

class ChapterView
{
 private:
  Reference current_reference;
 public: 
  ChapterView();
  virtual ~ChapterView();
  virtual viewType vt_get() = 0;
  // The = 0 syntax tells the compiler that these are pure virtual functions and
  // MUST be overridden in the derived class(es). We cannot instantiate a 
  // ChapterView object by itself.
  virtual void spelling_trigger() = 0;
  virtual unsigned int chapter_num_get() = 0;
  virtual ustring chapter_get_ustring() = 0; // TO DO: rename _get at end
  virtual ustring project_get() = 0;
  virtual ustring word_double_clicked_text_get() = 0;
  virtual ustring current_verse_get() = 0;
  virtual unsigned int book_get() = 0;
  virtual void book_set(unsigned int book) = 0;
  virtual unsigned int reload_chapter_num_get() = 0;
  virtual void create_or_update_formatting_data() = 0;
  virtual void apply_style(const ustring &marker) = 0;
  Reference current_reference_get() { return current_reference; }
  void current_reference_set(const Reference &ref) { current_reference.assign(ref); }
  virtual bool editable_get() = 0;
  virtual void chapter_save() = 0;
  virtual void chapter_load(unsigned int chapter) = 0;
  virtual void cut() = 0;
  virtual void copy() = 0;
  virtual void paste() = 0;
  virtual void font_set() = 0;
  virtual vector<ustring> spelling_get_misspelled() = 0;
  virtual void spelling_approve(const vector <ustring>& words) = 0;
  virtual void insert_table(const ustring & rawtext) = 0;
  virtual void insert_note(const ustring& marker, const ustring& rawtext) = 0;
  virtual void insert_text(const ustring &text) = 0;
  virtual ustring text_get_selection() = 0;
  virtual EditorTextViewType last_focused_type() = 0;
  virtual void redo() = 0;
  virtual void undo() = 0;
  virtual bool can_redo() = 0;
  virtual bool can_undo() = 0;
  virtual bool move_cursor_to_spelling_error(bool next, bool extremity) = 0;
  virtual void load_dictionaries() = 0;
  virtual void go_to_verse(const ustring &versenum, bool focus=false) = 0;
};

#endif
