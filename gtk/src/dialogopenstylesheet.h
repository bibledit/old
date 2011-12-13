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

#ifndef INCLUDED_DIALOGOPENSTYLESHEET_H
#define INCLUDED_DIALOGOPENSTYLESHEET_H


#include <gtk/gtk.h>


enum OpenStylesheetDialogType {osdtSwitch, osdtDelete};


class OpenStylesheetDialog
{
public:
  OpenStylesheetDialog (OpenStylesheetDialogType dialogtype, const ustring & currentstylesheet);
  ~OpenStylesheetDialog ();
  int run ();
  ustring stylesheet;
protected:
  GtkWidget *openstyledialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static void on_treeview1_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeview ();
  static void static_on_okbutton_clicked (GtkButton * button, gpointer user_data);
  void on_okbutton_clicked ();
  GtkListStore *store1;
  GtkTreeViewColumn *column1;
  GtkTreeSelection *select1;
};


#endif
