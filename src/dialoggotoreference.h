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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_DIALOGGOTOREFERENCE_H
#define INCLUDED_DIALOGGOTOREFERENCE_H


#include <glib.h>
#include <gtk/gtk.h>
#include "ustring.h"
#include "settings.h"
#include "reference.h"


class GotoReferenceDialog
{
public:
  GotoReferenceDialog (unsigned int currentbook, unsigned int currentchapter, const ustring& currentverse);
  ~GotoReferenceDialog ();
  int run();
  bool newreference;
  Reference reference;
protected:
  GtkWidget *gotoreferencedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label4;
  GtkWidget *notebook1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *entry1;
  GtkWidget *label2;
  GtkWidget *table1;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *scrolledwindow2;
  GtkWidget *treeview2;
  GtkWidget *scrolledwindow3;
  GtkWidget *treeview3;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkAccelGroup *accel_group;
private:
  unsigned int oldbook;
  unsigned int oldchapter;
  ustring oldverse;
  ustring language;
  vector <ustring> input;
  void on_jump();
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void show_bad_reference ();
  static void on_gotoreferencedialog_activate_focus (GtkWindow *window, gpointer user_data);
  void on_control_g ();
  static void on_entry1_activate (GtkEntry *entry, gpointer user_data);
  void on_activate ();
  static bool on_timeout (gpointer data);
  void timeout ();
  bool allow_activate;
  GtkListStore *store1;
  GtkListStore *store2;
  GtkListStore *store3;
  GtkTreeViewColumn *column1;
  GtkTreeViewColumn *column2;
  GtkTreeViewColumn *column3;
  GtkTreeSelection *select1;
  GtkTreeSelection *select2;
  GtkTreeSelection *select3;
  static gboolean on_treeview1_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview1_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  static gboolean on_treeview2_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview2_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  static gboolean on_treeview3_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview3_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_move1 ();
  void on_activate1 ();
  void on_move2 ();
  void on_activate2 ();
  void on_move3 ();
  void on_activate3 ();
};


#endif
