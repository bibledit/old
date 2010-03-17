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


#ifndef INCLUDED_DIALOGDICTIONARY_H
#define INCLUDED_DIALOGDICTIONARY_H


#include <gtk/gtk.h>
#include "ustring.h"


class DictionaryDialog
{
public:
  DictionaryDialog (const ustring& project);
  ~DictionaryDialog ();
  int run ();
protected:
  GtkWidget *dictionarydialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label_info;
  GtkWidget *treeview_dictionaries;
  GtkWidget *hbox_buttons;
  GtkWidget *button_add;
  GtkWidget *button_delete;
  GtkWidget *button_edit;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_treeview_dictionaries_cursor_changed (GtkTreeView *treeview, gpointer user_data);
  void on_treeview_dictionaries_cursor ();
  static void on_button_add_clicked (GtkButton *button, gpointer user_data);
  void on_button_add ();
  static void on_button_delete_clicked (GtkButton *button, gpointer user_data);
  void on_button_delete ();
  static void on_button_edit_clicked (GtkButton *button, gpointer user_data);
  void on_button_edit ();
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
private:
  ustring myproject;
  GtkListStore *model;
};


#endif
