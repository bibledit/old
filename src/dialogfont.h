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


#ifndef INCLUDED_DIALOGFONT_H
#define INCLUDED_DIALOGFONT_H


#include <gtk/gtk.h>


class FontDialog
{
public:
  FontDialog (int dummy);
  ~FontDialog ();
  int run ();
protected:
  GtkWidget *fontdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label24;
  GtkWidget *hbox3;
  GtkWidget *vbox5;
  GtkWidget *scrolledwindow2;
  GtkWidget *treeview1;
  GtkWidget *hbox4;
  GtkWidget *label25;
  GtkWidget *combobox1;
  GtkWidget *label26;
  GtkWidget *vseparator1;
  GtkWidget *label27;
  GtkObject *spinbutton1_adj;
  GtkWidget *spinbutton1;
  GtkWidget *label28;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static void on_okbutton_clicked (GtkButton * button, gpointer user_data);
  void on_okbutton ();
  static void on_treeview1_cursor_changed (GtkTreeView *treeview, gpointer user_data);
  void on_treeview ();
  static void on_combobox1_changed (GtkComboBox *combobox,  gpointer user_data);
  static void use_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data);
private:
  GtkListStore *store;
};


#endif
