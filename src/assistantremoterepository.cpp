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


RemoteRepositoryAssistant::RemoteRepositoryAssistant(int dummy) :
AssistantBase("Keyterms", "git_setup")
// Assistant for managing the remote repository.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));
  g_signal_connect (G_OBJECT (assistant), "prepare", G_CALLBACK (on_assistant_prepare_signal), gpointer(this));

  introduction ("Remote repository management");

  // Configuration and initialization.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  event_id_pending_tasks = 0;
  project_pending_tasks_count = -1;
  previous_project_pending_tasks_count = -1;
  event_id_entry_repository = 0;

  // Build the GUI for the setting whether to use a remote repository.
  vbox_use_repository = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_use_repository);
  page_number_use_repository = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_use_repository);

  checkbutton_use_repository = gtk_check_button_new_with_mnemonic ("_Use remote repository");
  gtk_widget_show (checkbutton_use_repository);
  gtk_box_pack_start (GTK_BOX (vbox_use_repository), checkbutton_use_repository, FALSE, FALSE, 0);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_use_repository), projectconfig->git_use_remote_repository_get());

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_use_repository, "Would you like to use a remote repository?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_use_repository, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_use_repository, true);

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

  radiobutton_task_selector_url = gtk_radio_button_new_with_mnemonic (NULL, "To set which _remote repository to use");
  gtk_widget_show (radiobutton_task_selector_url);
  gtk_box_pack_start (GTK_BOX (vbox_task_selector), radiobutton_task_selector_url, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_task_selector_url), radiobutton_task_selector_url_group);
  radiobutton_task_selector_url_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_task_selector_url));

  radiobutton_task_selector_sync = gtk_radio_button_new_with_mnemonic (NULL, "To _synchronize the local and remote repository");
  gtk_widget_show (radiobutton_task_selector_sync);
  gtk_box_pack_start (GTK_BOX (vbox_task_selector), radiobutton_task_selector_sync, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_task_selector_sync), radiobutton_task_selector_url_group);
  radiobutton_task_selector_url_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_task_selector_sync));

  radiobutton_task_selector_settings = gtk_radio_button_new_with_mnemonic (NULL, "To change a couple of se_ttings");
  gtk_widget_show (radiobutton_task_selector_settings);
  gtk_box_pack_start (GTK_BOX (vbox_task_selector), radiobutton_task_selector_settings, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_task_selector_settings), radiobutton_task_selector_url_group);
  radiobutton_task_selector_url_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_task_selector_settings));

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_task_selector, "What would you like to do?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_task_selector, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_task_selector, true);
  
  // Build GUI for waiting till the pending tasks are over.
  label_pending_tasks = gtk_label_new ("");
  gtk_widget_show (label_pending_tasks);
  page_number_pending_tasks = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_pending_tasks);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_pending_tasks, "Waiting till any pending tasks are over");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_pending_tasks, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_pending_tasks, false);

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

  // Set the repository location.
  gtk_entry_set_text (GTK_ENTRY (entry_repository), projectconfig->git_remote_repository_url_get().c_str());
  
  g_signal_connect ((gpointer) entry_repository, "changed", G_CALLBACK (on_entry_repository_changed), gpointer (this));

  // GUI for cloning the repository.
  vbox_clone = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_clone);
  page_number_clone = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_clone);

  label_clone = gtk_label_new ("clone");
  gtk_widget_show (label_clone);
  gtk_box_pack_start (GTK_BOX (vbox_clone), label_clone, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_clone), 0, 0.5);

  button_clone = gtk_button_new ();
  gtk_widget_show (button_clone);
  gtk_box_pack_start (GTK_BOX (vbox_clone), button_clone, FALSE, FALSE, 0);
  
  GtkWidget *alignment1;
  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_clone), alignment1);

  GtkWidget *hbox1;
  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  GtkWidget *image1;
  image1 = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  GtkWidget *label11;
  label11 = gtk_label_new_with_mnemonic ("C_opy remote repository to local data");
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox1), label11, FALSE, FALSE, 0);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_clone, "Copying the remote repository");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_clone, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_clone, false);

  g_signal_connect ((gpointer) button_clone, "clicked", G_CALLBACK (on_button_clone_clicked), gpointer (this));

  
  
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
  
  // Timers.
  event_id_pending_tasks = g_timeout_add_full(G_PRIORITY_DEFAULT, 600, GSourceFunc(on_pending_tasks_timeout), gpointer(this), NULL);
}

RemoteRepositoryAssistant::~RemoteRepositoryAssistant()
{
  gw_destroy_source(event_id_pending_tasks);
  gw_destroy_source(event_id_entry_repository);
}

void RemoteRepositoryAssistant::on_assistant_prepare_signal (GtkAssistant *assistant, GtkWidget *page, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_assistant_prepare(page);
}


void RemoteRepositoryAssistant::on_assistant_prepare (GtkWidget *page)
{
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
}


void RemoteRepositoryAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_assistant_apply();
}


void RemoteRepositoryAssistant::on_assistant_apply ()
{
  // Configurations.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // Whether to use the remote repository.
  bool use_remote_repository = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_use_repository));
  projectconfig->git_use_remote_repository_set(use_remote_repository);

  // The remote repository URL.
  projectconfig->git_remote_repository_url_set(repository_url_get());


  /*
  // Todo store settings.
    
  // Remote update interval.
  gtk_spin_button_update(GTK_SPIN_BUTTON(spinbutton_update));
  projectconfig->git_remote_update_interval_set(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton_update)));

  // Save conflict handling system.
  GitConflictHandlingType conflicthandling = gchtTakeMe;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_conflict_server)))
    conflicthandling = gchtTakeServer;
  projectconfig->git_remote_repository_conflict_handling_set(conflicthandling);
  
  */
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


gint RemoteRepositoryAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((RemoteRepositoryAssistant *) user_data)->assistant_forward (current_page);
}

gint RemoteRepositoryAssistant::assistant_forward (gint current_page)
{
  // New page number variables.
  gint new_page_number;
  bool new_page_determined = false;

  if (current_page == page_number_use_repository) {
    // If the repository is not used, go straight to the confirmation page.
    if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_use_repository))) {
      new_page_number = page_number_confirm;
      new_page_determined = true;
    }
  }

  // From here and onwards the remote repository is used.
  

  // Default behaviour is to go to the next page.
  if (!new_page_determined) {
    new_page_number = ++current_page;
    new_page_determined = true;
  }

  // Return the new page.
  return new_page_number;
}


bool RemoteRepositoryAssistant::try_git ()
// Tries git and returns true if everything's fine.
{
  // Whether git is okay.
  bool okay = true;
  
  if (okay) {
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
    gw_message ("Create first local repository");
    okay = try_git_create_repository ("local1", false);
  }

  if (okay) {
    gw_message ("Store data into first local repository");
    okay = try_git_store_data_in_repository ("local1", "--test--");
  }

  if (okay) {
    gw_message ("Create remote repository");
    okay = try_git_create_repository ("remote", true);
  }

  if (okay) {
    gw_message ("Fetch data from the first local repository into the remote one");
    okay = try_git_fetch_repository ("remote", "local1");
  }

  if (okay) {
    gw_message ("Checkout the first local repository");
    okay = try_git_checkout_repository ("local1", "remote");
  }

  if (okay) {
    gw_message ("Check data of first local repository");
    okay = try_git_check_data_in_repository ("local1", "--test--");
  }

  if (okay) {
    gw_message ("Checkout the second local repository");
    okay = try_git_checkout_repository ("local2", "remote");
  }

  if (okay) {
    gw_message ("Check data of second local repository");
    okay = try_git_check_data_in_repository ("local2", "--test--");
  }

  if (okay) {
    gw_message ("Store different data into first repository");
    okay = try_git_store_data_in_repository ("local1", "---test---");
  }

  if (okay) {
    gw_message ("Push first repository");
    okay = try_git_push_repository ("local1");
  }

  if (okay) {
    gw_message ("Pull second repository");
    okay = try_git_pull_repository ("local2");
  }

  if (okay) {
    gw_message ("Check data in second repository");
    okay = try_git_check_data_in_repository ("local2", "---test---");
  }

  if (okay) {
    gw_message ("Store different data into second repository");
    okay = try_git_store_data_in_repository ("local2", "----test----");
  }

  if (okay) {
    gw_message ("Push second repository");
    okay = try_git_push_repository ("local2");
  }

  if (okay) {
    gw_message ("Pull first repository");
    okay = try_git_pull_repository ("local1");
  }

  if (okay) {
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
    gtk_label_set_text(GTK_LABEL(label_try_git), "git-init fails to create a repository");
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
    gtk_label_set_text(GTK_LABEL(label_try_git), "git-add fails to add data to the repository");
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
    gtk_label_set_text(GTK_LABEL(label_try_git), "git-commit fails to commit data to the repository");
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
  GwSpawn spawn("git-clone");
  spawn.workingdirectory(cloning_directory);
  spawn.arg(git_testing_directory (remote));
  spawn.run();
  bool okay = (spawn.exitstatus == 0);

  // Message if things didn't work out.
  if (!okay) {
    gtk_label_set_text(GTK_LABEL(label_try_git), "git-clone fails to clone the repository");
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


bool RemoteRepositoryAssistant::on_pending_tasks_timeout(gpointer user_data)
{
  return ((RemoteRepositoryAssistant *) user_data)->on_pending_tasks();
}

bool RemoteRepositoryAssistant::on_pending_tasks()
// Sets the interface depending on whether there are any git tasks pending for the project.
{
  extern Settings *settings;
  project_pending_tasks_count = git_count_tasks_project(settings->genconfig.project_get());
  if (project_pending_tasks_count == 0) {
    if (previous_project_pending_tasks_count != 0) {
      gtk_label_set_text (GTK_LABEL (label_pending_tasks), "No pending tasks, you can go forward");
      gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_pending_tasks, true);
      previous_project_pending_tasks_count = project_pending_tasks_count;
      // Pause processing git tasks if there are no pending tasks for this project.
      // This is because tasks will be produced, but these tasks are unwanted.
      git_command_pause(true);
    }
  } else {
    ustring text = "Waiting for the remaining ";
    text.append (convert_to_string (project_pending_tasks_count));
    text.append (" tasks to complete"); 
    gtk_label_set_text (GTK_LABEL (label_pending_tasks), text.c_str());
  }
  return true;
}


void RemoteRepositoryAssistant::on_entry_repository_changed (GtkEditable *editable, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_entry_repository();
}


void RemoteRepositoryAssistant::on_entry_repository ()
{
  gw_destroy_source(event_id_entry_repository);
  event_id_entry_repository = g_timeout_add_full(G_PRIORITY_DEFAULT, 600, GSourceFunc(on_entry_changed_timeout), gpointer(this), NULL);
}

bool RemoteRepositoryAssistant::on_entry_changed_timeout(gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->entry_changed_timeout();
  return false;
}


void RemoteRepositoryAssistant::entry_changed_timeout()
{
  // Event done.
  event_id_entry_repository = 0;

  // Test read access to the repository.
  GwSpawn spawn("git-ls-remote");
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
  replace_text(url, "ssh", "");
  replace_text(url, "http", "");
  return url;
}


void RemoteRepositoryAssistant::on_button_clone_clicked (GtkButton *button, gpointer user_data)
{
  ((RemoteRepositoryAssistant *) user_data)->on_button_clone();
}


void RemoteRepositoryAssistant::on_button_clone () // Todo
{
}


/*
Todo working on git assistant.

Internally the things to be set are following the same order as now in the current setup dialog:

- Ask what to do:
* 1. Set the repository URL
* 2. Synchronize repos.
* 3. Settings.

If the repository changed, as compared to the previous one,
it should check out a new working copy.
Without that the assistant does not proceed.
When the repository did not change, the checking out of a new working 
copy is optional.
A label gives information to the user about that.
A button let's the user take the action. 

- Whether to synchronize the local and remote repository, 
* 1. Copy remote to local
* 2. Sync local with remote.
- Whether to make settings,
* 1. Update frequency.
* 2. Conflict resolution method.
When making settings only, we don't need to wait till the pending tasks are over.
In all other cases we need to wait.
Some tasks cannot yet be selected, these are such as when previous taks,
it depends upon, have not yet been done.
In such cases that task is greyed out.

 
Also do conflict resolution, so as to find out whether our resolution works with the git now installed.

And do a log read with our current tools to find out whether these works fine with git.

Run git status to see if that catches a conflict.
 
The stock version of git that comes with a distro, should be used.
It failed when collaboration tasks were asked of these, such push / pull / and merging 
or conflict management.
If the test passes, fine, else it recommends to install from source. 



bool GitSetupDialog::test_write_access(ustring & error)
// This tests whether there is write-access to the repository.
{
  // Writable variable.
  bool writable = false;

  // Create a temporal workspace.
  ustring temporal_directory = gw_build_filename(directories_get_temp(), "git_cloned_remote_repository");
  unix_rmdir(temporal_directory);
  gw_mkdir_with_parents(temporal_directory);

  // Clone the remote repository to the temporal workspace.
  {
    GwSpawn spawn("git-clone");
    spawn.workingdirectory(temporal_directory);
    spawn.arg(url_get());
    spawn.progress("Reading repository", false);
    spawn.run();
  }

  // The working directory.
  ustring working_directory = gw_build_filename(temporal_directory, gw_path_get_basename(url_get()));

  // Temporal file for trying write access.
  ustring filename = "__git_test_writable__";
  g_file_set_contents(gw_build_filename(working_directory, filename).c_str(), "", 0, NULL);

  // Add this file and commit it locally.
  {
    GwSpawn spawn("git-add");
    spawn.workingdirectory(working_directory);
    spawn.arg(".");
    spawn.run();
  }
  {
    GwSpawn spawn("git-commit");
    spawn.workingdirectory(working_directory);
    spawn.arg("-m");
    spawn.arg("Write test");
    spawn.arg("-a");
    spawn.run();
  }

  // Pull changes. Push them to see if there is write access.
  {
    GwSpawn spawn("git-pull");
    spawn.workingdirectory(working_directory);
    spawn.arg(url_get());
    spawn.run();
  }
  {
    GwSpawn spawn("git-push");
    spawn.workingdirectory(working_directory);
    spawn.arg(url_get());
    spawn.read();
    spawn.run();
    writable = (spawn.exitstatus == 0);
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      if (writable) {
        gw_message(spawn.standarderr[i]);
      } else {
        if (i > 0)
          error.append("\n");
        error.append(spawn.standarderr[i]);
      }
    }
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      gw_message(spawn.standardout[i]);
    }
  }

  // Remove the temporal file again from the remote repository.
  unlink(gw_build_filename(working_directory, filename).c_str());
  {
    GwSpawn spawn("git-commit");
    spawn.workingdirectory(working_directory);
    spawn.arg("-m");
    spawn.arg("Write test");
    spawn.arg("-a");
    spawn.run();
  }
  {
    GwSpawn spawn("git-push");
    spawn.workingdirectory(working_directory);
    spawn.arg(url_get());
    spawn.run();
  }

  // Remove the temporal work space.
  unix_rmdir(temporal_directory);

  // Return whether writable.
  return writable;
}

bool GitSetupDialog::test_pull_changes(ustring & error)
// This function first tries to pull the data from the remote repository 
// into a temporal copy of the local project.
// If that succeeds it does the actual pulling in the real project.
{
  // Variable whether the changes were pulled.
  bool pulled = false;

  // Copy the project to a temporal one.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ustring temporal_project = "__git__pull__test__";
  project_copy(project, temporal_project);

  // Try to pull the changes into the temporal project.
  ustring datadirectory = project_data_directory_project(temporal_project);
  {
    GwSpawn spawn("git-pull");
    spawn.workingdirectory(datadirectory);
    spawn.arg(url_get());
    spawn.read();
    spawn.run();
    pulled = (spawn.exitstatus == 0);
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      if (pulled) {
        gw_message(spawn.standarderr[i]);
      } else {
        if (i > 0)
          error.append("\n");
        error.append(spawn.standarderr[i]);
      }
    }
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      gw_message(spawn.standardout[i]);
    }
  }

  // Remove the temporal project again.
  project_delete(temporal_project);

  // If success, now pull the changes into the real project.
  if (pulled) {
    datadirectory = project_data_directory_project(project);
    GwSpawn spawn("git-pull");
    spawn.workingdirectory(datadirectory);
    spawn.arg(url_get());
    spawn.read();
    spawn.run();
    pulled = (spawn.exitstatus == 0);
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      if (pulled) {
        gw_message(spawn.standarderr[i]);
      } else {
        if (i > 0)
          error.append("\n");
        error.append(spawn.standarderr[i]);
      }
    }
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      gw_message(spawn.standardout[i]);
    }
  }
  // Return success status.
  return pulled;
}

bool GitSetupDialog::test_push_changes(ustring & error)
// This function push the changes into the remote repository.
{
  // Variable whether the changes were pushed.
  bool pushed = false;

  // Try to push the changes to the remote repository.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ustring datadirectory = project_data_directory_project(project);
  GwSpawn spawn("git-push");
  spawn.workingdirectory(datadirectory);
  spawn.arg(url_get());
  spawn.read();
  spawn.run();
  pushed = (spawn.exitstatus == 0);
  for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
    if (pushed) {
      gw_message(spawn.standarderr[i]);
    } else {
      if (i > 0)
        error.append("\n");
      error.append(spawn.standarderr[i]);
    }
  }
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    gw_message(spawn.standardout[i]);
  }

  // Return success status.
  return pushed;
}



*/
