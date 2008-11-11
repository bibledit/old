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

#ifndef INCLUDED_WINDOW_REFERENCES_H
#define INCLUDED_WINDOW_REFERENCES_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "windows.h"

enum WindowReferencesActionType { wratReferenceActivated, wratPopupMenu, wratReferencesSelected };

class WindowReferences : public WindowBase
{
public:
  WindowReferences(GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowReferences();
  void display (vector <Reference>& refs);
  GtkWidget *treeview;
  GtkListStore *liststore;
  GtkTreeViewColumn *treecolumn;
  GtkWidget * general_signal_button;
  WindowReferencesActionType action;
  Reference reference;
  vector <Reference> references;
  int popup_button;
  int popup_event_time;
  void open();
  void save();
  void clear();
  void dismiss();
  void hide();
  void activate();
protected:
  GtkWidget *scrolledwindow;
  GtkTreeSelection *treeselect;
private:

  static gboolean on_treeview_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data);
  static gboolean on_treeview_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data);
  static gboolean on_treeview_popup_menu(GtkWidget *widget, gpointer user_data);
  static gboolean on_treeview_move_cursor(GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview_cursor_changed(GtkTreeView *treeview, gpointer user_data);

  static void references_window_selection_foreach_function(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  static void on_collect_iters(GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  void show_popup_menu(GtkWidget *my_widget, GdkEventButton *event);
  static gboolean on_treeview_references_popup_menu(GtkWidget *widget, gpointer user_data);
  void treeview_references_popup_menu(GtkWidget *widget);
  void treeview_references_display_quick_reference();

};

#endif
