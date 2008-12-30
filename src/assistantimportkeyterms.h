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

#ifndef INCLUDED_ASSISTANT_IMPORT_KEYTERMS_H
#define INCLUDED_ASSISTANT_IMPORT_KEYTERMS_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "reference.h"
#include "windows.h"

class ImportKeytermsAssistant
{
public:
  ImportKeytermsAssistant(int dummy);
  ~ImportKeytermsAssistant();
  GtkWidget * signal_button;
protected:
  GtkWidget * assistant;
  
  // Introduction page.
  GtkWidget *label_intro;
  
  // File to import.
  GtkWidget *vbox_select_file;
  GtkWidget *label5;
  GtkWidget *button_open;
  static void on_file_chooser_open_file_activated (GtkFileChooser *chooser, gpointer user_data);
  void on_file_chooser_open ();

  // Type to import.
  GtkWidget *vbox_type;
  GtkWidget *radiobutton_type_standard;
  GtkWidget *radiobutton_type_otkey_db;
  GtkWidget *radiobutton_type_ktref_db;
  GtkWidget *radiobutton_type_ktbh;
  
  // Confirm.
  GtkWidget *label_confirm;
  
  
private:
  static void on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_apply ();
  static void on_assistant_cancel_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_cancel ();
  static void on_assistant_close_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_close ();
  static void on_assistant_prepare_signal (GtkAssistant *assistant, gpointer user_data);
  void on_assistant_prepare ();
  static bool on_assistant_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void on_assistant_delete ();

  static gint assistant_forward_function (gint current_page, gpointer user_data);
  gint assistant_forward (gint current_page);

  
};

#endif
