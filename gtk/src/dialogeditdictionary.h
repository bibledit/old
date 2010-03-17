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


#ifndef INCLUDED_DIALOGE_EDIT_DICTIONARY_H
#define INCLUDED_DIALOGE_EDIT_DICTIONARY_H


#include <gtk/gtk.h>
#include "ustring.h"


class EditDictionaryDialog
{
public:
  EditDictionaryDialog (const ustring& dictionary);
  ~EditDictionaryDialog ();
  int run ();
protected:
  GtkWidget *textviewdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *table1;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *button_import_dict;
  GtkWidget *alignment3;
  GtkWidget *hbox4;
  GtkWidget *image3;
  GtkWidget *label3;
  GtkWidget *button_import_file;
  GtkWidget *alignment2;
  GtkWidget *hbox3;
  GtkWidget *image2;
  GtkWidget *label2;
  GtkWidget *button_sort;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *button_export_dict;
  GtkWidget *alignment4;
  GtkWidget *hbox5;
  GtkWidget *image4;
  GtkWidget *label6;
  GtkWidget *button_export_file;
  GtkWidget *alignment5;
  GtkWidget *hbox6;
  GtkWidget *image5;
  GtkWidget *label7;
  GtkWidget *button_count;
  GtkWidget *alignment6;
  GtkWidget *hbox7;
  GtkWidget *image6;
  GtkWidget *label8;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_button_sort_clicked (GtkButton *button, gpointer user_data);
  void on_button_sort (vector <ustring> * lines_ptr);
  static void on_button_import_dict_clicked (GtkButton *button, gpointer user_data);
  void on_button_import_dict ();
  static void on_button_import_file_clicked (GtkButton *button, gpointer user_data);
  void on_button_import_file ();
  static void on_button_export_dict_clicked (GtkButton *button, gpointer user_data);
  void on_button_export_dict ();
  static void on_button_export_file_clicked (GtkButton *button, gpointer user_data);
  void on_button_export_file ();
  static void on_button_count_clicked (GtkButton *button, gpointer user_data);
  void on_button_count ();
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  ustring mydictionary;
  GtkTextBuffer * textbuffer;
  ustring filename;
  vector <ustring> get_other_editable_dictionaries ();
};


#endif
