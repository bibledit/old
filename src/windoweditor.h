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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_WINDOW_EDITOR_H
#define INCLUDED_WINDOW_EDITOR_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "windows.h"
#include "editor.h"

class WindowEditor : public WindowBase
{
public:
  WindowEditor(const ustring& project_name, GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowEditor();
  void go_to(const Reference& reference);
  void load_dictionaries();
  void undo();
  void redo();
  bool can_undo();
  bool can_redo();
  EditorTextViewType last_focused_type();
  vector <Reference> quick_references();
  Reference current_reference();
  ustring current_verse_number();
  ustring project();
  ustring text_get_selection();
  void text_erase_selection();
  GtkTextBuffer * last_focused_textbuffer();
  void text_insert(ustring text);
  void go_to_new_reference_highlight_set();
  ustring word_double_clicked_text();
  bool editable();
  void insert_note(const ustring& marker, const ustring& rawtext, GtkTextIter * iter, bool render);
  Editor * editor; // Todo move this to protected to fix all outside references to it.

protected:
  GtkWidget *vbox;
private:
};

#endif
