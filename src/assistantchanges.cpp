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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "libraries.h"
#include <glib.h>
#include "assistantchanges.h"
#include "help.h"
#include "settings.h"
#include "git.h"
#include "date_time_utils.h"
#include "projectutils.h"
#include "gtkwrappers.h"
#include "tiny_utilities.h"
#include "progresswindow.h"
#include "gwrappers.h"
#include "compareutils.h"


ChangesAssistant::ChangesAssistant(References * references) :
AssistantBase("Changes")
// Assistant for adding keyterms.
{
  myreferences = references;

  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));

  // Configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Introduction.
  introduction ("You can view the changes that have been made in the project.");

  // Ask user what he wants.
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  question_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox1);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox1, "Which changes would you like to see?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox1, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox1, true);

  GSList *radiobutton_since_last_review_group = NULL;

  radiobutton_since_last_review = gtk_radio_button_new_with_mnemonic (NULL, "The ones made since my last review");
  gtk_widget_show (radiobutton_since_last_review);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_since_last_review, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_last_review), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_last_review));

  radiobutton_since_date = gtk_radio_button_new_with_mnemonic (NULL, "The ones made since a certain date I will give");
  gtk_widget_show (radiobutton_since_date);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_since_date, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_date), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_date));

  radiobutton_between_dates = gtk_radio_button_new_with_mnemonic (NULL, "The ones made between two dates I will give");
  gtk_widget_show (radiobutton_between_dates);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_between_dates, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_between_dates), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_between_dates));

  // Date last review page.
  label_last_review = gtk_label_new ("");
  gtk_widget_show (label_last_review);
  last_review_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_last_review);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_last_review, "Information");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_last_review, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_last_review, true);

  // Show the date and time of last review. 
  // If that is uninitialized, take the date/time from the oldest revision in the git repository.
  unsigned int last_review_seconds = projectconfig->changes_last_review_get();
  if (last_review_seconds == 0) {
    last_review_seconds = git_oldest_commit(settings->genconfig.project_get());
    if (last_review_seconds == 0)
      last_review_seconds--;
    projectconfig->changes_last_review_set(last_review_seconds);
  }
  ustring last_review_label = "Last review was on " + date_time_seconds_human_readable(last_review_seconds, true);
  gtk_label_set_text(GTK_LABEL(label_last_review), last_review_label.c_str());

  // Date from.
  date_from_seconds = projectconfig->changes_since_get();
  if (date_from_seconds == 0) {
    date_from_seconds = git_oldest_commit(settings->genconfig.project_get());
    if (date_from_seconds == 0)
      date_from_seconds--;
    projectconfig->changes_since_set(date_from_seconds);
  }
  date_from_object = new DateWidget (&date_from_seconds, true);
  date_from_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), date_from_object->hbox);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_from_object->hbox, "");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), date_from_object->hbox, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), date_from_object->hbox, true);

  // Date to.
  date_to_seconds = projectconfig->changes_till_get();
  if (date_to_seconds == 0) {
    date_to_seconds = date_time_seconds_get_current();
    projectconfig->changes_till_set(date_to_seconds);
  }
  date_to_object = new DateWidget (&date_to_seconds, true);
  date_to_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), date_to_object->hbox);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_to_object->hbox, "Changes till which date?");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), date_to_object->hbox, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), date_to_object->hbox, true);

  // Confirm.
  label_confirm = gtk_label_new ("Ready for generating changes");
  gtk_widget_show (label_confirm);
  confirm_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, "Ready for generating changes");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);

  // Progress.
  label_progress = gtk_label_new ("Generating changes..., please wait");
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, "Generating changes");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);

  // Summary.
  label_summary = gtk_label_new ("Done.");
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, "Ready");
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_summary, GTK_ASSISTANT_PAGE_SUMMARY);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_summary, true);
  
  // Finish assistant.
  gtk_widget_show_all (assistant);
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), 0);
}

ChangesAssistant::~ChangesAssistant()
{
  delete date_from_object;
  delete date_to_object;
  if (!temporal_from_project.empty()) {
    project_delete(temporal_from_project);
  }
  if (!temporal_to_project.empty()) {
    project_delete(temporal_to_project);
  }
}

void ChangesAssistant::on_assistant_apply_signal (GtkAssistant *assistant, gpointer user_data)
{
  ((ChangesAssistant *) user_data)->on_assistant_apply();
}


void ChangesAssistant::on_assistant_apply ()
{
  // Configuration.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Save times.
  projectconfig->changes_since_set(date_from_seconds);
  projectconfig->changes_till_set(date_to_seconds);
  
  
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_last_review))) {
    view_local_changes(true);
    projectconfig->changes_last_review_set(date_time_seconds_get_current());
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date))) {
    view_local_changes(false);
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_between_dates))) {
    gtkw_dialog_error (NULL, "Not yet implemented"); // Todo
  }
  

  
  
  
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
  
}


gint ChangesAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ChangesAssistant *) user_data)->assistant_forward (current_page);
}

gint ChangesAssistant::assistant_forward (gint current_page)
{
  // Go to the right page from the question what to do, and set the gui.
  if (current_page == question_page) {
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date)))
      gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_from_object->hbox, "Changes since which date?");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_between_dates)))
      gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_from_object->hbox, "Changes from which date?");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_last_review)))
      return last_review_page;
    else 
      return date_from_page;
  }
  
  // If reviewing since last change, go straight to the confirmation page.
  if (current_page == last_review_page) {
    return confirm_page;
  }

  // If on the date-from/since, decide whether to ask for the date-to,
  // and ensure that the date-to is more recent.
  if (current_page == date_from_page) {
    if (date_to_seconds < date_from_seconds) {
      date_to_seconds = date_from_seconds;
      date_to_object->set_date();
    }
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date)))
      return confirm_page;
  }
    
  // Default behaviour is to go to the next page.
  return ++current_page;
}


void ChangesAssistant::view_local_changes(bool changes_since_last_review)
{
  // Configuration.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Retrieve the seconds to check the changes since.
  unsigned int second = projectconfig->changes_since_get();
  if (changes_since_last_review)
    second = projectconfig->changes_last_review_get();

  // Copy the current project to the temporal history project.
  temporal_from_project = settings->genconfig.project_get() + "_as_it_was_at_" + convert_to_string(second);
  {
    ProgressWindow progresswindow("Copying project", false);
    progresswindow.set_fraction(0.5);
    project_copy(settings->genconfig.project_get(), temporal_from_project);
  }

  // The data directories to work in.
  ustring history_project_data_directory = project_data_directory_project(temporal_from_project);
  ustring project_data_directory = project_data_directory_project(settings->genconfig.project_get());

  // Retrieve the name of the first commit since or at the date and time given.
  vector < ustring > commits;
  vector < unsigned int >seconds;
  git_log_read(project_data_directory, commits, seconds, "");
  // Note if the date and time are older than the project's oldest commit.
  bool date_time_older_than_project = false;
  ustring commit;
  if (!commits.empty()) {
    unsigned int most_recent_second = seconds[0];
    // If the date and time are more recent than the most recent commit,
    // it doesn't pick a commit, as there are no changes recorded.
    if (second <= most_recent_second) {
      commit = git_log_pick_commit_at_date_time(commits, seconds, second);
    }
    unsigned int oldest_second = seconds[seconds.size() - 1];
    if (second < oldest_second) {
      date_time_older_than_project = true;
      commit = commits[0];
    }
  }
  // If there are no changes recorded since that date and time, give a message and bail out.
  if (commit.empty()) {
    gtkw_dialog_info(assistant, "No changes have been recorded since that time");
  } else {

    // Changes were recorded.
    // Check the revision out.

    GwSpawn spawn("git-checkout");
    spawn.workingdirectory(history_project_data_directory);
    spawn.arg("-b");
    spawn.arg("bibleditcomparison");
    spawn.arg(commit);
    spawn.progress("Retrieving data from history", false);
    spawn.run();
    if (spawn.exitstatus != 0) {
      gtkw_dialog_error(assistant, "Failed to retrieve history");
    }
    // If the commit is older than the project, clear the temporal project.
    if (date_time_older_than_project) {
      vector < unsigned int >books = project_get_books(temporal_from_project);
      for (unsigned int i = 0; i < books.size(); i++) {
        project_remove_book(temporal_from_project, books[i]);
      }
    }
    // Run comparison.
    compare_with(myreferences, settings->genconfig.project_get(), temporal_from_project, true);

  }

  // If reviewing changes since last review, set the date for the next review.  
  if (changes_since_last_review) {
    projectconfig->changes_last_review_set(date_time_seconds_get_current());
  }
}

