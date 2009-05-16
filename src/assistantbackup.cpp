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


#include "libraries.h"
#include <glib.h>
#include "assistantbackup.h"
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


BackupAssistant::BackupAssistant(int dummy) :
AssistantBase("Backup", "")
// Backup assistant.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("A backup helps keeping your data safe");

  // Configuration and initialization.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  //ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Build the GUI for the task selector.
  vbox_select_type = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_select_type);
  page_number_select_type = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_select_type);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_select_type), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_select_type, "What would you like to backup?");
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

  radiobutton_select_type_everything = gtk_radio_button_new_with_mnemonic (NULL, "Everything");
  gtk_widget_show (radiobutton_select_type_everything);
  gtk_box_pack_start (GTK_BOX (vbox_select_type), radiobutton_select_type_everything, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_select_type_everything), radiobutton_select_type_group);
  radiobutton_select_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_select_type_everything));

  Shortcuts shortcuts_select_type (0);
  shortcuts_select_type.button (radiobutton_select_type_bible);
  shortcuts_select_type.button (radiobutton_select_type_notes);
  shortcuts_select_type.button (radiobutton_select_type_everything);
  shortcuts_select_type.consider_assistant();
  shortcuts_select_type.process();

  // Confirm or change Bible.
  vbox_bible_name = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox_bible_name);
  page_number_bible_name = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_name);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_bible_name), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_name, "Is this the right Bible?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_name, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, true);

  label_project_name = gtk_label_new ("Bible name");
  gtk_widget_show (label_project_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), label_project_name, FALSE, FALSE, 0);

  button_bible_name = gtk_button_new ();
  gtk_widget_show (button_bible_name);
  gtk_box_pack_start (GTK_BOX (vbox_bible_name), button_bible_name, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_bible_name, "clicked", G_CALLBACK (on_button_bible_name_clicked), gpointer (this));

  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label12;

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_bible_name), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label12 = gtk_label_new_with_mnemonic ("Choose another one");
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox1), label12, FALSE, FALSE, 0);

  Shortcuts shortcuts_bible_name (0);
  shortcuts_bible_name.label (label12);
  shortcuts_bible_name.consider_assistant();
  shortcuts_bible_name.process();

  // Select file where to save to.
  vbox_file = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_file);
  page_number_file = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_file);
  gtk_container_set_border_width (GTK_CONTAINER (vbox_file), 10);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_file, "Where would you like to save it?");
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
  label_confirm = gtk_label_new ("Backup is about to be made");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Backup is about to be made");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Backup was made");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

BackupAssistant::~BackupAssistant()
{
}

void BackupAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((BackupAssistant *) user_data)->on_assistant_prepare(page);
}


void BackupAssistant::on_assistant_prepare (GtkWidget *page)
{
  extern Settings *settings;

  if (page == vbox_bible_name) {
    // Prepare for the page to confirm or change the Bible.
    if (bible_name.empty()) {
      bible_name = settings->genconfig.project_get();
    }
    gtk_label_set_text (GTK_LABEL (label_project_name), bible_name.c_str());
    if (bible_name.empty()) {
      gtk_label_set_text (GTK_LABEL (label_project_name), "No Bible selected");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_name, !bible_name.empty());
  }

  if (page == vbox_file) {
    gtk_label_set_text (GTK_LABEL (label_file), filename.c_str());
    if (filename.empty()) {
      gtk_label_set_text (GTK_LABEL (label_project_name), "(None)");
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_file, !filename.empty());
  }
}


void BackupAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((BackupAssistant *) user_data)->on_assistant_apply();
}


void BackupAssistant::on_assistant_apply ()
{
  // Save all configurations.
  extern Settings *settings;
  settings->save();

  // Make the backup.
  switch (get_type()) {
    case btBible:
    {
      backup_bible (bible_name, filename);
      break;
    }
    case btNotes:
    {
      backup_notes (filename);
      break;
    }
    case btAll:
    {
      backup_all (filename);
      break;
    }
  }

  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint BackupAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((BackupAssistant *) user_data)->assistant_forward (current_page);
}

gint BackupAssistant::assistant_forward (gint current_page)
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


void BackupAssistant::on_button_bible_name_clicked (GtkButton *button, gpointer user_data)
{
  ((BackupAssistant *) user_data)->on_button_bible_name ();
}


void BackupAssistant::on_button_bible_name ()
{
  project_select(bible_name);
  on_assistant_prepare (vbox_bible_name);
}


void BackupAssistant::on_button_file_clicked (GtkButton *button, gpointer user_data)
{
  ((BackupAssistant *) user_data)->on_button_file ();
}


void BackupAssistant::on_button_file ()
{
  ustring file = gtkw_file_chooser_save (assistant, "", filename);
  if (!file.empty()) {
    filename = file;
    compress_ensure_tar_gz_suffix (filename);
    on_assistant_prepare (vbox_file);
  }
}


BackupType BackupAssistant::get_type ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_bible))) {
    return btBible;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_notes))) {
    return btNotes;
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_select_type_everything))) {
    return btAll;
  }
  return btAll;
}



/*

Todo backup.

We need an assistant that asks what sort of restore to make.
On startup, if there's no data directory, it only offers the whole thing restore option.
When importing project notes, it may give a warning if it finds any existing notes. It will say that the new notes 
will be merged with the existing ones.

Differentiate between a backup and an export.
A backup backups everything that belongs to some item, e.g. a whole project or everything.
An export exports part of the data we wish to see, and in a certain format.

A restore is the opposite of a backup.
An import is the opposite of an export.

One possible export is to email the data of a project to an email address.
It requires mutt. It can send a test email, just with text, and the user can then decide whether the mailer works well.

The projects notes are exported on backup.
On restore, the notes are included one by one, and are imported in the general notes database,
except if a verbatim copy of that note is already present in the database.

If there's no .bibledit folder, before anything else is done, it offers to import a backup.
It would only accept a complete backup, in tar.gz format.
That archive should have a file that indicates that this is a complete backup.
It would refuse anything without that flag.

*/
