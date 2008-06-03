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


#ifndef INCLUDED_EDITORS_GUI_H
#define INCLUDED_EDITORS_GUI_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "editor.h"


class EditorsGUI
{
public:
  EditorsGUI (GtkWidget * notebook_vbox);
  ~EditorsGUI ();

  // Widgets.
public:
private:
  GtkWidget *my_notebook_vbox;
  GtkWidget *notebook;

  // Initialization and finalization.
public:
  void jumpstart (const ustring& project);
private:
  void desktop_save ();
  void desktop_load ();

  // Open.
public:
  void open (const ustring& project, int method);
private:

  // Save.
public:
  void save ();
private:

  // Close.
public:
  void close ();
private:
  static void on_button_close_tab_clicked (GtkButton *button, gpointer user_data);
  void on_button_close_tab (GtkButton *button);
  static void on_button_close_editor_clicked (GtkButton *button, gpointer user_data);
  void on_button_close_editor (GtkButton *button);
  void close (Editor * editor);

  // Focus.
public:
  Editor * focused_editor ();
  GtkWidget * focus_button;
  bool has_focus ();
private:
  static void on_focus_signal_clicked (GtkButton *button, gpointer user_data);
  void on_focus_signal (GtkButton *button);

  // Navigation.
public:
  void go_to (const Reference& reference);
  GtkWidget * new_verse_button;
  GtkWidget * new_styles_button;
private:
  static void on_new_verse_signalled (GtkButton *button, gpointer user_data);
  void on_new_verse ();
  static void on_editor_style_changed (GtkButton *button, gpointer user_data);
  void editor_style_changed ();

  // Internal storage and flags.
public:
private:
  vector <Editor *> editors;
  bool pass_signals_on;

  // Editors per page.
public:
  vector <Editor *> visible_editors_get ();
private:
  void projects_pages_get (vector <ustring>& projects, vector <int> * pages);
  void editors_pages_get (vector <Editor *>& editorpointers, vector <int> * pages);
  
  // Quick references.
public:
  void quick_references_textview_set (GtkWidget * widget);
private:
  GtkWidget * quick_references_textview;

  // Word double-click.
public:
  GtkWidget * word_double_clicked_button;
private:
  static void on_word_double_clicked (GtkButton *button, gpointer user_data);
  void word_double_clicked ();

  // Editor reloading.
public:
  GtkWidget * editor_reload_button;
  void reload_chapter (unsigned int book, unsigned int chapter);
private:
  static void on_editor_reload_clicked (GtkButton *button, gpointer user_data);
  void on_editor_reload ();

  // Fonts.
public:
  void set_fonts ();
private:

  // Project switching.
public:
  void next_previous_project (bool next);
private:
  
  // Styles.
public:
  void reload_styles ();
private:
  
  // Title bar.
public:
  void title_bar_display ();
private:
  
  // Editor change.
public:
  GtkWidget * editor_changed_button;
private:
  static void on_editor_changed_clicked (GtkButton *button, gpointer user_data);
  void on_editor_changed ();
  
  // Editor text.
public:
  ustring get_text (const ustring& project);
private:
  
  // Reload dictionaries.
public:
  void reload_dictionaries ();
private:
  
};


#endif
