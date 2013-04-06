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


#ifndef INCLUDED_DIALOGNEWSTYLESHEET_H
#define INCLUDED_DIALOGNEWSTYLESHEET_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "types.h"


class NewStylesheetDialog
{
public:
  NewStylesheetDialog (const ustring& worksheet);
  ~NewStylesheetDialog ();
  int run ();
  ustring name;
  StylesheetType stylesheettype;
protected:
  GtkWidget *newstylesheetdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox2;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *entry1;
  GtkWidget *label_info;
  GtkWidget *radiobutton_basic;
  GtkWidget *radiobutton_paragraph;
  GtkWidget *radiobutton_word_note;
  GtkWidget *radiobutton_sbp;
  GtkWidget *radiobutton_full;
  GtkWidget *expander1;
  GtkWidget *label2;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton;
private:
  ustring myworksheet;
  static void on_entry_changed (GtkEditable *editable, gpointer user_data);
  void on_entry ();
  static void on_radiobutton_basic_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_paragraph_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_word_note_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_sbp_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_full_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();


  static void on_treeview1_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeview ();
  GtkListStore *store1;
  GtkTreeViewColumn *column1;
  GtkTreeSelection *select1;
  vector <ustring> markers;

};


#endif
