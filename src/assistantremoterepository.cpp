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


RemoteRepositoryAssistant::RemoteRepositoryAssistant(int dummy) :
AssistantBase("Keyterms", "git_setup")
// Assistant for managing the remote repository.
{
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));

  introduction ("You can manage the remote repository.");

  // Configurations.
  extern Settings *settings;
  ustring project = settings->genconfig.project_get();
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

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
  check_git_version_okay = false;
  
  vbox_try_git = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_try_git);
  page_number_try_git = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox_try_git);

  label_try_git = gtk_label_new ("");
  gtk_widget_show (label_try_git);
  gtk_box_pack_start (GTK_BOX (vbox_try_git), label_try_git, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_try_git), 0, 0.5);

  expander_try_git_more = gtk_expander_new (NULL);
  gtk_widget_show (expander_try_git_more);
  gtk_box_pack_start (GTK_BOX (vbox_try_git), expander_try_git_more, TRUE, TRUE, 0);

  scrolledwindow_try_git_more = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_try_git_more);
  gtk_container_add (GTK_CONTAINER (expander_try_git_more), scrolledwindow_try_git_more);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_try_git_more), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_try_git_more), GTK_SHADOW_IN);

  textview_try_git_more = gtk_text_view_new ();
  gtk_widget_show (textview_try_git_more);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_try_git_more), textview_try_git_more);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview_try_git_more), FALSE);

  label_try_git_more = gtk_label_new_with_mnemonic ("_More information");
  gtk_widget_show (label_try_git_more);
  gtk_expander_set_label_widget (GTK_EXPANDER (expander_try_git_more), label_try_git_more);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox_try_git, "Trying out the content tracker");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox_try_git, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_try_git, false);

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


  /*
  
  // The remote repository.
  projectconfig->git_remote_repository_url_set(url_get());

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

  // If going to the page to try git, take the necessary actions.
  if (new_page_number == page_number_try_git) {
    if (!check_git_version_okay) {
      check_git_version_okay = check_git_version ();
    }
  }
  if (check_git_version_okay) {
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox_try_git, true);
  }
  
  // Return the new page.
  return new_page_number;
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
  textview_try_git_load ("Version check", spawn.standardout);
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
  }
  gtk_label_set_text(GTK_LABEL(label_try_git), versiontext.c_str());
  return okay;
}


void RemoteRepositoryAssistant::textview_try_git_load (const gchar* title, vector <ustring>& lines)
// Writes a title and lines of text to the textview for trying out git.
{
  GtkTextBuffer * buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview_try_git_more));
  gtk_text_buffer_insert_at_cursor (buffer, "===== ", -1);
  gtk_text_buffer_insert_at_cursor (buffer, title, -1);
  gtk_text_buffer_insert_at_cursor (buffer, " =====\n", -1);
  for (unsigned int i = 0; i < lines.size(); i++) {
    gtk_text_buffer_insert_at_cursor (buffer, lines[i].c_str(), -1);
    gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
  }
}


/*
Todo working on git assistant.


As of late we've seen versions of git that don't seem to behave well. 
Or, put in different words, that seem to be buggy. 
These versions of git failed when collaboration tasks were asked of these, such push / pull / and merging or conflict management.

It this continues to come up, it would be recommended to make a routine that tests all the relevant functions of git. 
This testing will be done when collaboration is set up. 
At present only the minimum version of git is checked, and if that's fine, then git passes. 
However, this check should be extended with all functions that are used during collaboration of two or more people.

Once it passes, it is okay, else it recommends to install from source. 

There is a problem with the git version that we use, so we need to install the right version from source, 
and also look into ways to find out whether the version of git installed is okay. 
There should be automated tests that do this on git-setup.

Perhaps the stock version of git that comes with a distro, should be used, but this was tried and failed. 

We really need to be able to use the stock version of git that comes with a distro.
It is not good manners to require the user to install git from source...

Once a version of git has been tested, the exact version number is stored 
in the general configuration, so that only in case a newer version of git is found,
a new test is required again. We should not stop any existing git remote repositories
to fail however, but only when the assistant is called, then a red flag is raised.





*/


