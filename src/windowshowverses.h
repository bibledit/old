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

#ifndef INCLUDED_WINDOW_SHOW_VERSES_H
#define INCLUDED_WINDOW_SHOW_VERSES_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "windows.h"

class WindowShowVerses : public WindowBase
{
public:
  WindowShowVerses(GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowShowVerses();
  void go_to(Reference& reference);
protected:
  GtkWidget *vbox;
  GtkWidget *expander1;
  GtkWidget *vbox1;
  GtkWidget *scrolledwindow2;
  GtkWidget *treeview1;
  GtkWidget *hbox1;
  GtkWidget *button_add;
  GtkWidget *button_delete;
  GtkWidget *label1;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
private:
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  void on_button_add ();
  static void on_button_delete_clicked (GtkButton *button, gpointer user_data);
  void on_button_delete ();
  static void on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  void load_tree ();
  GtkListStore *store;
  static void on_treeview1_drag_end (GtkWidget *widget, GdkDragContext *drag_context, gpointer user_data);
  void treeview1_drag_end ();
  void load_verses();
  Reference myreference;
};

#endif
