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


#ifndef INCLUDED_DIALOGSELECTCHAPTERS_H
#define INCLUDED_DIALOGSELECTCHAPTERS_H


#include <gtk/gtk.h>
#include "ustring.h"


class SelectChaptersDialog
{
public:
  SelectChaptersDialog (const ustring& project, unsigned int book, const ustring& currentselection);
  ~SelectChaptersDialog ();
  int run ();
  ustring new_selection;
protected:
  GtkWidget *selectchaptersdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *buttonall;
  GtkWidget *alignment5;
  GtkWidget *hbox8;
  GtkWidget *image5;
  GtkWidget *label18;
  GtkWidget *label17;
  GtkWidget *label16;
  GtkWidget *label19;
  GtkWidget *label20;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeviewchapterfrom;
  GtkWidget *scrolledwindow2;
  GtkWidget *treeviewversefrom;
  GtkWidget *scrolledwindow3;
  GtkWidget *treeviewchapterto;
  GtkWidget *scrolledwindow4;
  GtkWidget *treeviewverseto;
  GtkWidget *scrolledwindow5;
  GtkWidget *treeviewportions;
  GtkWidget *button_add;
  GtkWidget *button_delete;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  ustring myproject;
  unsigned int mybook;
  bool driving_listviews;
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void on_fromchapter ();
  void on_fromverse ();
  void on_tochapter ();
  void on_toverse ();
  static void on_buttonall_clicked (GtkButton *button, gpointer user_data);
  void on_all ();
  static void on_treeviewchapterfrom_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeviewchapterfrom_row ();
  static gboolean on_treeviewchapterfrom_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeviewchapterfrom_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  static void on_treeviewversefrom_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeviewversefrom_row ();
  static gboolean on_treeviewversefrom_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeviewversefrom_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  static void on_treeviewchapterto_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeviewchapterto_row ();
  static gboolean on_treeviewchapterto_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeviewchapterto_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  static void on_treeviewverseto_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeviewverseto_row ();
  static gboolean on_treeviewverseto_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  static gboolean on_treeviewverseto_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void portion_set (const ustring& selection);
  ustring portion_get ();
  void verses_from_to (bool called_by_from);
  GtkTreeViewColumn *columnchapterfrom;
  GtkTreeSelection *selectchapterfrom;
  GtkListStore * liststorechapterfrom;
  GtkTreeViewColumn *columnversefrom;
  GtkTreeSelection *selectversefrom;
  GtkListStore * liststoreversefrom;
  GtkTreeViewColumn *columnchapterto;
  GtkTreeSelection *selectchapterto;
  GtkListStore * liststorechapterto;
  GtkTreeViewColumn *columnverseto;
  GtkTreeSelection *selectverseto;
  GtkListStore * liststoreverseto;
  static void on_treeview1_cursor_changed (GtkTreeView *treeview, gpointer user_data);
  void treeview1_cursor_changed ();
  static gboolean on_treeview1_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  void treeview1_move_cursor ();
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  void button_add_clicked ();
  static void on_button_delete_clicked (GtkButton *button, gpointer user_data);
  void button_delete_clicked ();
  GtkTreeViewColumn *columnportions;
  GtkTreeSelection *selectportions;
  GtkListStore * liststoreportions;
  void update_active_portion ();
};


#endif
