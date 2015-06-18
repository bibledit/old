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
#include "assistantchanges.h"
#include "help.h"
#include "settings.h"
#include "date_time_utils.h"
#include "projectutils.h"
#include "gtkwrappers.h"
#include "tiny_utilities.h"
#include "progresswindow.h"
#include "gwrappers.h"
#include "compareutils.h"
#include "snapshots.h"
#include "utilities.h"
#include <glib/gi18n.h>

ChangesAssistant::ChangesAssistant(WindowReferences * references_window) :
  AssistantBase(_("Changes"), _("changes"))
// Assistant for adding keyterms.
{
  my_references_window = references_window;

  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (assistant), GtkAssistantPageFunc (assistant_forward_function), gpointer(this), NULL);
  
  g_signal_connect (G_OBJECT (assistant), "apply", G_CALLBACK (on_assistant_apply_signal), gpointer(this));

  // Configurations.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Introduction.
  introduction (_("You can view the changes that have been made in the project."));

  // Ask user what he wants.
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  question_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), vbox1);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), vbox1, _("Which changes would you like to see?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), vbox1, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), vbox1, true);

  GSList *radiobutton_since_last_review_group = NULL;

  radiobutton_since_last_review = gtk_radio_button_new_with_mnemonic (NULL, _("The ones made since my last review"));
  gtk_widget_show (radiobutton_since_last_review);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_since_last_review, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_last_review), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_last_review));

  radiobutton_since_date = gtk_radio_button_new_with_mnemonic (NULL, _("The ones made since a certain date I will give"));
  gtk_widget_show (radiobutton_since_date);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_since_date, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_date), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_date));

  radiobutton_between_dates = gtk_radio_button_new_with_mnemonic (NULL, _("The ones made between two dates I will give"));
  gtk_widget_show (radiobutton_between_dates);
  gtk_box_pack_start (GTK_BOX (vbox1), radiobutton_between_dates, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_between_dates), radiobutton_since_last_review_group);
  radiobutton_since_last_review_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_between_dates));

  // Date last review page.
  label_last_review = gtk_label_new ("");
  gtk_widget_show (label_last_review);
  last_review_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_last_review);

  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_last_review, _("Information"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_last_review, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_last_review, true);

  // Show the date and time of last review. 
  // If that is uninitialized, take the date/time from the oldest revision in the Snapshots.
  unsigned int last_review_seconds = projectconfig->changes_last_review_get();
  if (last_review_seconds == 0) {
    last_review_seconds = snapshots_oldest_second (settings->genconfig.project_get());
    if (last_review_seconds == 0)
      last_review_seconds--;
    projectconfig->changes_last_review_set(last_review_seconds);
  }
  ustring last_review_label = _("Last review was on ") + date_time_seconds_human_readable(last_review_seconds, true);
  gtk_label_set_text(GTK_LABEL(label_last_review), last_review_label.c_str());

  // Date from.
  date_from_seconds = projectconfig->changes_since_get();
  if (date_from_seconds == 0) {
    date_from_seconds = snapshots_oldest_second (settings->genconfig.project_get());
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
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_to_object->hbox, _("Changes till which date?"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), date_to_object->hbox, GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), date_to_object->hbox, true);

  // Confirm.
  label_confirm = gtk_label_new (_("Ready for generating changes"));
  gtk_widget_show (label_confirm);
  confirm_page = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_confirm);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_confirm, _("Ready for generating changes"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_confirm, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_confirm, true);

  // Progress.
  label_progress = gtk_label_new (_("Generating changes..., please wait"));
  gtk_widget_show (label_progress);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_progress);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_progress, _("Generating changes"));
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label_progress, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label_progress, true);

  // Summary.
  label_summary = gtk_label_new (_("Done."));
  gtk_widget_show (label_summary);
  summary_page_number = gtk_assistant_append_page (GTK_ASSISTANT (assistant), label_summary);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label_summary, _("Ready"));
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

gint ChangesAssistant::assistant_forward_function (gint current_page, gpointer user_data)
{
  return ((ChangesAssistant *) user_data)->assistant_forward (current_page);
}

gint ChangesAssistant::assistant_forward (gint current_page)
{
  // Go to the right page from the question what to do, and set the gui.
  if (current_page == question_page) {
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date)))
      gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_from_object->hbox, _("Changes since which date?"));
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_between_dates)))
      gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), date_from_object->hbox, _("Changes from which date?"));
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
  
  // The actions. 
  ustring project = settings->genconfig.project_get();
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_last_review))) {
    temporal_from_project = copy_project_and_move_back_in_history (project, projectconfig->changes_last_review_get ());
    view_changes(project, temporal_from_project);
    // Set the date for the next review.  
    projectconfig->changes_last_review_set(date_time_seconds_get_current());
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date))) {
    temporal_from_project = copy_project_and_move_back_in_history (project, date_from_seconds);
    view_changes(project, temporal_from_project);
  }
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_between_dates))) {
    temporal_from_project = copy_project_and_move_back_in_history (project, date_from_seconds);
    temporal_to_project = copy_project_and_move_back_in_history (project, date_to_seconds);
    view_changes(temporal_to_project, temporal_from_project);
  }
  
  // Show summary.
  gtk_assistant_set_current_page (GTK_ASSISTANT (assistant), summary_page_number);
}


ustring ChangesAssistant::copy_project_and_move_back_in_history (const ustring& project, unsigned int second)
{
  // Progress.
  ProgressWindow progresswindow(_("Going back in history"), false);

  // Create a temporal history project.
  ustring copiedproject = project + _(" as it was on ") + date_time_seconds_human_readable (second, false);
  project_create_restore (copiedproject, "");

  // Go through all the books of the original project.
  vector <unsigned int> books = project_get_books (project);
  progresswindow.set_iterate (0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    progresswindow.iterate ();
    unsigned int book = books[bk];
    // Go through all the chapters of the book.
    vector <unsigned int> chapters = project_get_chapters (project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      unsigned int chapter = chapters[ch];
      // Store the chapter in the state as it was on the second given.
      unsigned int second_to_restore_to = 0;
      vector <unsigned int> seconds = snapshots_get_seconds (project, book, chapter);
      for (unsigned int s = 0; s < seconds.size(); s++) {
        if (second_to_restore_to == 0) {
          if (second >= seconds[s]) {
            second_to_restore_to = seconds[s];
          }
        }
      }
      if (second_to_restore_to) {
        ustring data = snapshots_get_chapter (project, book, chapter, second_to_restore_to);
        ParseLine parseline (data);
        CategorizeChapterVerse ccv(parseline.lines);
        project_store_chapter(copiedproject, book, ccv);
      }
    }
  }

  // Give result.
  return copiedproject;
}


void ChangesAssistant::view_changes(const ustring& current_stage_project, const ustring& previous_stage_project)
{
  compare_with(my_references_window, previous_stage_project, current_stage_project, true);
}


