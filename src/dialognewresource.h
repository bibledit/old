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


#ifndef INCLUDED_DIALOGNEWRESOURCE_H
#define INCLUDED_DIALOGNEWRESOURCE_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "shortcuts.h"
#include "resource_utils.h"


class NewResourceDialog
{
public:
  NewResourceDialog (const ustring& templatefile);
  ~NewResourceDialog ();
  int run ();
  ustring edited_template_file;
private:
  // Dialog.
  GtkWidget *newresourcedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *table1;
  void build_table_and_type (Shortcuts& shortcuts);
  unsigned int table_attachment_offset;

  // Entry.
  void build_entry (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label, gchar * label_text, GtkWidget *& entry, const ustring& entry_text, GCallback handler);

  // Button.
  void build_button (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label_short, GtkWidget *& button, gchar * button_text, Shortcuts& shortcuts, GCallback handler, GtkWidget *& label_long);

  // Checkbutton and button.
  void build_checkbutton_button (GtkWidget *& image_ok, GtkWidget *& label_ok, 
                                 GtkWidget *& checkbutton, gchar * checkbutton_text, GCallback checkbutton_handler, 
                                 GtkWidget *& button, gchar * button_text, GCallback button_handler, 
                                 Shortcuts& shortcuts, 
                                 GtkWidget *& label);
  
  // Textview.
  void build_textview (GtkWidget *& image_ok, GtkWidget *& label_ok, GtkWidget *& label,
                       GtkWidget *& textview, gchar * text, GCallback handler);
  
  // Dialog action.
  GtkWidget *dialog_action_area1;
  GtkWidget *helpbutton;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;

  // Template and working directory.
  ustring mytemplatefile;
  void create_working_directory (const ustring& templatefile);
  ustring workingdirectory;
  
  // Resource type.
  GtkWidget *image_type_ok;
  GtkWidget *label_type_ok;
  GtkWidget *label_type_short;
  GtkWidget *button_type;
  GtkWidget *label_type_long;
  static void on_type_button_clicked (GtkButton *button, gpointer user_data);
  void on_type_button ();
  bool type_gui ();
  ResourceType resource_type;
  ustring based_upon ();
  
  // Title.
  GtkWidget *image_title_ok;
  GtkWidget *label_title_ok;
  GtkWidget *label_title;
  GtkWidget *entry_title;
  static void on_entry_title_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_title ();
  bool title_gui ();
  ustring title;

  // Home page.
  GtkWidget *image_home_page_ok;
  GtkWidget *label_home_page_ok;
  GtkWidget *label_home_page;
  GtkWidget *entry_home_page;
  static void on_entry_home_page_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_home_page ();
  bool home_page_gui ();
  ustring home_page;

  // URL constructor.
  GtkWidget *image_url_constructor_ok;
  GtkWidget *label_url_constructor_ok;
  GtkWidget *label_url_constructor;
  GtkWidget *entry_url_constructor;
  static void on_entry_url_constructor_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_url_constructor ();
  bool url_constructor_gui ();
  ustring url_constructor;

  // Index file constructor.
  GtkWidget *image_index_constructor_ok;
  GtkWidget *label_index_constructor_ok;
  GtkWidget *label_index_constructor;
  GtkWidget *entry_index_constructor;
  static void on_entry_index_constructor_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_index_constructor ();
  bool index_constructor_gui ();
  ustring index_constructor;

  // Book names.
  GtkWidget *image_books_ok;
  GtkWidget *label_books_ok;
  GtkWidget *label_books_short;
  GtkWidget *button_books;
  GtkWidget *label_books_long;
  static void on_books_button_clicked (GtkButton *button, gpointer user_data);
  void on_books_button ();
  bool books_gui ();
  map <unsigned int, ustring> books;

  // Book test.
  GtkWidget *image_book_test_ok;
  GtkWidget *label_book_test_ok;
  GtkWidget *checkbutton_book_test;
  GtkWidget *button_book_test;
  GtkWidget *label_book_test;
  static void on_checkbutton_book_test_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_book_test ();
  static void on_button_book_test_clicked (GtkButton *button, gpointer user_data);
  void on_button_book_test ();
  bool book_test_gui ();
  bool book_test_ok;

  // Add files.
  GtkWidget *image_add_files_ok;
  GtkWidget *label_add_files_ok;
  GtkWidget *label_add_files_short;
  GtkWidget *button_add_files;
  GtkWidget *label_add_files_long;
  static void on_add_files_button_clicked (GtkButton *button, gpointer user_data);
  void on_add_files_button ();
  bool add_files_gui ();

  // Add directories.
  GtkWidget *image_add_directories_ok;
  GtkWidget *label_add_directories_ok;
  GtkWidget *label_add_directories_short;
  GtkWidget *button_add_directories;
  GtkWidget *label_add_directories_long;
  static void on_add_directories_button_clicked (GtkButton *button, gpointer user_data);
  void on_add_directories_button ();
  bool add_directories_gui ();

  // Anchor names.
  GtkWidget *image_anchors_ok;
  GtkWidget *label_anchors_ok;
  GtkWidget *label_anchors_short;
  GtkWidget *button_anchors;
  GtkWidget *label_anchors_long;
  static void on_anchors_button_clicked (GtkButton *button, gpointer user_data);
  void on_anchors_button ();
  bool anchors_gui ();
  map <unsigned int, ustring> anchors;

  // Anchor test.
  GtkWidget *image_anchor_test_ok;
  GtkWidget *label_anchor_test_ok;
  GtkWidget *checkbutton_anchor_test;
  GtkWidget *button_anchor_test;
  GtkWidget *label_anchor_test;
  static void on_checkbutton_anchor_test_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_anchor_test ();
  static void on_button_anchor_test_clicked (GtkButton *button, gpointer user_data);
  void on_button_anchor_test ();
  bool anchor_test_gui ();
  bool anchor_test_ok;

  // Write anchors.
  GtkWidget *image_write_anchors_ok;
  GtkWidget *label_write_anchors_ok;
  GtkWidget *checkbutton_write_anchors;
  GtkWidget *button_write_anchors;
  GtkWidget *label_write_anchors;
  static void on_checkbutton_write_anchors_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_write_anchors ();
  static void on_button_write_anchors_clicked (GtkButton *button, gpointer user_data);
  void on_button_write_anchors ();
  bool write_anchors_gui ();
  bool write_anchors_ok;

  // General gui.
  void gui ();
  void build_gui_from_resource_type (Shortcuts& shortcuts, const ustring& filename_based_upon);
  
  // Ok button.
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();

};


#endif
