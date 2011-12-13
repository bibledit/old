/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


#include "libraries.h"
#include <glib.h>
#include "assistantdeletekeyterms.h"
#include "help.h"
#include "keyterms.h"
#include "combobox.h"


DeleteKeytermsAssistant::DeleteKeytermsAssistant(int dummy) :
AssistantBase("Keyterms", "deletekeyterms")
// Assistant for deleting keyterms.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));

  introduction ("You can delete categories from the keyterms database.");
  
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox1);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox1, "Which collection would you like to delete?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox1, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox1, true);

  combobox1 = gtk_combo_box_new_text ();
  gtk_widget_show (combobox1);
  gtk_box_pack_start (GTK_BOX (vbox1), combobox1, TRUE, TRUE, 0);
  
  vector <ustring> categories = keyterms_get_categories ();
  combobox_set_strings (combobox1, categories);
  combobox_set_index (combobox1, 0);

  label_confirm = gtk_label_new ("Ready for delete");
  gtk_widget_show (label_confirm);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Ready for delete");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("Deleting...");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "Deleting");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Delete done.");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish assistant.
  
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}


DeleteKeytermsAssistant::~DeleteKeytermsAssistant()
{
}


void DeleteKeytermsAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((DeleteKeytermsAssistant *) user_data)->on_assistant_apply();
}


void DeleteKeytermsAssistant::on_assistant_apply ()
{
  ustring category = combobox_get_active_string (combobox1);
  keyterms_delete_collection (category);

  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint DeleteKeytermsAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((DeleteKeytermsAssistant *) user_data)->assistant_forward (current_page);
}


gint DeleteKeytermsAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  return ++current_page;
}


