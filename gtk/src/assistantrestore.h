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


#ifndef INCLUDED_ASSISTANT_RESTORE_H
#define INCLUDED_ASSISTANT_RESTORE_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "assistants.h"
#include "backup.h"


class RestoreAssistant : public AssistantBase
{
public:
  RestoreAssistant(int dummy);
  virtual ~RestoreAssistant();
private:
  // Assistant page preparation.
  static void on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data);
  void on_assistant_prepare (GtkWidget *page);

  // Restore type selection.
  int page_number_select_type;
  GtkWidget *vbox_select_type;
  GtkWidget *radiobutton_select_type_bible;
  GtkWidget *radiobutton_select_type_notes;
  GtkWidget *radiobutton_select_type_resource;
  GtkWidget *radiobutton_select_type_everything;
  BackupType get_type ();

  // Bible name.
  int page_number_bible_name;
  GtkWidget *vbox_bible_name;
  GtkWidget *entry_bible_name;
  GtkWidget *label_bible_name;
  static void on_entry_bible_name_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_bible_name ();
  ustring bible_name;

  // Select file.
  int page_number_file;
  GtkWidget *vbox_file;
  GtkWidget *button_file;
  GtkWidget *label_file;
  static void on_button_file_clicked (GtkButton *button, gpointer user_data);
  void on_button_file ();
  ustring filename;
  
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
  vector <ustring> restore_feedback;
};


#endif
