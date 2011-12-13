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

#ifndef INCLUDED_OUTLINE_H
#define INCLUDED_OUTLINE_H

#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"

class Outline
{
public:
  Outline(GtkWidget *vbox);
  ~Outline();
  void focus();
  bool focused();
  void goto_reference(const ustring& project, Reference& reference);
  GtkWidget * reference_changed_signal;
  guint newchapter, newverse;
private:
  GtkWidget *scrolledwindow;
public:
  GtkWidget *treeview;
private:
  GtkTreeStore *store;
  ustring myproject;
  unsigned int mybook;
  unsigned int mychapter;
  void load();
  vector<ustring> headers;
  vector<unsigned int> levels;
  vector<unsigned int> chapters_from;
  vector<unsigned int> verses_from;
  vector<unsigned int> chapters_to;
  vector<unsigned int> verses_to;
  unsigned int chapter_last;
  map<unsigned int, unsigned int> verses_last;
  void retrieve_headers(unsigned int chapter);
  void chapters_verses();
  void handle_chapter_zero_headers();
  vector<ustring> paragraphmarkers;
  vector<unsigned int> paragraphlevels;
  ustring versemarker;
  void new_project_handling();
  static gboolean on_view_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_view_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  bool expand_collapse(bool expand);
  void focus(Reference& reference);
  void focus_internal_children(unsigned int reference, GtkTreeIter& parent, GtkTreeSelection * selection, bool& focused);
  void focus_internal_select(unsigned int reference, GtkTreeIter& iter, GtkTreeSelection * selection, bool& focused);
  static void on_view_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_view_row(GtkTreePath *path);
  static gboolean on_view_popup_menu(GtkWidget *widget, gpointer user_data);
  unsigned int reference_number(unsigned int chapter, unsigned int verse);
};

#endif
