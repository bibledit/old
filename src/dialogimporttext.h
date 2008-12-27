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


#ifndef INCLUDED_DIALOGIMPORT_TEXT_H
#define INCLUDED_DIALOGIMPORT_TEXT_H


#include <gtk/gtk.h>
#include "usfm.h"
#include "types.h"


class ImportTextDialog
{
public:
  ImportTextDialog (int dummy);
  ~ImportTextDialog ();
  int run ();
protected:
  GtkWidget *importsfmdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *image_directory;
  GtkWidget *label_directory_gui;
  GtkWidget *label24;
  GtkWidget *button_directory;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label25;
  
  // Compressed file.  
  GtkWidget *label_compressed;
  GtkWidget *button_compressed;
  GtkWidget *alignment9;
  GtkWidget *hbox14;
  GtkWidget *image13;
  GtkWidget *label43;
  
  GtkWidget *label_unicode;
  GtkWidget *image_unicode;
  GtkWidget *label_unicode_gui;
  GtkWidget *label29;
  GtkWidget *button_unicode;
  GtkWidget *alignment5;
  GtkWidget *hbox10;
  GtkWidget *image8;
  GtkWidget *label30;
  GtkWidget *label_books;
  GtkWidget *image_books;
  GtkWidget *label_books_gui;
  GtkWidget *label27;
  GtkWidget *button_books;
  GtkWidget *alignment4;
  GtkWidget *hbox8;
  GtkWidget *image6;
  GtkWidget *label28;
  GtkWidget *label_directory;
  GtkWidget *label_import;
  GtkWidget *image_assign_ok;
  GtkWidget *label_assign_ok;
  GtkWidget *label_assign_question;
  GtkWidget *button_assign;
  GtkWidget *alignment6;
  GtkWidget *hbox11;
  GtkWidget *image10;
  GtkWidget *label38;
  GtkWidget *label_assign_info;
  GtkWidget *expander1;
  GtkWidget *vbox2;
  GtkWidget *checkbutton_overwrite;
  GtkWidget *hbox_book_names;
  GtkWidget *checkbutton_book_names;
  GtkWidget *button_book_names;
  GtkWidget *alignment10;
  GtkWidget *hbox16;
  GtkWidget *image14;
  GtkWidget *label44;
  GtkWidget *label42;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  ustring directory;
  map <ustring, ustring> filename_bookname_map;
  vector <ustring> unknown_filenames;
  vector <unsigned int> selectable_booknames;
  vector <ustring> selectable_filenames;
  bool select_all_books;
  set <unsigned int> selected_books;
  vector <ustring> filenames_not_in_unicode;
  ustring encoding_convert_from;
  bool assigning_done;
  ustring pick_newest_filename (const vector <ustring> &filenames);
  static void on_button_directory_clicked (GtkButton *button, gpointer user_data);
  void on_button_directory (ustring dir);
  ImportType importtype;
  static void on_button_compressed_clicked (GtkButton *button, gpointer user_data);
  void on_button_compressed();
  static void on_button_assign_clicked (GtkButton *button, gpointer user_data);
  void on_button_assign ();
  static void on_button_books_clicked (GtkButton *button, gpointer user_data);
  void on_button_books ();
  static void on_button_unicode_clicked (GtkButton *button, gpointer user_data);
  void on_button_unicode ();
  static void on_checkbutton_overwrite_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void set_gui ();
  ustring uncompress_directory();
  map <unsigned int, ustring> unusual_book_names;
  static void on_checkbutton_book_names_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_book_names();
  static void on_button_book_names_clicked (GtkButton *button, gpointer user_data);
  void on_button_book_names();
};


#endif
