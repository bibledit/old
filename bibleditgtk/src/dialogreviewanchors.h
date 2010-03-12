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


#ifndef INCLUDED_DIALOGREVIEWANCHORS_H
#define INCLUDED_DIALOGREVIEWANCHORS_H


#include <gtk/gtk.h>


class ReviewAnchorsDialog
{
public:
  ReviewAnchorsDialog (vector <unsigned int> * books, vector<unsigned int> * chapters, vector<unsigned int> * verses, vector <unsigned int> * linenumbers);
  ~ReviewAnchorsDialog ();
  int run ();
protected:
  GtkWidget *reviewanchorsdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow3;
  GtkWidget *treeview_anchors;
  GtkWidget *hbox2;
  GtkWidget *button_spurious_anchors;
  GtkWidget *alignment1;
  GtkWidget *hbox3;
  GtkWidget *image1;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static gboolean on_treeview_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  bool treeview_key_press_handler (GdkEventKey *event);
  static void on_collect_iters (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static void on_button_spurious_anchors_clicked (GtkButton *button, gpointer user_data);
  void on_button_spurious_anchors ();
  static void static_on_okbutton_clicked (GtkButton * button, gpointer user_data);
  void on_okbutton_clicked ();
  GtkListStore *store;
  GtkTreeViewColumn *column0; // Book name
  GtkTreeViewColumn *column1; // Chapter
  GtkTreeViewColumn *column2; // Verse
  GtkTreeViewColumn *column3; // Line number
  GtkTreeSelection *select;
  vector <unsigned int> * mybooks;
  vector <unsigned int> * mychapters;
  vector <unsigned int> * myverses;
  vector <unsigned int> * mylinenumbers;
};


#endif
