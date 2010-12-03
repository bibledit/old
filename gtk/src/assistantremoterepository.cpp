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
#include "assistantremoterepository.h"
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
#include "progresswindow.h"
#include "notes_utils.h"


RemoteRepositoryAssistant::RemoteRepositoryAssistant(int dummy) :
AssistantBase("Remote repository setup", "menu-preferences/dialog-remote-repository")
// Assistant for managing the remote repository.
{
  // Configuration and initialization.
  extern Settings *settings;
  bible = settings->genconfig.project_get();
  event_id_entry_repository = 0;
  persistent_clone_directory = git_testing_directory ("clone");
  write_access_granted = false;
  ignore_entry_repository_changed = false;
    
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("Remote repository management for Bible " + bible + " or the project notes");

  // Build the GUI for the Bible or notes selector.
  vbox_bible_notes_selector = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_bible_notes_selector);
  page_number_bible_notes_selector = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_bible_notes_selector);

  GSList *radiobutton_bible_notes_selector_group = NULL;

  radiobutton_bible_notes_selector_bible = gtk_radio_button_new_with_mnemonic (NULL, "B_ible repository");
  gtk_widget_show (radiobutton_bible_notes_selector_bible);
  gtk_box_pack_start (GTK_BOX (vbox_bible_notes_selector), radiobutton_bible_notes_selector_bible, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_notes_selector_bible), radiobutton_bible_notes_selector_group);
  radiobutton_bible_notes_selector_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_notes_selector_bible));

  radiobutton_bible_notes_selector_notes = gtk_radio_button_new_with_mnemonic (NULL, "Project _notes repository");
  gtk_widget_show (radiobutton_bible_notes_selector_notes);
  gtk_box_pack_start (GTK_BOX (vbox_bible_notes_selector), radiobutton_bible_notes_selector_notes, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bible_notes_selector_notes), radiobutton_bible_notes_selector_group);
  radiobutton_bible_notes_selector_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bible_notes_selector_notes));

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_bible_notes_selector, "What type of repository would you like to set?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_bible_notes_selector, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_bible_notes_selector, true);
  
  // Build the GUI for the setting whether to use a remote repository.
  checkbutton_use_repository = gtk_check_button_new_with_mnemonic ("_Use remote repository");
  gtk_widget_show (checkbutton_use_repository);
  page_number_use_repository = gtk_assistant_append_page (GTK_ASSISTANT (assistant), checkbutton_use_repository);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), checkbutton_use_repository, "Would you like to use a remote repository?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), checkbutton_use_repository, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), checkbutton_use_repository, true);

  // System for trying out git.
  git_tried_and_okay = false;
  
  label_try_git = gtk_label_new ("The content tracker has been tested and works fine");
  gtk_widget_show (label_try_git);
  page_number_try_git = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_try_git);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_try_git, "Trying out the content tracker");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_try_git, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_try_git, false);

  // Build the GUI for the task selector.
  vbox_task_selector = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_task_selector);
  page_number_task_selector = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_task_selector);

  GSList *radiobutton_task_selector_url_group = NULL;

  radiobutton_task_selector_url = gtk_radio_button_new_with_mnemonic (NULL, "To set or change which the _remote repository to use");
  gtk_widget_show (radiobutton_task_selector_url);
  gtk_box_pack_start (GTK_BOX (vbox_task_selector), radiobutton_task_selector_url, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_task_selector_url), radiobutton_task_selector_url_group);
  radiobutton_task_selector_url_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_task_selector_url));

  radiobutton_task_selector_settings = gtk_radio_button_new_with_mnemonic (NULL, "To set or change other se_ttings");
  gtk_widget_show (radiobutton_task_selector_settings);
  gtk_box_pack_start (GTK_BOX (vbox_task_selector), radiobutton_task_selector_settings, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_task_selector_settings), radiobutton_task_selector_url_group);
  radiobutton_task_selector_url_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_task_selector_settings));

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_task_selector, "What would you like to do?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_task_selector, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_task_selector, true);
  
  // Build GUI for the repository URL.
  vbox_repository = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_repository);
  page_number_repository = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_repository);

  hbox_repository = gtk_hbox_new (FALSE, 3);
  gtk_widget_show (hbox_repository);
  gtk_box_pack_start (GTK_BOX (vbox_repository), hbox_repository, TRUE, TRUE, 0);

  label_repository = gtk_label_new_with_mnemonic ("_Repository");
  gtk_widget_show (label_repository);
  gtk_box_pack_start (GTK_BOX (hbox_repository), label_repository, FALSE, FALSE, 0);

  entry_repository = gtk_entry_new ();
  gtk_widget_show (entry_repository);
  gtk_box_pack_start (GTK_BOX (hbox_repository), entry_repository, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry_repository), 9679);

  label_repository_accessible = gtk_label_new ("");
  gtk_widget_show (label_repository_accessible);
  gtk_box_pack_start (GTK_BOX (vbox_repository), label_repository_accessible, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_repository_accessible), 0, 0.5);
  
  gtk_label_set_mnemonic_widget (GTK_LABEL (label_repository), entry_repository);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_repository, "Enter the location of the remote repository");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_repository, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_repository, false);

  g_signal_connect ((gpointer) entry_repository, "changed", G_CALLBACK (on_entry_repository_changed), gpointer (this));

  // GUI for cloning the repository.
  vbox_clone = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_clone);
  page_number_clone = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_clone);

  label_clone = gtk_label_new ("");
  gtk_widget_show (label_clone);
  gtk_box_pack_start (GTK_BOX (vbox_clone), label_clone, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_clone), 0, 0.5);

  button_clone = gtk_button_new ();
  gtk_widget_show (button_clone);
  gtk_box_pack_start (GTK_BOX (vbox_clone), button_clone, FALSE, FALSE, 0);
  
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_clone), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic ("C_lone the remote repository");
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_clone, "Cloning data");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_clone, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_clone, false);

  g_signal_connect ((gpointer) button_clone, "clicked", G_CALLBACK (on_button_clone_clicked), gpointer (this));

  // Write tester.
  label_write_test = gtk_label_new ("");
  gtk_widget_show (label_write_test);
  page_number_write_test = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_write_test);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_write_test, "Remote repository write test");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_write_test, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_write_test, false);

  // GUI for pushing our data into the remote repository.
  vbox_push = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_push);
  page_number_push = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_push);

  label_push = gtk_label_new ("");
  gtk_widget_show (label_push);
  gtk_box_pack_start (GTK_BOX (vbox_push), label_push, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_push), 0, 0.5);

  button_push = gtk_button_new ();
  gtk_widget_show (button_push);
  gtk_box_pack_start (GTK_BOX (vbox_push), button_push, FALSE, FALSE, 0);
  
  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_push), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic ("_Push my data to the remote repository");
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_push, "Would you like to push your data to the repository?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_push, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_push, true);

  g_signal_connect ((gpointer) button_push, "clicked", G_CALLBACK (on_button_push_clicked), gpointer (this));

  // Conflict resolution.
  vbox_conflict = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_conflict);
  page_number_conflict = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_conflict);

  label_conflict = gtk_label_new ("When synchronizing, if there is a conflict between the data in the remote repository and the local data, give preference to");
  gtk_widget_show (label_conflict);
  gtk_box_pack_start (GTK_BOX (vbox_conflict), label_conflict, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label_conflict), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label_conflict), 0, 0.5);

  GSList *radiobutton_conflict_local_group = NULL;

  radiobutton_conflict_local = gtk_radio_button_new_with_mnemonic (NULL, "the _local data");
  gtk_widget_show (radiobutton_conflict_local);
  gtk_box_pack_start (GTK_BOX (vbox_conflict), radiobutton_conflict_local, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_conflict_local), radiobutton_conflict_local_group);
  radiobutton_conflict_local_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_conflict_local));

  radiobutton_conflict_remote = gtk_radio_button_new_with_mnemonic (NULL, "the data in the _remote repository");
  gtk_widget_show (radiobutton_conflict_remote);
  gtk_box_pack_start (GTK_BOX (vbox_conflict), radiobutton_conflict_remote, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_conflict_remote), radiobutton_conflict_local_group);
  radiobutton_conflict_local_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_conflict_remote));

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_conflict, "Conflict resolution");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_conflict, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_conflict, true);
  
  // Build the confirmation stuff.
  label_confirm = gtk_label_new ("Settings are ready to be applied");
  gtk_widget_show (label_confirm);
  page_number_confirm = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Settings are ready to be applied");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);
  
  label_progress = gtk_label_new ("");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);
  
  label_summary = gtk_label_new ("Settings have been applied");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish building assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}


RemoteRepositoryAssistant::~RemoteRepositoryAssistant()
{
  gw_destroy_source(event_id_entry_repository);
}


void RemoteRepositoryAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_assistant_prepare(page);
}


void RemoteRepositoryAssistant::on_assistant_prepare (GtkWidget *page)
{
  extern Settings * settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(bible);
  
  if (page == checkbutton_use_repository) {
    // Set all values in the GUI, according to the project configuration if it is a Bible, or the general configuration for project notes.

    // Whether to use the remote repository.
    bool use_remote_repository = false;
    if (bible_notes_selector_bible ())
      use_remote_repository = projectconfig->git_use_remote_repository_get();
    else
      use_remote_repository = settings->genconfig.consultation_notes_git_use_remote_repository_get();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_use_repository), use_remote_repository);

    // Set the repository location.
    ustring repository_url;
    if (bible_notes_selector_bible ())
      repository_url = projectconfig->git_remote_repository_url_get();
    else
      repository_url = settings->genconfig.consultation_notes_git_remote_repository_url_get();
    ignore_entry_repository_changed = true;
    gtk_entry_set_text (GTK_ENTRY (entry_repository), repository_url.c_str());
    ignore_entry_repository_changed = false;

    // Set conflict handling values.
    GitConflictHandlingType conflicthandling = gchtTakeMe;
    if (bible_notes_selector_bible ())
      conflicthandling = (GitConflictHandlingType) projectconfig->git_remote_repository_conflict_handling_get();
    else
      conflicthandling = (GitConflictHandlingType) settings->genconfig.consultation_notes_git_remote_repository_conflict_handling_get();
    switch (conflicthandling) {
    case gchtTakeMe:
      {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_conflict_local), true);
        break;
      }
    case gchtTakeServer:
      {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_conflict_remote), true);
        break;
      }
    }
   
  }
  
  if (page == label_try_git) {
    // Prepare for the page to try git.
    if (!git_tried_and_okay) {
      git_tried_and_okay = try_git ();
      if (git_tried_and_okay) {
        gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_try_git, true);
      }
    }
  }

  if (page == vbox_repository) {
    // Prepare for the page where the repository URL is set.
    gtk_widget_grab_focus (entry_repository);
    on_entry_repository();
  }

  if (page == vbox_clone) {
    // Prepare for the page where the cloning is done.
    if (repository_url_get() != previously_cloned_url) {
      repository_unclone();
    }
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_clone, repository_was_cloned());
    if (!repository_was_cloned()) {
      gtk_label_set_text (GTK_LABEL (label_clone), "");
    }
  }
  
  if (page == label_write_test) {
    // Prepare for the page for testing write access.
    if (!write_access_granted) {
      test_write_access ();
    }
  }
}


void RemoteRepositoryAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_assistant_apply();
}


void RemoteRepositoryAssistant::on_assistant_apply ()
{
  // Configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(bible);

  // Whether to use the remote repository.
  bool use_remote_repository = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_use_repository));
  if (bible_notes_selector_bible ())
    projectconfig->git_use_remote_repository_set(use_remote_repository);
  else
    settings->genconfig.consultation_notes_git_use_remote_repository_set(use_remote_repository);

  // The remote repository URL.
  if (bible_notes_selector_bible ())
    projectconfig->git_remote_repository_url_set(repository_url_get());
  else
    settings->genconfig.consultation_notes_git_remote_repository_url_set(repository_url_get());
  
  // Save conflict handling system.
  GitConflictHandlingType conflicthandling = gchtTakeMe;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_conflict_remote)))
    conflicthandling = gchtTakeServer;
  if (bible_notes_selector_bible ())
    projectconfig->git_remote_repository_conflict_handling_set(conflicthandling);
  else
    settings->genconfig.consultation_notes_git_remote_repository_conflict_handling_set(conflicthandling);

  // If the repository was cloned, move it into place.
  if (repository_was_cloned()) {
    ustring destination_data_directory;
    if (bible_notes_selector_bible ())
      destination_data_directory = project_data_directory_project(bible);
    else
      destination_data_directory = notes_shared_storage_folder ();
    unix_rmdir(destination_data_directory);
    unix_mv(persistent_clone_directory, destination_data_directory);
    // Switch rename detection off. 
    // This is necessary for the consultation notes, since git has been seen to detect spurious renames.
    GwSpawn spawn ("git");
    spawn.workingdirectory (destination_data_directory);
    spawn.arg ("config");
    spawn.arg ("--global");
    spawn.arg ("diff.renamelimit");
    spawn.arg ("0");
    spawn.run ();
  }

  if (bible_notes_selector_bible ()) {
    // Take a snapshot of the whole project.
    snapshots_shoot_project (bible);
  } else{
    // Create the index for the consultation notes.
    notes_create_index ();
  }

  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint RemoteRepositoryAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((RemoteRepositoryAssistant *) user_data)->assistant_forward (current_page);
}


gint RemoteRepositoryAssistant::assistant_forward (gint current_page)
{
  // Default behaviour is to go to the next page.
  gint new_page_number = current_page + 1;

  if (current_page == page_number_use_repository) {
    // If the repository is not used, go straight to the confirmation page.
    if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_use_repository))) {
      new_page_number = page_number_confirm;
    }
  }

  if (current_page == page_number_task_selector) {
    // Go to the right page depending on which task is selected.
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_task_selector_settings))) {
      new_page_number = page_number_conflict;
    }
  }

  if (current_page == page_number_clone) {
    if (!repository_was_cloned()) {
      // If we pushed data to the repository, skip the write access test is needed.
      new_page_number = page_number_conflict;
    }
  }

  // Return the new page.
  return new_page_number;
}


bool RemoteRepositoryAssistant::try_git ()
// Tries git and returns true if everything's fine.
{
  // Progress.
  ProgressWindow progresswindow ("Trying the contents tracker", false);
  
  // Whether git is okay.
  bool okay = true;
  
  if (okay) {
    progresswindow.set_fraction (0.05);
    gw_message ("Check git version number");
    okay = check_git_version ();
  }
  
  // Clean the directory to work in.
  {
    ustring directory = git_testing_directory ("");
    unix_rmdir (directory);
    gw_mkdir_with_parents (directory);
  }
  
  if (okay) {
    progresswindow.set_fraction (0.11);
    gw_message ("Create first local repository");
    okay = try_git_create_repository ("local1", false);
  }

  if (okay) {
    progresswindow.set_fraction (0.17);
    gw_message ("Store data into first local repository");
    okay = try_git_store_data_in_repository ("local1", "--test--");
  }

  if (okay) {
    progresswindow.set_fraction (0.23);
    gw_message ("Create remote repository");
    okay = try_git_create_repository ("remote", true);
  }

  if (okay) {
    progresswindow.set_fraction (0.29);
    gw_message ("Fetch data from the first local repository into the remote one");
    okay = try_git_fetch_repository ("remote", "local1");
  }

  if (okay) {
    progresswindow.set_fraction (0.35);
    gw_message ("Checkout the first local repository");
    okay = try_git_checkout_repository ("local1", "remote");
  }

  if (okay) {
    progresswindow.set_fraction (0.41);
    gw_message ("Check data of first local repository");
    okay = try_git_check_data_in_repository ("local1", "--test--");
  }

  if (okay) {
    progresswindow.set_fraction (0.47);
    gw_message ("Checkout the second local repository");
    okay = try_git_checkout_repository ("local2", "remote");
  }

  if (okay) {
    progresswindow.set_fraction (0.52);
    gw_message ("Check data of second local repository");
    okay = try_git_check_data_in_repository ("local2", "--test--");
  }

  if (okay) {
    progresswindow.set_fraction (0.58);
    gw_message ("Store different data into first repository");
    okay = try_git_store_data_in_repository ("local1", "---test---");
  }

  if (okay) {
    progresswindow.set_fraction (0.64);
    gw_message ("Push first repository");
    okay = try_git_push_repository ("local1");
  }

  if (okay) {
    progresswindow.set_fraction (0.70);
    gw_message ("Pull second repository");
    okay = try_git_pull_repository ("local2");
  }

  if (okay) {
    progresswindow.set_fraction (0.76);
    gw_message ("Check data in second repository");
    okay = try_git_check_data_in_repository ("local2", "---test---");
  }

  if (okay) {
    progresswindow.set_fraction (0.82);
    gw_message ("Store different data into second repository");
    okay = try_git_store_data_in_repository ("local2", "----test----");
  }

  if (okay) {
    progresswindow.set_fraction (0.88);
    gw_message ("Push second repository");
    okay = try_git_push_repository ("local2");
  }

  if (okay) {
    progresswindow.set_fraction (0.94);
    gw_message ("Pull first repository");
    okay = try_git_pull_repository ("local1");
  }

  if (okay) {
    progresswindow.set_fraction (1);
    gw_message ("Check data in first repository");
    okay = try_git_check_data_in_repository ("local1", "----test----");
  }

  // Return whether git is ok.
  return okay;
}


bool RemoteRepositoryAssistant::check_git_version ()
// Checks the git version.
{
  bool okay = false;
  GwSpawn spawn("git");
  spawn.arg("--version");
  spawn.read();
  spawn.run();
  ustring versiontext;
  if (!spawn.standardout.empty())
    versiontext = spawn.standardout[0];
  gw_message (versiontext);
  Parse parse1(versiontext, false);
  if (parse1.words.size() >= 3) {
    Parse parse2(parse1.words[2], false, ".");
    if (parse2.words.size() >= 3) {
      unsigned int version = 100 * convert_to_int(parse2.words[0]) + 10 * convert_to_int(parse2.words[1]) + convert_to_int(parse2.words[2]);
      if (version >= 150)
        okay = true;
    }
  }
  if (!okay) {
    versiontext.append(", but should be 1.5.0 or higher");
    gtk_label_set_text(GTK_LABEL(label_try_git), versiontext.c_str());
  }
  return okay;
}


ustring RemoteRepositoryAssistant::git_testing_directory (const ustring& name)
// Get the directory of the repository for testing git.
{
  ustring directory = gw_build_filename (directories_get_temp(), "git-test", name);
  return directory;
}


ustring RemoteRepositoryAssistant::git_testing_file (const ustring& name)
// Gets the filename of the repository for testing git.
{
  ustring filename = gw_build_filename (git_testing_directory (name), "data");
  return filename;
}


bool RemoteRepositoryAssistant::try_git_create_repository (const ustring& name, bool remote)
{
  ustring directory = git_testing_directory (name);
  gw_mkdir_with_parents (directory);
  GwSpawn spawn ("git");
  spawn.workingdirectory (directory);
  if (remote) 
    spawn.arg ("--bare");
  spawn.arg ("init");
  if (remote)
    spawn.arg ("--shared");
  spawn.run();
  bool okay = (spawn.exitstatus == 0);
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git init fails to create a repository");
  }
  return okay;
}


bool RemoteRepositoryAssistant::try_git_store_data_in_repository (const ustring& repository, const gchar* data)
// Put data into the repository.
{
  ustring filename = git_testing_file (repository);
  g_file_set_contents(filename.c_str(), data, -1, NULL);
  ustring directory = git_testing_directory (repository);

  GwSpawn spawn ("git");
  spawn.workingdirectory (directory);
  spawn.arg ("add");
  spawn.arg (".");
  spawn.run();
  bool okay = (spawn.exitstatus == 0);
  
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git add fails to add data to the repository");
  }

  if (okay) {
    GwSpawn spawn ("git");
    spawn.workingdirectory (directory);
    spawn.arg ("commit");
    spawn.arg ("-m");
    spawn.arg ("Commit");
    spawn.arg ("-a");  
    spawn.run();
    okay = (spawn.exitstatus == 0);
  }
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git commit fails to commit data to the repository");
  }

  return okay;
}


bool RemoteRepositoryAssistant::try_git_check_data_in_repository (const ustring& repository, const gchar* data)
// Checks the data into the repository.
{
  ustring filename = git_testing_file (repository);
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  bool okay = (contents);
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "cannot read data in repository");
  }
  ustring repository_data;
  if (contents) {
    repository_data = contents;
    g_free (contents);
  }
  ustring standard_data (data);
  if (okay) {
    if (repository_data != standard_data) {
      gtk_label_set_text(GTK_LABEL(label_try_git), "unexpected data in repository");
    }
  }
  return okay;
}


bool RemoteRepositoryAssistant::try_git_fetch_repository (const ustring& remote, const ustring& local)
// Fetch the local into the remote repository.
{
  GwSpawn spawn ("git");
  spawn.workingdirectory (git_testing_directory (remote));
  spawn.arg ("--bare");
  spawn.arg ("fetch");
  spawn.arg (git_testing_directory (local));
  spawn.arg ("master:master");
  spawn.run();
  bool okay = (spawn.exitstatus == 0);
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git --bare fetch fails to fetch data into remote repository");
  }
  return okay;
}


bool RemoteRepositoryAssistant::try_git_checkout_repository (const ustring& local, const ustring& remote)
// Checks the remote repository out into the local one.
{
  // Directory to run the cloning process in.
  ustring cloning_directory = git_testing_directory (local) + "clone";
  gw_mkdir_with_parents(cloning_directory);

  // Clone the remote repository.
  GwSpawn spawn("git");
  spawn.arg("clone");
  spawn.workingdirectory(cloning_directory);
  spawn.arg(git_testing_directory (remote));
  spawn.run();
  bool okay = (spawn.exitstatus == 0);

  // Message if things didn't work out.
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git clone fails to clone the repository");
  }
  
  // Move the repository into place.
  ustring cloned_directory = gw_build_filename (cloning_directory, remote);
  ustring local_directory = git_testing_directory (local);
  unix_rmdir(local_directory);
  if (okay) {
    unix_mv(cloned_directory, local_directory);
  }
  unix_rmdir(cloning_directory);
  
  // Return result.
  return okay;
}


bool RemoteRepositoryAssistant::try_git_pull_repository (const ustring& name)
// Pull repository.
{
  GwSpawn spawn ("git");
  spawn.workingdirectory (git_testing_directory (name));
  spawn.arg ("pull");
  spawn.run();
  bool okay = (spawn.exitstatus == 0);
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git pull from remote repository failed");
  }
  return okay;
}


bool RemoteRepositoryAssistant::try_git_push_repository (const ustring& name)
// Push repository.
{
  GwSpawn spawn ("git");
  spawn.workingdirectory (git_testing_directory (name));
  spawn.arg ("push");
  spawn.run();
  bool okay = (spawn.exitstatus == 0);
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git push to remote repository failed");
  }
  return okay;
}


void RemoteRepositoryAssistant::on_entry_repository_changed (GtkEditable *editable, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_entry_repository();
}


void RemoteRepositoryAssistant::on_entry_repository ()
{
  if (ignore_entry_repository_changed)
    return;
  gw_destroy_source(event_id_entry_repository);
  event_id_entry_repository = g_timeout_add_full(G_PRIORITY_DEFAULT, 1000, GSourceFunc(on_entry_changed_timeout), gpointer(this), NULL);
}


bool RemoteRepositoryAssistant::on_entry_changed_timeout(gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->entry_changed_timeout();
  return false;
}


void RemoteRepositoryAssistant::entry_changed_timeout()
{
  // Progress.
  ProgressWindow progresswindow ("Testing read access", false);
  progresswindow.set_fraction (0.5);
  
  // Event done.
  event_id_entry_repository = 0;

  // Test read access to the repository.
  GwSpawn spawn("git");
  spawn.arg("ls-remote");
  spawn.arg(repository_url_get());
  spawn.read();
  spawn.run();
  bool access = spawn.exitstatus == 0;
  ustring message;
  if (access) {
    message = "Read access to the repository has been granted";
  } else {
    message = "Cannot access the repository:";
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      message.append("\n");
      message.append(spawn.standarderr[i]);
    }
  }
  gtk_label_set_text (GTK_LABEL (label_repository_accessible), message.c_str());
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_repository, access);
}


ustring RemoteRepositoryAssistant::repository_url_get()
// Gets the URL of the remote repository.
{
  ustring url;
  url = gtk_entry_get_text (GTK_ENTRY (entry_repository));
  url = trim(url);
  replace_text(url, "http", "");
  return url;
}


void RemoteRepositoryAssistant::on_button_clone_clicked (GtkButton *button, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_button_clone();
}


void RemoteRepositoryAssistant::on_button_clone ()
{
  // Progress.
  ProgressWindow progresswindow ("Cloning repository", false);
  progresswindow.set_fraction (0.5);
  
  // Clear out persistent clone directory.
  repository_unclone();
  
  // Create temporal clone directory.
  ustring temporal_clone_directory = git_testing_directory ("tempclone");
  unix_rmdir(temporal_clone_directory);
  gw_mkdir_with_parents(temporal_clone_directory);
  
  // Clone the remote repository
  GwSpawn spawn("git");
  spawn.workingdirectory(temporal_clone_directory);
  spawn.arg ("clone");
  spawn.arg(repository_url_get());
  spawn.run();

  if (spawn.exitstatus == 0) {
    // Move the cloned repository into the persistent clone directory.
    ReadDirectories rd (temporal_clone_directory, "", "");
    if (!rd.directories.empty()) {
      ustring subdirectory = rd.directories[0];
      subdirectory = gw_build_filename (temporal_clone_directory, subdirectory);
      unix_mv (subdirectory, persistent_clone_directory);
    }
    unix_rmdir(temporal_clone_directory);
  } else {
    // Clone failed, clear out any remains.
    repository_unclone();
  }  
  
  // Update structures.
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_clone, repository_was_cloned());
  if (repository_was_cloned()) {
    gtk_label_set_text (GTK_LABEL (label_clone), "The data has been cloned, you can go forward");
    previously_cloned_url = repository_url_get();
  } else {
    gtk_label_set_text (GTK_LABEL (label_clone), "Cloning the data failed, please try again");
    repository_unclone();
  }
}


bool RemoteRepositoryAssistant::repository_was_cloned ()
// Check whether the repository was cloned.
{
  return g_file_test (persistent_clone_directory.c_str(), G_FILE_TEST_IS_DIR);
}


void RemoteRepositoryAssistant::repository_unclone ()
// Does the house keeping for indicating that the repository was not cloned.
{
  unix_rmdir(persistent_clone_directory);
  previously_cloned_url.clear();
  write_access_granted = false;
}


void RemoteRepositoryAssistant::test_write_access ()
// Checks whether there is write access from the local clone to the remote repository.
{
  // GUI update.
  // If a wrong host is entered as the git repository, the testing for the write access
  // may "hang" for a long time. For that reason it can be cancelled by the user.
  ProgressWindow progresswindow ("Testing write access", true);
  gtk_label_set_text (GTK_LABEL (label_write_test), "Testing write access to the remote repository");

  // Temporal file for trying write access.
  ustring filename = "test_repository_writable";
  g_file_set_contents(gw_build_filename(persistent_clone_directory, filename).c_str(), "", 0, NULL);

  // Add this file and commit it locally.
  progresswindow.set_fraction (0.2);
  {
    GwSpawn spawn("git");
    spawn.workingdirectory(persistent_clone_directory);
    spawn.arg("add");
    spawn.arg(".");
    spawn.run();
    write_access_granted = (spawn.exitstatus == 0);
  }
  if (write_access_granted) {
    GwSpawn spawn("git");
    spawn.workingdirectory(persistent_clone_directory);
    spawn.arg("commit");
    spawn.arg("-m");
    spawn.arg("Write test");
    spawn.arg("-a");
    spawn.run();
    write_access_granted = (spawn.exitstatus == 0);
  }

  // Pull changes.
  progresswindow.set_fraction (0.4);
  if (write_access_granted) {
    GwSpawn spawn("git");
    spawn.workingdirectory(persistent_clone_directory);
    spawn.arg ("pull");
    spawn.run();
    // When pulling from an empty repository, the exit status is undefined.
    // So we cannot test for it here.
    // write_access_granted = (spawn.exitstatus == 0);
  }
  // Push the changes to see if there is write access. 
  // Notice the --all switch to be used when pushing to an empty remote repository.
  progresswindow.set_fraction (0.6);
  if (write_access_granted) {
    GwSpawn spawn("git");
    spawn.workingdirectory(persistent_clone_directory);
    spawn.arg ("push");
    spawn.arg ("--all");
    spawn.run();
    write_access_granted = (spawn.exitstatus == 0);
  }

  // Remove the temporal file again from the remote repository.
  progresswindow.set_fraction (0.8);
  unlink(gw_build_filename(persistent_clone_directory, filename).c_str());
  if (write_access_granted) {
    GwSpawn spawn("git");
    spawn.workingdirectory(persistent_clone_directory);
    spawn.arg("commit");
    spawn.arg("-m");
    spawn.arg("Write test");
    spawn.arg("-a");
    spawn.run();
    write_access_granted = (spawn.exitstatus == 0);
  }
  progresswindow.set_fraction (1);
  if (write_access_granted) {
    GwSpawn spawn("git");
    spawn.workingdirectory(persistent_clone_directory);
    spawn.arg ("push");
    spawn.run();
    write_access_granted = (spawn.exitstatus == 0);
  }

  // Set the GUI.
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_write_test, write_access_granted);
  if (write_access_granted) {
    gtk_label_set_text (GTK_LABEL (label_write_test), "Write access was granted, you can go forward");
  } else {
    gtk_label_set_text (GTK_LABEL (label_write_test), "No write access. Please check the system log for more information");
  }  
}


void RemoteRepositoryAssistant::on_button_push_clicked (GtkButton *button, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_button_push();
}


void RemoteRepositoryAssistant::on_button_push ()
/*
It copies the existing data, without the .git directory, into the persistent clone,
replaces any data that was there, and then pushes this data to the remote repository.
This makes the remote repository to have an exact copy of our data.
*/
{
  // Progress.
  ProgressWindow progresswindow ("Pushing your data", false);
  progresswindow.set_fraction (0.2);
  
  // Copy our data into a temporal location.
  ustring my_data_directory = notes_shared_storage_folder ();
  if (bible_notes_selector_bible ())
    my_data_directory = project_data_directory_project(bible);
  ustring temporal_data_directory = git_testing_directory ("mydata");
  unix_cp_r (my_data_directory, temporal_data_directory);

  // In rare cases a .git directory could have been copied along with our data. Remove that.
  unix_rmdir (gw_build_filename (temporal_data_directory, ".git"));

  // Remove all directories and all files from the persistent clone directory, but leave the .git directory
  {
    ReadDirectories rd (persistent_clone_directory, "", "");
    for (unsigned int i = 0; i < rd.directories.size(); i++) {
      if (rd.directories[i] != ".git") {
        unix_rmdir (gw_build_filename (persistent_clone_directory, rd.directories[i]));
      }
    }
    ReadFiles rf (persistent_clone_directory, "", "");
    for (unsigned int i = 0; i < rf.files.size(); i++) {
      unlink (gw_build_filename (persistent_clone_directory, rf.files[i]).c_str());
    }
  }
  
  // Move our data, from its temporal location, into the persistent clone directory.
  progresswindow.set_fraction (0.4);
  {
    ReadDirectories rd (temporal_data_directory, "", "");
    for (unsigned int i = 0; i < rd.directories.size(); i++) {
      unix_mv (gw_build_filename (temporal_data_directory, rd.directories[i]), persistent_clone_directory);
    }
    ReadFiles rf (temporal_data_directory, "", "");
    for (unsigned int i = 0; i < rf.files.size(); i++) {
      unix_mv (gw_build_filename (temporal_data_directory, rf.files[i]), persistent_clone_directory);
    }
  }

  // Commit the new data in the persistent clone directory.
  progresswindow.set_fraction (0.55);
  {
    GwSpawn spawn ("git");
    spawn.workingdirectory (persistent_clone_directory);
    spawn.arg ("add");
    spawn.arg (".");
    spawn.run ();
  }
  progresswindow.set_fraction (0.65);
  {
    GwSpawn spawn ("git");
    spawn.workingdirectory (persistent_clone_directory);
    spawn.arg ("commit");
    spawn.arg ("-a");
    spawn.arg ("-m");
    spawn.arg ("user data into repo");
    spawn.run ();
  }

  // Push our data to the remote repository.
  progresswindow.set_fraction (0.8);
  GwSpawn spawn("git");
  spawn.workingdirectory(persistent_clone_directory);
  spawn.arg ("push");
  spawn.run();

  // Take action depending on the outcome of pushing to the remote repository.
  if (spawn.exitstatus == 0) {
    // Clone okay.
    gtk_label_set_text (GTK_LABEL (label_push), "Your data has been pushed to the remote repository");
  } else {
    // Clone failed.
    gtk_label_set_text (GTK_LABEL (label_push), "Your data could not be pushed to the remote repository,\nplease restart the assistant");
    repository_unclone();
  }  
}


bool RemoteRepositoryAssistant::bible_notes_selector_bible ()
{
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_bible_notes_selector_bible));
}


