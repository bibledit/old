/*
** Copyright (©) 2003 The Free Software Foundation.
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


#ifndef INCLUDED_DIALOG_SELECT_BOOKS_H
#define INCLUDED_DIALOG_SELECT_BOOKS_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "settings.h"


class SelectBooksDialog
{
public:
  SelectBooksDialog (bool showportions, GtkWindow *parent=NULL);
  ~SelectBooksDialog ();
  void language (const ustring& language);
  void selectables (vector <unsigned int>& selectablebooks);
  void selection_set (const set <unsigned int>& selectedbooks);
  int run ();
  vector <unsigned int> selection;
  set <unsigned int> selectionset;
protected:
  GtkWidget *selectbooksdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox11;
  GtkWidget *vbox2;
  GtkWidget *labelbooks;
  GtkWidget *scrolledwindowbooks;
  GtkWidget *treeviewbooks;
  GtkWidget *vbox3;
  GtkWidget *labelportions;
  GtkWidget *scrolledwindowportions;
  GtkWidget *treeviewportions;
  GtkWidget *table1;
  GtkWidget *nobutton;
  GtkWidget *alignment1;
  GtkWidget *hbox4;
  GtkWidget *image1;
  GtkWidget *label6;
  GtkWidget *otbutton;
  GtkWidget *alignment2;
  GtkWidget *hbox5;
  GtkWidget *image2;
  GtkWidget *label7;
  GtkWidget *ntbutton;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label8;
  GtkWidget *otherbutton;
  GtkWidget *alignment6;
  GtkWidget *hbox9;
  GtkWidget *image6;
  GtkWidget *label11;
  GtkWidget *allbutton;
  GtkWidget *alignment4;
  GtkWidget *hbox7;
  GtkWidget *image4;
  GtkWidget *label9;
  GtkWidget *currentbutton;
  GtkWidget *alignment5;
  GtkWidget *hbox8;
  GtkWidget *image5;
  GtkWidget *label10;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static gboolean on_treeviewbooks_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  gboolean on_treeviewbooks_key_press (GdkEventKey *event);
  static void on_treeviewportions_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_treeviewportions_row ();
  static void on_nobutton_clicked (GtkButton *button, gpointer user_data);
  void on_button_no ();
  static void on_otbutton_clicked (GtkButton *button, gpointer user_data);
  void on_button_ot ();
  static void on_ntbutton_clicked (GtkButton *button, gpointer user_data);
  void on_button_nt ();
  static void on_otherbutton_clicked (GtkButton *button, gpointer user_data);
  void on_button_other ();
  static void on_allbutton_clicked (GtkButton *button, gpointer user_data);
  void on_button_all ();
  static void on_currentbutton_clicked (GtkButton *button, gpointer user_data);
  void on_button_current ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void selection_display (set<unsigned int>& books);
private:
  GtkListStore *storebooks;
  GtkTreeViewColumn *columnbooks;
  GtkTreeSelection *selectbooks;
  ustring myproject;
  ustring mylanguage;
  vector<unsigned int> myselectables;
  set<unsigned int> myselection;
  void loadbooks ();
  static void selection_foreach_function (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  bool myshowportions;
  GtkListStore *storeportions;
  GtkTreeViewColumn *columnportions;
  GtkTreeSelection *selectportions;
  void loadportions ();
  guint update_gui_event_id;
  static bool on_update_gui_timeout (gpointer user_data);
  void update_gui ();
  gint treeviewbooks_previous_y;
  gint treeviewportions_previous_y;
};


#endif
