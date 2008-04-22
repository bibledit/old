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
#include "dialogchanges.h"
#include "help.h"
#include "settings.h"
#include "shortcuts.h"
#include "dialogdate.h"
#include "date_time_utils.h"
#include "utilities.h"
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "unixwrappers.h"
#include "projectutils.h"
#include "compareutils.h"
#include "tiny_utilities.h"
#include "git.h"
#include "directories.h"


ChangesDialog::ChangesDialog (References * references)
{
  myreferences = references;
  
  Shortcuts shortcuts (0);

  changesdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (changesdialog), "Changes");
  gtk_window_set_position (GTK_WINDOW (changesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint (GTK_WINDOW (changesdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (changesdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  label2 = gtk_label_new ("This will show the changes that have been made in the project since a certain date and time.");
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label2, FALSE, FALSE, 0);
  gtk_label_set_line_wrap (GTK_LABEL (label2), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  hbox3 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox3, TRUE, TRUE, 0);

  GSList *radiobutton_since_last_group = NULL;
  
  radiobutton_since_last = gtk_radio_button_new_with_mnemonic (NULL, "View changes since last review done on");
  gtk_widget_show (radiobutton_since_last);
  gtk_box_pack_start (GTK_BOX (hbox3), radiobutton_since_last, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_last), radiobutton_since_last_group);
  radiobutton_since_last_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_last));

  shortcuts.button (radiobutton_since_last);

  label_last_review = gtk_label_new ("");
  gtk_widget_show (label_last_review);
  gtk_box_pack_start (GTK_BOX (hbox3), label_last_review, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_last_review), 0, 0.5);

  hbox5 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox5, FALSE, FALSE, 0);

  radiobutton_since_date = gtk_radio_button_new_with_mnemonic (NULL, "View changes since");
  gtk_widget_show (radiobutton_since_date);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton_since_date, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_since_date), radiobutton_since_last_group);
  radiobutton_since_last_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_since_date));

  shortcuts.button (radiobutton_since_date);

  label_since = gtk_label_new ("");
  gtk_widget_show (label_since);
  gtk_box_pack_start (GTK_BOX (hbox5), label_since, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_since), 0, 0.5);

  button_since = gtk_button_new ();
  gtk_widget_show (button_since);
  gtk_box_pack_start (GTK_BOX (hbox5), button_since, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_since), alignment2);

  hbox6 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox6);

  image2 = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox6), image2, FALSE, FALSE, 0);

  label6 = gtk_label_new_with_mnemonic ("Set");
  gtk_widget_show (label6);
  gtk_box_pack_start (GTK_BOX (hbox6), label6, FALSE, FALSE, 0);

  shortcuts.label (label6);

  dialog_action_area1 = GTK_DIALOG (changesdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (changesdialog, &shortcuts, "changes");
  
  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (changesdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (cancelbutton1);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (changesdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (okbutton1);
  
  shortcuts.process();

  g_signal_connect ((gpointer) radiobutton_since_last, "toggled", G_CALLBACK (on_radiobutton_toggled), gpointer(this));
  g_signal_connect ((gpointer) radiobutton_since_date, "toggled", G_CALLBACK (on_radiobutton_toggled), gpointer(this));
  g_signal_connect ((gpointer) button_since, "clicked", G_CALLBACK (on_button_since_clicked), gpointer(this));
  g_signal_connect ((gpointer) okbutton1, "clicked", G_CALLBACK (on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus (okbutton1);
  gtk_widget_grab_default (okbutton1);
  
  gui ();
}


ChangesDialog::~ChangesDialog ()
{
  // Delete project that has the history.
  if (!temporal_project.empty()) {
    project_delete (temporal_project);
  }
  // Destroy the dialog.
  gtk_widget_destroy (changesdialog);
}


int ChangesDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (changesdialog));
}



void ChangesDialog::on_radiobutton_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((ChangesDialog *) user_data)->gui ();
}


void ChangesDialog::on_button_since_clicked (GtkButton *button, gpointer user_data)
{
  ((ChangesDialog *) user_data)->on_button_since ();
}


void ChangesDialog::on_button_since ()
{
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
  guint32 seconds = projectconfig->changes_since_get ();
  DateDialog dialog (&seconds, true);
  if (dialog.run () == GTK_RESPONSE_OK) {
    projectconfig->changes_since_set (seconds);
    gui ();
  }
}


void ChangesDialog::on_okbutton1_clicked (GtkButton *button, gpointer user_data)
{
  ((ChangesDialog *) user_data)->on_okbutton ();
}


void ChangesDialog::on_okbutton ()
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_last)))
    view_local_changes (true);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date)))
    view_local_changes (false);
}


void ChangesDialog::gui ()
{
  // Configurations.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
  // Seconds.
  unsigned int seconds;
  // Fetch seconds since last review. 
  // If that is zero, that means, uninitialized, take the date/time from
  // the first revision in the git repository.
  seconds = projectconfig->changes_last_review_get ();
  if (seconds == 0) {
    seconds = git_log_date_time_at_revision (settings->genconfig.project_get(), 1);
    seconds--;
    projectconfig->changes_last_review_set (seconds);
  }
  // Set label to the human readable date and time.
  ustring s;
  s = date_time_seconds_human_readable (seconds, true);
  gtk_label_set_text (GTK_LABEL (label_last_review), s.c_str());
  // Same story for the seconds to view the differences since.
  seconds = projectconfig->changes_since_get ();
  if (seconds == 0) {
    seconds = git_log_date_time_at_revision (settings->genconfig.project_get(), 1);
    seconds--;
    projectconfig->changes_since_set (seconds);
  }
  s = date_time_seconds_human_readable (seconds, true);
  gtk_label_set_text (GTK_LABEL (label_since), s.c_str());
  // Sensitivity of buttons.
  bool changes_since_date = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (radiobutton_since_date));
  gtk_widget_set_sensitive (button_since, changes_since_date);
}


void ChangesDialog::view_local_changes (bool changes_since_last_review)
{
  // Configuration.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
 
  // Retrieve the seconds to check the changes since.
  unsigned int second = projectconfig->changes_since_get ();
  if (changes_since_last_review)
    second = projectconfig->changes_last_review_get ();

  // Copy the current project to the temporal history project.
  temporal_project = settings->genconfig.project_get() + "_as_it_was_at_" + convert_to_string (second);
  {
    ProgressWindow progresswindow ("Copying project", false);
    progresswindow.set_fraction (0.5);
    project_copy (settings->genconfig.project_get(), temporal_project);
  }
  
  // The data directories to work in.
  ustring history_project_data_directory = project_data_directory_project (temporal_project);
  ustring project_data_directory = project_data_directory_project (settings->genconfig.project_get());

  // Retrieve the name of the first commit since or at the date and time given.
  vector <ustring> commits;
  vector <unsigned int> seconds;
  git_log_read (project_data_directory, commits, seconds, "");
  // Note if the date and time are older than the project's oldest commit.
  bool date_time_older_than_project = false;
  ustring commit;
  if (!commits.empty ()) {
    unsigned int most_recent_second = seconds[0];
    // If the date and time are more recent than the most recent commit,
    // it doesn't pick a commit, as there are no changes recorded.
    if (second <= most_recent_second) {
      commit = git_log_pick_commit_at_date_time (commits, seconds, second);
    }
    unsigned int oldest_second = seconds[seconds.size () - 1];
    if (second < oldest_second) {
      date_time_older_than_project = true;
      commit = commits[0];
    }
  }
  
  // If there are no changes recorded since that date and time, give a message and bail out.
  if (commit.empty ()) {
    gtkw_dialog_info (changesdialog, "No changes have been recorded since that time");
    return;
  }

  // Check the revision out.
  GwSpawn spawn ("git-checkout");
  spawn.workingdirectory (history_project_data_directory);
  spawn.arg ("-b");
  spawn.arg ("bibleditcomparison");
  spawn.arg (commit);
  spawn.progress ("Retrieving data from history", false);
  spawn.run ();
  if (spawn.exitstatus != 0) {
    gtkw_dialog_error (changesdialog, "Failed to retrieve history");
  }

  // If the commit is older than the project, clear the temporal project.
  if (date_time_older_than_project) {
    vector <unsigned int> books = project_get_books (temporal_project);
    for (unsigned int i = 0; i < books.size (); i++) {
      project_remove_book (temporal_project, books[i]);
    }
  }
  
  // Run comparison.
  compare_with (myreferences, settings->genconfig.project_get(), temporal_project, true);
  
  // If reviewing changes since last review, set the date for the next review.  
  if (changes_since_last_review) {
    projectconfig->changes_last_review_set (date_time_seconds_get_current ());    
  }  
}
