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


#ifndef INCLUDED_GUI_SELECT_BIBLES_H
#define INCLUDED_GUI_SELECT_BIBLES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "shortcuts.h"


class SelectBiblesGui
{
public:
  SelectBiblesGui (GtkWidget * box, Shortcuts& shortcuts);
  ~SelectBiblesGui ();
  void set (const vector <bool>& enabled, const vector <ustring>& bible);
  void get (vector <bool>& enabled, vector <ustring>& bible);
  void focus();
private:
  GtkWidget *vbox;
  GtkWidget *scrolledwindow;
  GtkWidget *treeview;
  GtkWidget *hbox;
  GtkWidget *button_add;
  GtkWidget *button_remove;
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  static void on_button_remove_clicked (GtkButton *button, gpointer user_data);
  void on_button_add ();
  void on_button_remove ();
  GtkListStore *store;
  static void on_enabled_toggled(GtkCellRendererToggle *cell, gchar *path_str, gpointer data);
  void on_rendering_toggle(GtkCellRendererToggle *cell, gchar *path_str);
  void add_bible (bool enabled, const ustring& bible);
  static void on_collect_iters (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  
};


#endif
