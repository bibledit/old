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


#ifndef INCLUDED_DIALOGASSIGNBOOKS_H
#define INCLUDED_DIALOGASSIGNBOOKS_H


#include <gtk/gtk.h>
#include "ustring.h"


class AssignBooksDialog
{
public:
  AssignBooksDialog (vector <ustring>& unknown_filenames);
  ~AssignBooksDialog ();
  int run ();
  vector <ustring> assigned_files;
  vector <ustring> assigned_books;
  bool assigned (const ustring& bookname);
protected:
  GtkWidget *assignbooksdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label12;
  GtkWidget *hseparator1;
  GtkWidget *hbox6;
  GtkWidget *label13;
  GtkWidget *label_filename;
  GtkWidget *label15;
  GtkWidget *label_bookname;
  vector <GtkWidget *> bookname_labels;
  GtkWidget *button_view;
  vector <GtkWidget *> view_buttons;
  GtkWidget *alignment1;
  GtkWidget *hbox7;
  GtkWidget *image1;
  GtkWidget *label17;
  GtkWidget *button_assign;
  vector <GtkWidget *> assign_buttons;
  GtkWidget *alignment2;
  GtkWidget *hbox8;
  GtkWidget *image2;
  GtkWidget *label18;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_button_view_clicked (GtkButton *button, gpointer user_data);
  void on_view (GtkButton *button);
  static void on_button_assign_clicked (GtkButton *button, gpointer user_data);
  void on_assign (GtkButton *button);
  static void on_button_ok_clicked (GtkButton *button, gpointer user_data);
  void on_ok ();
  vector <ustring> * myunknown_filenames;
};


#endif
