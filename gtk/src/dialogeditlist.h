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

#ifndef INCLUDED_DIALOGEDITLIST_H
#define INCLUDED_DIALOGEDITLIST_H


#include <gtk/gtk.h>
#include "ustring.h"


class EditListDialog
{
public:
  EditListDialog (vector<ustring> * lines, 
                  const ustring& title, const ustring& info,
                  bool remove, bool add, bool sort, bool import, bool exprt, bool duplicates, bool reorderable,
                  vector<ustring> * addables);
  ~EditListDialog ();
  int run ();
protected:
  GtkWidget *editlistdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox3;
  GtkWidget *label3;
  GtkWidget *label_info;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *table1;
  GtkWidget *buttonremove;
  GtkWidget *button_add;
  GtkWidget *button_sort;
  GtkWidget *alignment1;
  GtkWidget *hbox4;
  GtkWidget *image1;
  GtkWidget *label5;
  GtkWidget *button_import;
  GtkWidget *alignment2;
  GtkWidget *hbox5;
  GtkWidget *image2;
  GtkWidget *label6;
  GtkWidget *button_export;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label7;
  GtkWidget *button_doubles;
  GtkWidget *alignment4;
  GtkWidget *hbox7;
  GtkWidget *image4;
  GtkWidget *label8;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  vector<ustring> * myaddables;
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_removebutton_clicked (GtkButton *button, gpointer user_data);
  void on_remove ();
  static void on_collect_iters (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  void on_add ();
  static void on_button_sort_clicked (GtkButton *button, gpointer user_data);
  void on_sort ();
  static void on_button_import_clicked (GtkButton *button, gpointer user_data);
  void on_import ();
  static void on_button_export_clicked (GtkButton *button, gpointer user_data);
  void on_export ();
  static void on_button_doubles_clicked (GtkButton *button, gpointer user_data);
  void on_duplicates ();
  static void on_treeview1_cursor_changed (GtkTreeView *treeview, gpointer user_data);
  void on_cursor ();
  static gboolean on_treeview1_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static gboolean on_treeview1_select_all (GtkTreeView *treeview, gpointer user_data);
  static gboolean on_treeview1_select_cursor_parent (GtkTreeView *treeview, gpointer user_data);
  static gboolean on_treeview1_select_cursor_row (GtkTreeView *treeview, gboolean start_editing, gpointer user_data);
  static gboolean on_treeview1_toggle_cursor_row (GtkTreeView *treeview, gpointer user_data);
  static gboolean on_treeview1_unselect_all (GtkTreeView *treeview, gpointer user_data);
  vector<ustring> * mylines;
  GtkTreeViewColumn *column;
  GtkTreeSelection *select;
  GtkListStore * liststore;
};


#endif
