/*
** Copyright (©) 2003-2009 Teus Benschop.
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


#ifndef INCLUDED_DIALOGREVERT_H
#define INCLUDED_DIALOGREVERT_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "gui_display_changes.h"


class RevertDialog
{
public:
  RevertDialog (Reference * reference);
  ~RevertDialog ();
  int run ();
protected:
  GtkWidget *revertdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *comboboxbook;
  GtkWidget *comboboxchapter;
  GtkWidget *labelbook;
  GtkWidget *labelchapter;
  GtkWidget *hseparator1;
  GtkWidget *labelrevisions;
  GtkWidget *scrolledwindowrevisions;
  GtkWidget *treeviewrevisions;
  GtkWidget *vseparator1;
  GtkWidget *vbox2;
  GtkWidget *radiobutton_current;
  GtkWidget *radiobutton_previous;
  GtkWidget *radiobutton_changes;
  DisplayChangesGui * changes_gui;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_comboboxbook_changed (GtkComboBox *combobox, gpointer user_data);
  void on_book_changed ();
  static void on_comboboxchapter_changed (GtkComboBox *combobox, gpointer user_data);
  void on_chapter_changed ();
  static void on_treeviewrevisions_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_revision_activated ();
  static void on_radiobutton_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton (GtkToggleButton *togglebutton);
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  ustring project;
  ustring language;
  int reference_chapter;
  unsigned int book_get ();
  unsigned int chapter_get ();
  GtkListStore *store;
  GtkTreeViewColumn *column;
  GtkTreeSelection *selection;
  vector <unsigned int> seconds; // Stores the seconds of the dates now loaded in the dialog.
  bool revisionloaded;
  unsigned int branch;
  vector <ustring> history_data;
};


#endif
