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
  Reference current_reference();
  ustring current_verse_number();
  vector <Reference> quick_references();
  void go_to_new_reference_highlight_set();
  
  void load_dictionaries();
  
  void undo();
  void redo();
  bool can_undo();
  bool can_redo();
  ustring text_get_selection();
  void text_erase_selection();
  void text_insert(ustring text);
  ustring word_double_clicked_text();
  void insert_note(const ustring& marker, const ustring& rawtext, GtkTextIter * iter, bool render);
  ustring get_chapter();
  void insert_table(const ustring& rawtext, GtkTextIter * iter);
  void chapter_load(unsigned int chapter_in, vector <ustring> * lines_in = NULL);
  void chapter_save();
  unsigned int reload_chapter_number();
  
  EditorTextViewType last_focused_type();
  GtkTextBuffer * last_focused_textbuffer();

  ustring project();
  bool editable();
  unsigned int book();
  unsigned int chapter();

  void apply_style(const ustring& marker);
  set <ustring> get_styles_at_cursor();
  void create_or_update_formatting_data();
  void set_font();

  Editor * editor_get();

  GtkWidget * new_verse_signal;
  GtkWidget * new_styles_signal;
  GtkWidget * quick_references_button;
  GtkWidget * word_double_clicked_signal;
  GtkWidget * reload_signal;
  GtkWidget * changed_signal;
protected:
  GtkWidget *vbox;

  Editor * editor;

  static void on_new_verse_signalled(GtkButton *button, gpointer user_data);
  void on_new_verse();
  static void on_new_styles_signalled(GtkButton *button, gpointer user_data);
  void on_new_styles();
  static void on_quick_references_signalled(GtkButton *button, gpointer user_data);
  void on_quick_references();
  static void on_word_double_click_signalled(GtkButton *button, gpointer user_data);
  void on_word_double_click();
  static void on_reload_signalled(GtkButton *button, gpointer user_data);
  void on_reload();
  static void on_changed_signalled(GtkButton *button, gpointer user_data);
  void on_changed();
private:
};

#endif
