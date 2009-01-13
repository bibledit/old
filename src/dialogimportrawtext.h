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

#ifndef INCLUDED_DIALOG_IMPORT_RAW_TEXT_H
#define INCLUDED_DIALOG_IMPORT_RAW_TEXT_H

#include <gtk/gtk.h>
#include "libraries.h"
#include "ustring.h"

class ImportRawTextDialog
{
public:
  ImportRawTextDialog(int dummy);
  ~ImportRawTextDialog();
  int run();
protected:
  GtkWidget *importrawtextdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox_controls;
  GtkWidget *vbox2;
  GtkWidget *label_book;
  GtkWidget *combobox_book;
  GtkWidget *hseparator1;
  GtkWidget *vbox3;
  GtkWidget *label_chapter;
  GtkWidget *combobox_chapter;
  GtkWidget *hseparator2;
  GtkWidget *button_discover_markup;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label4;
  GtkWidget *hseparator3;
  GtkWidget *vbox_filter;
  GtkWidget *label_filter;
  GtkWidget *combobox_filter;
  GtkWidget *button_filter;
  GtkWidget *alignment2;
  GtkWidget *hbox4;
  GtkWidget *image2;
  GtkWidget *label7;
  GtkWidget *hseparator4;
  GtkWidget *checkbutton_okay;
  GtkWidget *checkbutton_verses_at_start;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *hbox3;
  GtkWidget *image_ok;
  GtkWidget *label_ok;
  GtkWidget *label_info;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_textbuffer_changed(GtkEditable * editable, gpointer user_data);
  void on_textbuffer();
  static void on_combobox_book_changed(GtkComboBox *combobox, gpointer user_data);
  void on_combobox_book();
  static void on_combobox_chapter_changed(GtkComboBox *combobox, gpointer user_data);
  void on_combobox_chapter();
  static void on_button_discover_markup_clicked(GtkButton *button, gpointer user_data);
  void on_button_discover_markup();
  static void on_button_filter_clicked(GtkButton *button, gpointer user_data);
  void on_button_filter();
  static void on_checkbutton_okay_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_okbutton1_clicked(GtkButton *button, gpointer user_data);
  void on_okbutton();
  void gui();
  guint gui_event_id;
  static bool gui_timeout(gpointer user_data);
  void gui_execute();

  GtkTextBuffer * textbuffer;
  bool programmatically_setting_text;
  ustring project;
  ustring versification;
  unsigned int book;
  int chapter;
  vector <ustring> get_verses(vector <ustring> * non_line_starters);
  bool text_was_discovered;
};

#endif
