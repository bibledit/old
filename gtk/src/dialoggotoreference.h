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
  bool go_back;
  bool go_forward;
protected:
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;
  GtkWidget *notebook;
  GtkWidget *entry_free;
  GtkWidget *treeview_book;
  GtkWidget *treeview_chapter;
  GtkWidget *treeview_verse;
  GtkWidget *radiobutton_back;
  GtkWidget *radiobutton_forward;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  unsigned int oldbook;
  unsigned int oldchapter;
  ustring oldverse;
  ustring language;
  vector <ustring> input;
  void on_jump();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void show_bad_reference ();
  static gboolean on_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data);
  void on_control_g ();
  static void on_entry_free_activate (GtkEntry *entry, gpointer user_data);
  void on_activate ();
  GtkListStore *store_book;
  GtkListStore *store_chapter;
  GtkListStore *store_verse;
  static gboolean on_treeview_book_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview_book_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  static gboolean on_treeview_chapter_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview_chapter_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  static gboolean on_treeview_verse_move_cursor (GtkTreeView *treeview, GtkMovementStep step, gint count, gpointer user_data);
  static void on_treeview_verse_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
  void on_move_book ();
  void on_activate_book ();
  void on_move_chapter ();
  void on_activate_chapter ();
  void on_move_verse ();
  void on_activate_verse ();
};


#endif
