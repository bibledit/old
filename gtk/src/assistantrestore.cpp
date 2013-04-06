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


#include "libraries.h"
#include <glib.h>
#include "assistantrestore.h"
#include "help.h"
#include "settings.h"
#include "utilities.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include "git.h"
#include "projectutils.h"
#include "snapshots.h"
#include "gtkwrappers.h"
#include "compress.h"
#include "restore.h"


RestoreAssistant::RestoreAssistant(int dummy) :
AssistantBase("Restore", "restore")
// Restore assistant.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("This assists you with restoring a backup");

  // Configuration and initialization.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();

  // Build the GUI for the task selector.
  vbox_select_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_type);
  page_number_select_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_select_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_type, "What would you like to restore?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_select_type, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_select_type, true);

  GSList *radiobutton_select_type_group = NULL;

  radiobutton_select_type_bible = gtk_radio_button_new_with_mnemonic (NULL, "Bible");
  gtk_widget_show (radiobutton_select_type_bible);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_bible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_bible), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_bible));

  radiobutton_select_type_notes = gtk_radio_button_new_with_mnemonic (NULL, "Notes");
  gtk_widget_show (radiobutton_select_type_notes);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_notes, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_notes), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_notes));

  radiobutton_select_type_resource = gtk_radio_button_new_with_mnemonic (NULL, "Resource");
  gtk_widget_show (radiobutton_select_type_resource);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_resource, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_resource), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_resource));

  radiobutton_select_type_everything = gtk_radio_button_new_with_mnemonic (NULL, "Everything");
  gtk_widget_show (radiobutton_select_type_everything);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_everything, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_everything), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_everything));

  Shortcuts shortcuts_select_type (0);
  shortcuts_select_type.button (radiobutton_select_type_bible);
  shortcuts_select_type.button (radiobutton_select_type_notes);
  shortcuts_select_type.button (radiobutton_select_type_resource);
  shortcuts_select_type.button (radiobutton_select_type_everything);
  shortcuts_select_type.consider_assistant();
  shortcuts_select_type.process();

  // Confirm or change Bible.
  vbox_bible_name = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox_bible_name);
  page_number_bible_name = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_name);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_name), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_name, "What will be the name of the Bible?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_name, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, true);

  entry_bible_name = gtk_entry_new ();
  gtk_widget_show (entry_bible_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), entry_bible_name, FALSE, FALSE, 0);

  label_bible_name = gtk_label_new ("");
  gtk_widget_show (label_bible_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), label_bible_name, FALSE, FALSE, 0);

  g_signal_connect_after ((gpointer) entry_bible_name, "changed", G_CALLBACK (on_entry_bible_name_changed), gpointer (this));

  // Select file.
  vbox_file = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_file);
  page_number_file = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_file);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_file), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_file, "What is the file to restore?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_file, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, false);

  button_file = gtk_button_new ();
  gtk_widget_show (button_file);
  gtk_box_pack_start (GTK_BOX (vbox_file), button_file, FALSE, FALSE, 0);

  GtkWidget *alignment2;
  GtkWidget *hbox2;
  GtkWidget *image2;

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_file), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label_file = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label_file);
  gtk_box_pack_start (GTK_BOX (hbox2), label_file, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_file, "clicked", G_CALLBACK (on_button_file_clicked), gpointer(this));

  // Build the confirmation stuff.
  label_confirm = gtk_label_new ("Restore is about to be done");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Restore is about to be done");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

RestoreAssistant::~RestoreAssistant()
{
}

void RestoreAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((RestoreAssistant *) user_data)->on_assistant_prepare(page);
}


void RestoreAssistant::on_assistant_prepare (GtkWidget *page)
{
  if (page == vbox_bible_name) {
    bool bible_name_is_ok = !bible_name.empty();
    ustring message = "Will restore to Bible " + bible_name;
    vector <ustring> available_bibles = projects_get_all ();
    set <ustring> bible_set (available_bibles.begin (), available_bibles.end());
    if (bible_set.find (bible_name) != bible_set.end ()) {
      bible_name_is_ok = false;
      message = "This Bible already exists";
    }
    if (bible_name.empty()) {
      message = "Please enter a name for the Bible to restore to";
    }
    gtk_label_set_text (GTK_LABEL (label_bible_name), message.c_str());
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, bible_name_is_ok);

  }

  if (page == vbox_file) {
    gtk_label_set_text (GTK_LABEL (label_file), filename.c_str());
    if (filename.empty()) {
      gtk_label_set_text (GTK_LABEL (label_file), "(None)");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, !filename.empty());
  }
  
  if (page == label_summary) {
    ustring summary;
    for (unsigned int i = 0; i < restore_feedback.size(); i++) {
      if (i)
        summary.append ("\n");
      summary.append (restore_feedback[i]);
    }
    gtk_label_set_text (GTK_LABEL (label_summary), summary.c_str());
  }
}


void RestoreAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((RestoreAssistant *) user_data)->on_assistant_apply();
}


void RestoreAssistant::on_assistant_apply ()
{
  // Unpack the tarball.
  ustring unpack_directory = gw_build_filename (directories_get_temp (), "restore");
  unix_rmdir (unpack_directory);
  gw_mkdir_with_parents (unpack_directory);
  if (uncompress (filename, unpack_directory)) {

    // Do specialized operations on the unpacked data.
    switch (get_type()) {
      case btBible:
      {
        restore_project (unpack_directory, bible_name, restore_feedback);
        break;
      }
      case btNotes:
      {
        restore_notes (unpack_directory, restore_feedback);
        break;
      }
      case btResource:
      {
        restore_resource (unpack_directory, restore_feedback);
        break;
      }
      case btAll:
      {
        restore_all_stage_one (unpack_directory, restore_feedback);
        break;
      }
    }
  } else {
    restore_feedback.push_back ("Failed to unpack file " + filename);
  }
 
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint RestoreAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((RestoreAssistant *) user_data)->assistant_forward (current_page);
}

gint RestoreAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  gint new_page_number = current_page + 1;

  if (current_page == page_number_select_type) {
    if (get_type () != btBible) {
      new_page_number = page_number_file;
    }
  }

  // Return the new page.
  return new_page_number;
}


void RestoreAssistant::on_entry_bible_name_changed (GtkEditable *editable, gpointer user_data)
{
  ((RestoreAssistant *) user_data)->on_entry_bible_name ();
}


void RestoreAssistant::on_entry_bible_name ()
{
  bible_name = gtk_entry_get_text (GTK_ENTRY (entry_bible_name));
  on_assistant_prepare (vbox_bible_name);
}


void RestoreAssistant::on_button_file_clicked (GtkButton *button, gpointer user_data)
{
  ((RestoreAssistant *) user_data)->on_button_file ();
}


void RestoreAssistant::on_button_file ()
{
  ustring file = gtkw_file_chooser_open (assistant, "", filename);
  if (!file.empty()) {
    filename = file;
    on_assistant_prepare (vbox_file);
  }
}


BackupType RestoreAssistant::get_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_bible))) {
    return btBible;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_notes))) {
    return btNotes;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_resource))) {
    return btResource;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_everything))) {
    return btAll;
  }
  return btAll;
}


