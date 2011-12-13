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


#ifndef INCLUDED_DIALOGLISTVIEW_H
#define INCLUDED_DIALOGLISTVIEW_H


#include <gtk/gtk.h>


class ListviewDialog
{
public:
  ListviewDialog (const ustring& title, vector<ustring>& list, const ustring& focus, bool sortlist, gchar * help);
  ~ListviewDialog ();
  void two_pages (const gchar * tab1, const gchar * tab2, vector<ustring>& list, bool sortlist);
  void tab_switcher (guint accel_key, GdkModifierType accel_mods, const gchar * acceleratortext, const gchar * actiontext1, const gchar * actiontext2);
  int run ();
  ustring focus;
protected:
  GtkWidget *listviewdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *hbox1;
  GtkWidget *label_shortcut;
  GtkWidget *label_action;
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *label_tab_1;
  GtkWidget *scrolledwindow2;
  GtkWidget *treeview2;
  GtkWidget *label_tab_2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  GtkAccelGroup *accel_group;
  static void on_treeview1_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  static void on_treeview2_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeview ();
  static void static_on_okbutton_clicked (GtkButton * button, gpointer user_data);
  void on_okbutton_clicked ();
  GtkListStore *store1;
  GtkTreeViewColumn *column1;
  GtkTreeSelection *select1;
  GtkListStore *store2;
  GtkTreeViewColumn *column2;
  GtkTreeSelection *select2;
  static void on_activate_focus (GtkWindow *window, gpointer user_data);
  void on_focus ();
  const gchar * action_text_1;
  const gchar * action_text_2;
  static void on_notebook1_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
  void on_switch_page ();
  ustring initialfocus;
};


#endif
