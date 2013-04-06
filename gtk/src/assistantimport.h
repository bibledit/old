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


#ifndef INCLUDED_ASSISTANT_IMPORT_H
#define INCLUDED_ASSISTANT_IMPORT_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"
#include "backup.h"
#include "export_utils.h"
#include "windowreferences.h"
#include "windowstyles.h"
#include "windowcheckkeyterms.h"
#include "import.h"
#include "bibleworks.h"
#include "windowsoutpost.h"


class ImportAssistant : public AssistantBase
{
public:
  ImportAssistant(WindowReferences * references_window, WindowStyles * styles_window, WindowCheckKeyterms * check_keyterms_window, WindowsOutpost * windows_outpost);
  virtual ~ImportAssistant();
  bool import_notes;
  bool import_keyterms;
private:
  // Local variables.
  WindowReferences * my_references_window;
  WindowStyles * my_styles_window;
  WindowCheckKeyterms * my_check_keyterms_window;
  WindowsOutpost * my_windows_outpost;

  // Assistant page preparation.
  static void on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data);
  void on_assistant_prepare (GtkWidget *page);

  // Import type selection.
  int page_number_select_type;
  GtkWidget *vbox_select_type;
  GtkWidget *radiobutton_select_type_bible;
  GtkWidget *radiobutton_select_type_references;
  GtkWidget *radiobutton_select_type_stylesheet;
  GtkWidget *radiobutton_select_type_notes;
  GtkWidget *radiobutton_select_type_keyterms;
  ImportType get_type ();

  // Confirm or change Bible.
  int page_number_bible_name;
  GtkWidget *vbox_bible_name;
  GtkWidget *label_project_name;
  GtkWidget *button_bible_name;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label12;
  static void on_button_bible_name_clicked (GtkButton *button, gpointer user_data);
  void on_button_bible_name ();
  ustring bible_name;

  // Bible import type.
  int page_number_bible_type;
  GtkWidget *vbox_bible_type;
  GtkWidget *radiobutton_bible_usfm;
  GtkWidget *radiobutton_bible_bibleworks;
  GtkWidget *radiobutton_bible_online_bible;
  GtkWidget *radiobutton_bible_raw_text;
  ImportBibleType get_bible_type ();

  // Online Bible running?
  int page_number_online_bible_running;
  GtkWidget * label_online_bible_running;
  
  // Online Bible to import.
  int page_number_online_bible_bible;
  GtkWidget * vbox_online_bible_bible;
  GtkWidget * combobox_online_bible_bible;
  static void on_combobox_online_bible_bible_changed (GtkComboBox *combobox, gpointer user_data);
  void on_combobox_online_bible_bible ();
  
  // Select files.
  int page_number_files;
  GtkWidget *vbox_files;
  GtkWidget *button_files;
  GtkWidget *label_files;
  static void on_button_files_clicked (GtkButton *button, gpointer user_data);
  void on_button_files ();
  vector <ustring> files_names;
  vector <unsigned int> files_book_ids;
  vector <ustring> files_messages;
  
  // Confirmation stuff.
  int page_number_confirm;
  GtkWidget *label_confirm;
  GtkWidget *label_progress;
  GtkWidget *label_summary;
  int summary_page_number;
  vector <ustring> summary_messages;
  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);
  set <int> forward_sequence;
};


#endif
