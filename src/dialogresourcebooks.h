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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#ifndef INCLUDED_DIALOGRESOURCEBOOKS_H
#define INCLUDED_DIALOGRESOURCEBOOKS_H


#include <gtk/gtk.h>
#include "ustring.h"


class ResourceBooksDialog
{
public:
  ResourceBooksDialog (map <unsigned int, ustring>& books);
  ~ResourceBooksDialog ();
  int run ();
  map <unsigned int, ustring> newbooks;
protected:
  GtkWidget *resourcebooksdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label_info;
  GtkWidget *hseparator1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data);
  void on_cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text);
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
private:
  GtkListStore *model;
};


#endif
