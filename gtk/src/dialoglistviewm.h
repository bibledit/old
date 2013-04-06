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


#ifndef INCLUDED_DIALOGLISTVIEWM_H
#define INCLUDED_DIALOGLISTVIEWM_H


#include <gtk/gtk.h>


class ListviewMDialog
{
public:
  ListviewMDialog (const ustring& title, vector<ustring>& list, bool sortlist, gchar * help);
  ~ListviewMDialog ();
  int run ();
  vector <ustring> foci;
protected:
  GtkWidget *dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow;
  GtkWidget *treeview;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static gboolean on_treeview_button_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeview_key_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void on_treeview_cursor();
  static void on_treeview_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data);
  void on_treeview();
  GtkListStore *store;
  GtkTreeViewColumn *column;
  GtkTreeSelection *select;
};


#endif
