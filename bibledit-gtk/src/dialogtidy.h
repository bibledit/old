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


#ifndef INCLUDED_DIALOGTIDY_H
#define INCLUDED_DIALOGTIDY_H


#include <gtk/gtk.h>
#include "ustring.h"


class TidyDialog
{
public:
  TidyDialog (int dummy);
  ~TidyDialog ();
  int run ();
protected:
  GtkWidget *tidydialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label11;
  GtkWidget *hseparator1;
  GtkWidget *checkbutton_translate_books;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *checkbutton_normalize_hyphens;
  GtkWidget *checkbutton_space_between_chapter_verse;
  GtkWidget *checkbutton_space_series_verses;
  GtkWidget *checkbutton_full_stop_ends_text;
  GtkWidget *checkbutton_ampersand_semicolon;
  GtkWidget *checkbutton_space_before_punctuation;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_checkbutton_translate_books_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_translate_books ();
  static void cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data);
  void on_cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text);
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void gui ();
private:
  GtkListStore *model;
};


#endif
