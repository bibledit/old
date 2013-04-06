/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_DIALOGREPLACE_H
#define INCLUDED_DIALOGREPLACE_H


#include <gtk/gtk.h>
#include "referenceutils.h"


class ReplaceDialog
{
public:
  ReplaceDialog (int dummy);
  ~ ReplaceDialog ();
  int run ();
  vector <Reference> results;
protected:
private:
  GtkWidget *replacedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *checkbuttoncase;
  GtkWidget *checkbuttonbook;
  GtkWidget *checkbuttonchapter;
  GtkWidget *selectbutton;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label9;
  GtkWidget *dialog_action_area1;
  GtkWidget *buttonfind;
  GtkWidget *buttoncancel;
  static void on_checkbuttonbook_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_checkbuttonchapter_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  vector <unsigned int> selectable_books;
  static void replacedialog_on_buttonfind_clicked (GtkButton *button, gpointer user_data);
  void on_buttonfind_clicked ();
  static void on_selectbutton_clicked (GtkButton *button, gpointer user_data);
  void on_selectbutton_clicked2 ();
  static void on_word_entry_changed (GtkEditable * editable, gpointer user_data);
  void set_gui ();  
};


#endif
