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

#ifndef INCLUDED_ASSISTANT_RESOURCE_H
#define INCLUDED_ASSISTANT_RESOURCE_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"
#include "webkitbrowser.h"


class ResourceAssistant : public AssistantBase
{
public:
  ResourceAssistant(bool new_resource);
  virtual ~ResourceAssistant();
private:
  // Assistant page preparation.
  static void on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data);
  void on_assistant_prepare (GtkWidget *page);

  // General variables.
  bool is_new_resource;

  // Working data.
  ustring working_directory ();
  ustring working_configuration_file ();
    
  // Title.
  GtkWidget *vbox_title;
  GtkWidget *entry_title;
  GtkWidget *label_title;
  static void on_entry_title_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_title ();
  ustring title ();

  // Files and folders.
  GtkWidget *table_files;
  GtkWidget *button_add_files;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label6;
  GtkWidget *button_add_folders;
  GtkWidget *alignment2;
  GtkWidget *hbox3;
  GtkWidget *image2;
  GtkWidget *label7;
  GtkWidget *label_files;
  GtkWidget *label_folders;
  static void on_button_add_files_clicked (GtkButton *button, gpointer user_data);
  void on_button_add_files (bool gui_only);
  static void on_button_add_folders_clicked (GtkButton *button, gpointer user_data);
  void on_button_add_folders (bool gui_only);

  // Home page.
  GtkWidget *vbox_home_page;
  GtkWidget *entry_home_page;
  GtkWidget *vbox_home_page_resource;
  WebkitBrowser *browser_home_page;
  static void on_entry_home_page_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_home_page ();
  ustring home_page ();

  // Book set.
  GtkWidget *vbox_bookset;
  GtkWidget *label_bookset_info;
  GtkWidget *hbox_bookset;
  GtkWidget *button_bookset;
  GtkWidget *label_bookset_count;
  static void on_button_bookset_clicked (GtkButton *button, gpointer user_data);
  void on_button_bookset (bool update_gui_only);
  map <unsigned int, ustring> books;
  
  // URL.
  GtkWidget *vbox_url;
  GtkWidget *label_url;
  GtkWidget *entry_url;
  ustring url_construction ();

  // Tester.
  GtkWidget *vbox_tester;
  GtkWidget *hbox_tester;
  GtkWidget *combobox_test_book;
  GtkWidget *combobox_test_chapter;
  GtkWidget *combobox_test_verse;
  WebkitBrowser *browser_tester;
  static void on_combobox_test_changed (GtkComboBox *combobox, gpointer user_data);
  void on_combobox_test ();  

  // Confirmation stuff.
  int page_number_confirm;
  GtkWidget *label_confirm;
  GtkWidget *label_progress;
  GtkWidget *label_summary;
  unsigned int summary_page_number;
  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);
};

#endif
