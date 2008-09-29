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


#ifndef INCLUDED_DIALOGVIEWSUBVERSION_H
#define INCLUDED_DIALOGVIEWSUBVERSION_H


#include <gtk/gtk.h>
#include "ustring.h"


class ViewGitDialog
{
public:
  ViewGitDialog (int dummy);
  ~ViewGitDialog ();
  int run ();
protected:
  GtkWidget *viewgitdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *hbox1;
  GtkWidget *buttonadd;
  GtkWidget *buttondelete;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  guint event_source_id;
  GtkListStore *store;
  GtkTreeViewColumn *column_task;
  GtkTreeViewColumn *column_project;
  GtkTreeViewColumn *column_description;
  GtkTreeViewColumn *column_status;
  GtkTreeSelection *select;
  static bool reload_timeout (gpointer data);
  void reload ();
  static void collect_iters (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_buttonadd_clicked (GtkButton *button, gpointer user_data);
  void on_addbutton ();
  static void on_buttondelete_clicked (GtkButton *button, gpointer user_data);
  void on_deletebutton ();
  ustring row_get_id (GtkTreeModel * model, GtkTreeIter * iter);
  ustring make_id (const ustring& task, const ustring& project, const ustring& description, const ustring& status);
};


#endif
