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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "libraries.h"
#include <glib.h>
#include "assistantimportkeyterms.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "gtkwrappers.h"

ImportKeytermsAssistant::ImportKeytermsAssistant(int dummy)
// Assistant for adding keyterms.
{
  signal_button = gtk_button_new ();
  
  assistant = gtk_assistant_new();
  gtk_widget_show (assistant);
  gtk_window_set_modal (GTK_WINDOW (assistant), true);
  gtk_window_set_title (GTK_WINDOW (assistant), "Keyterms");
  
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "cancel", G_CALLBACK (on_assistant_cancel_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "close", G_CALLBACK (on_assistant_close_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "delete_event", G_CALLBACK(on_assistant_delete_event), gpointer(this));

  // Introduction.
  
  label_intro = gtk_label_new ("Text files that contain keyterms can be imported into the keyterms database.");
  gtk_widget_show (label_intro);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_intro);
  gtk_label_set_line_wrap (GTK_LABEL (label_intro), TRUE);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_intro, "Introduction");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_intro, GTK_ASSISTANT_PAGE_INTRO);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_intro, true);

  // File to import.

  vbox_select_file = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_file);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_file);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_file, "Which file would you like to import?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_select_file, GTK_ASSISTANT_PAGE_CONTENT);

  label5 = gtk_label_new ("The file that will be imported should satisfy certain conditions. The online help provides more information.");
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (vbox_select_file), label5, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label5), TRUE);

  button_open =  gtk_file_chooser_button_new ("", GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_show (button_open);
  gtk_box_pack_start (GTK_BOX (vbox_select_file), button_open, FALSE, FALSE, 0);
  
  g_signal_connect ((gpointer) button_open, "selection-changed", G_CALLBACK (on_file_chooser_open_file_activated), gpointer (this));

  // Type to import.

  vbox_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_type);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_type);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_type, "What type of file are you importing?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_type, true);

  GSList *radiobutton_type_standard_group = NULL;

  radiobutton_type_standard = gtk_radio_button_new_with_mnemonic (NULL, "Standard textfile");
  gtk_widget_show (radiobutton_type_standard);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_standard, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_standard), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_standard));

  radiobutton_type_otkey_db = gtk_radio_button_new_with_mnemonic (NULL, "OTKEY.DB");
  gtk_widget_show (radiobutton_type_otkey_db);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_otkey_db, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_otkey_db), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_otkey_db));

  radiobutton_type_ktref_db = gtk_radio_button_new_with_mnemonic (NULL, "KTREF.DB");
  gtk_widget_show (radiobutton_type_ktref_db);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_ktref_db, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_ktref_db), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_ktref_db));

  radiobutton_type_ktbh = gtk_radio_button_new_with_mnemonic (NULL, "Key Terms in Biblical Hebrew Project");
  gtk_widget_show (radiobutton_type_ktbh);
  gtk_box_pack_start (GTK_BOX (vbox_type), radiobutton_type_ktbh, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_type_ktbh), radiobutton_type_standard_group);
  radiobutton_type_standard_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_type_ktbh));

  // Confirmation.
  
  label_confirm = gtk_label_new ("Ready for import");
  gtk_widget_show (label_confirm);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Ready for import");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  
  // Finish assistant.
  
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ImportKeytermsAssistant::~ImportKeytermsAssistant()
{
  gtk_widget_destroy (assistant);
  gtk_widget_destroy (signal_button);
}

void ImportKeytermsAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  cout << "apply" << endl; // Todo
  ((ImportKeytermsAssistant *) user_data)->on_assistant_apply();
}


void ImportKeytermsAssistant::on_assistant_apply ()
{
  gtk_button_clicked (GTK_BUTTON (signal_button));
}


void ImportKeytermsAssistant::on_assistant_cancel_signal (GtkAssistant *assistant, gpointer user_data)
{
  cout << "cancel" << endl; // Todo
  ((ImportKeytermsAssistant *) user_data)->on_assistant_cancel();
}


void ImportKeytermsAssistant::on_assistant_cancel ()
{
  gtk_button_clicked (GTK_BUTTON (signal_button));
}


void ImportKeytermsAssistant::on_assistant_close_signal (GtkAssistant *assistant, gpointer user_data)
{
  cout << "close" << endl; // Todo
  ((ImportKeytermsAssistant *) user_data)->on_assistant_close();
}


void ImportKeytermsAssistant::on_assistant_close ()
{
  gtk_button_clicked (GTK_BUTTON (signal_button));
}


void ImportKeytermsAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ImportKeytermsAssistant *) user_data)->on_assistant_prepare();
}


void ImportKeytermsAssistant::on_assistant_prepare ()
{
}


bool ImportKeytermsAssistant::on_assistant_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  ((ImportKeytermsAssistant *) user_data)->on_assistant_delete();
  return true;
}


void ImportKeytermsAssistant::on_assistant_delete ()
{
  gtk_button_clicked (GTK_BUTTON (signal_button));
}


gint ImportKeytermsAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ImportKeytermsAssistant *) user_data)->assistant_forward (current_page);
}

gint ImportKeytermsAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  return ++current_page;
}


void ImportKeytermsAssistant::on_file_chooser_open_file_activated (GtkFileChooser *chooser, gpointer user_data)
{
  ((ImportKeytermsAssistant *) user_data)->on_file_chooser_open ();
}


void ImportKeytermsAssistant::on_file_chooser_open ()
{
  ustring filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (button_open));
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_select_file, !filename.empty ());
}

