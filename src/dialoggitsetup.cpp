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
#include "dialoggitsetup.h"
#include "utilities.h"
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "unixwrappers.h"
#include "directories.h"
#include "shell.h"
#include "settings.h"
#include "gui.h"
#include "progresswindow.h"
#include "help.h"
#include "shortcuts.h"
#include "projectutils.h"
#include "books.h"
#include "tiny_utilities.h"
#include "git.h"

GitSetupDialog::GitSetupDialog(int dummy) {
  Shortcuts shortcuts(0);

  action_copy = false;
  action_sync = false;

  // Build GUI.
  gitsetupdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW (gitsetupdialog), "Git Remote Repository Setup");
  gtk_window_set_position(GTK_WINDOW (gitsetupdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW (gitsetupdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (gitsetupdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  checkbutton_remote = gtk_check_button_new_with_mnemonic("Use remote repository");
  gtk_widget_show(checkbutton_remote);
  gtk_box_pack_start(GTK_BOX (vbox1), checkbutton_remote, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_remote);

  hbox20 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox20);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox20, TRUE, TRUE, 0);

  label_path = gtk_label_new("Repository");
  gtk_widget_show(label_path);
  gtk_box_pack_start(GTK_BOX (hbox20), label_path, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_path), 0, 0.5);

  shortcuts.label(label_path);

  entry_path = gtk_entry_new();
  gtk_widget_show(entry_path);
  gtk_box_pack_start(GTK_BOX (hbox20), entry_path, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char(GTK_ENTRY (entry_path), 8226);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 0);

  hbox25 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox25);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox25, TRUE, TRUE, 0);

  image_version = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_version);
  gtk_box_pack_start(GTK_BOX (hbox25), image_version, FALSE, TRUE, 0);

  label_version_done = gtk_label_new("");
  gtk_widget_show(label_version_done);
  gtk_box_pack_start(GTK_BOX (hbox25), label_version_done, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_version_done), 0, 0.5);

  label_version_info = gtk_label_new("");
  gtk_widget_show(label_version_info);
  gtk_box_pack_start(GTK_BOX (hbox25), label_version_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_version_info), 0, 0.5);

  hseparator9 = gtk_hseparator_new();
  gtk_widget_show(hseparator9);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator9, TRUE, TRUE, 0);

  hbox21 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox21);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox21, TRUE, TRUE, 0);

  image_tasks_running_ok = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_tasks_running_ok);
  gtk_box_pack_start(GTK_BOX (hbox21), image_tasks_running_ok, FALSE, TRUE, 0);

  label_tasks_running_ok = gtk_label_new("");
  gtk_widget_show(label_tasks_running_ok);
  gtk_box_pack_start(GTK_BOX (hbox21), label_tasks_running_ok, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_tasks_running_ok), 0, 0.5);

  label_tasks_running_info = gtk_label_new("");
  gtk_widget_show(label_tasks_running_info);
  gtk_box_pack_start(GTK_BOX (hbox21), label_tasks_running_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_tasks_running_info), 0, 0.5);

  hseparator5 = gtk_hseparator_new();
  gtk_widget_show(hseparator5);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator5, TRUE, TRUE, 0);

  hbox22 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox22);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox22, TRUE, TRUE, 0);

  image_repository_accessible_ok = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_repository_accessible_ok);
  gtk_box_pack_start(GTK_BOX (hbox22), image_repository_accessible_ok, FALSE, TRUE, 0);

  label_repository_accessible_done = gtk_label_new("");
  gtk_widget_show(label_repository_accessible_done);
  gtk_box_pack_start(GTK_BOX (hbox22), label_repository_accessible_done, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_repository_accessible_done), 0, 0.5);

  label_repository_accessible_info = gtk_label_new("");
  gtk_widget_show(label_repository_accessible_info);
  gtk_box_pack_start(GTK_BOX (hbox22), label_repository_accessible_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_repository_accessible_info), 0, 0.5);

  label_repository_accessible_output = gtk_label_new("");
  gtk_widget_show(label_repository_accessible_output);
  gtk_box_pack_start(GTK_BOX (vbox1), label_repository_accessible_output, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_repository_accessible_output), 0, 0.5);

  hseparator6 = gtk_hseparator_new();
  gtk_widget_show(hseparator6);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator6, TRUE, TRUE, 0);

  hbox23 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox23);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox23, TRUE, TRUE, 0);

  image_repository_writable = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_repository_writable);
  gtk_box_pack_start(GTK_BOX (hbox23), image_repository_writable, FALSE, TRUE, 0);

  label_repository_writable_done = gtk_label_new("");
  gtk_widget_show(label_repository_writable_done);
  gtk_box_pack_start(GTK_BOX (hbox23), label_repository_writable_done, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_repository_writable_done), 0, 0.5);

  label_repository_writable_info = gtk_label_new("");
  gtk_widget_show(label_repository_writable_info);
  gtk_box_pack_start(GTK_BOX (hbox23), label_repository_writable_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_repository_writable_info), 0, 0.5);

  label_repository_writable_output = gtk_label_new("");
  gtk_widget_show(label_repository_writable_output);
  gtk_box_pack_start(GTK_BOX (vbox1), label_repository_writable_output, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_repository_writable_output), 0, 0.5);

  hseparator7 = gtk_hseparator_new();
  gtk_widget_show(hseparator7);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator7, TRUE, TRUE, 0);

  hbox28 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox28);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox28, TRUE, TRUE, 0);

  image_action = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_action);
  gtk_box_pack_start(GTK_BOX (hbox28), image_action, FALSE, TRUE, 0);

  label_action_done = gtk_label_new("");
  gtk_widget_show(label_action_done);
  gtk_box_pack_start(GTK_BOX (hbox28), label_action_done, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_action_done), 0, 0.5);

  label_action_info = gtk_label_new("");
  gtk_widget_show(label_action_info);
  gtk_box_pack_start(GTK_BOX (hbox28), label_action_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_action_info), 0, 0.5);

  vbox5 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox5);
  gtk_box_pack_start(GTK_BOX (hbox28), vbox5, TRUE, TRUE, 0);

  button_copy = gtk_button_new_with_mnemonic("Copy repository to local data now");
  gtk_widget_show(button_copy);
  gtk_box_pack_start(GTK_BOX (vbox5), button_copy, FALSE, FALSE, 0);

  shortcuts.button(button_copy);

  button_synchronize = gtk_button_new_with_mnemonic("Synchronize local data with repository now");
  gtk_widget_show(button_synchronize);
  gtk_box_pack_start(GTK_BOX (vbox5), button_synchronize, FALSE, FALSE, 0);

  shortcuts.button(button_synchronize);

  hseparator11 = gtk_hseparator_new();
  gtk_widget_show(hseparator11);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator11, TRUE, TRUE, 0);

  hbox24 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox24);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox24, TRUE, TRUE, 0);

  image_pull = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_pull);
  gtk_box_pack_start(GTK_BOX (hbox24), image_pull, FALSE, TRUE, 0);

  label_pull_done = gtk_label_new("Done");
  gtk_widget_show(label_pull_done);
  gtk_box_pack_start(GTK_BOX (hbox24), label_pull_done, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_pull_done), 0, 0.5);

  label_pull_info = gtk_label_new("Changes pulled");
  gtk_widget_show(label_pull_info);
  gtk_box_pack_start(GTK_BOX (hbox24), label_pull_info, TRUE, TRUE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_pull_info), 0, 0.5);

  label_pull_output = gtk_label_new("Repository pull output");
  gtk_widget_show(label_pull_output);
  gtk_box_pack_start(GTK_BOX (vbox1), label_pull_output, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_pull_output), 0, 0.5);

  hseparator8 = gtk_hseparator_new();
  gtk_widget_show(hseparator8);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator8, TRUE, TRUE, 0);

  hbox27 = gtk_hbox_new(FALSE, 6);
  gtk_widget_show(hbox27);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox27, TRUE, TRUE, 0);

  image_push = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_push);
  gtk_box_pack_start(GTK_BOX (hbox27), image_push, FALSE, TRUE, 0);

  label_push_done = gtk_label_new("");
  gtk_widget_show(label_push_done);
  gtk_box_pack_start(GTK_BOX (hbox27), label_push_done, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_push_done), 0, 0.5);

  label_push_info = gtk_label_new("");
  gtk_widget_show(label_push_info);
  gtk_box_pack_start(GTK_BOX (hbox27), label_push_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_push_info), 0, 0.5);

  label_push_output = gtk_label_new("");
  gtk_widget_show(label_push_output);
  gtk_box_pack_start(GTK_BOX (vbox1), label_push_output, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label_push_output), 0, 0.5);

  hseparator10 = gtk_hseparator_new();
  gtk_widget_show(hseparator10);
  gtk_box_pack_start(GTK_BOX (vbox1), hseparator10, TRUE, TRUE, 0);

  hbox19 = gtk_hbox_new(FALSE, 5);
  gtk_widget_show(hbox19);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox19, TRUE, TRUE, 0);

  label_pull_push = gtk_label_new("Synchronize data with the repository every");
  gtk_widget_show(label_pull_push);
  gtk_box_pack_start(GTK_BOX (hbox19), label_pull_push, FALSE, FALSE, 0);

  shortcuts.label(label_pull_push);

  spinbutton_update_adj = gtk_adjustment_new(5, 5, 3600, 1, 10, 10);
  spinbutton_update = gtk_spin_button_new(GTK_ADJUSTMENT (spinbutton_update_adj), 1, 0);
  gtk_widget_show(spinbutton_update);
  gtk_box_pack_start(GTK_BOX (hbox19), spinbutton_update, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON (spinbutton_update), TRUE);

  label55 = gtk_label_new("seconds");
  gtk_widget_show(label55);
  gtk_box_pack_start(GTK_BOX (hbox19), label55, FALSE, FALSE, 0);

  vbox4 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox4);
  gtk_box_pack_start(GTK_BOX (vbox1), vbox4, TRUE, TRUE, 0);

  label53 = gtk_label_new("While pulling changes, if there is a conflict between the data on the server and the local data");
  gtk_widget_show(label53);
  gtk_box_pack_start(GTK_BOX (vbox4), label53, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC (label53), 0, 0.5);

  GSList *radiobutton_conflict_me_group= NULL;

  radiobutton_conflict_me = gtk_radio_button_new_with_mnemonic(NULL, "take my data");
  gtk_widget_show(radiobutton_conflict_me);
  gtk_box_pack_start(GTK_BOX (vbox4), radiobutton_conflict_me, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON (radiobutton_conflict_me), radiobutton_conflict_me_group);
  radiobutton_conflict_me_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON (radiobutton_conflict_me));

  shortcuts.button(radiobutton_conflict_me);

  radiobutton_conflict_server = gtk_radio_button_new_with_mnemonic(NULL, "take the server's data");
  gtk_widget_show(radiobutton_conflict_server);
  gtk_box_pack_start(GTK_BOX (vbox4), radiobutton_conflict_server, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON (radiobutton_conflict_server), radiobutton_conflict_me_group);
  radiobutton_conflict_me_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON (radiobutton_conflict_server));

  shortcuts.button(radiobutton_conflict_server);

  dialog_action_area1 = GTK_DIALOG (gitsetupdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (gitsetupdialog, &shortcuts, "git_setup");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG (gitsetupdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG (gitsetupdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(okbutton);

  shortcuts.process();

  g_signal_connect ((gpointer) checkbutton_remote, "toggled",
      G_CALLBACK (on_checkbutton_remote_toggled),
      gpointer(this));
  g_signal_connect ((gpointer) entry_path, "changed",
      G_CALLBACK (on_entry_changed),
      gpointer(this));
  g_signal_connect ((gpointer) button_copy, "clicked",
      G_CALLBACK (on_button_action_clicked),
      gpointer(this));
  g_signal_connect ((gpointer) button_synchronize, "clicked",
      G_CALLBACK (on_button_action_clicked),
      gpointer(this));
  g_signal_connect ((gpointer) okbutton, "clicked",
      G_CALLBACK (on_okbutton_clicked),
      gpointer(this));

  gtk_widget_grab_default(okbutton);
  gtk_label_set_mnemonic_widget(GTK_LABEL (label_path), entry_path);
  gtk_label_set_mnemonic_widget(GTK_LABEL (label_pull_push), spinbutton_update);

  // Initialize object's variables.
  event_id_gui = 0;
  project_tasks_count = 1;
  previous_project_tasks_count = 0;
  version_okay = false;

  // Version information.
  GwSpawn spawn("git");
  spawn.arg("--version");
  spawn.read();
  spawn.run();
  ustring versiontext;
  if (!spawn.standardout.empty())
    versiontext = spawn.standardout[0];
  Parse parse1(versiontext, false);
  if (parse1.words.size() >= 3) {
    Parse parse2(parse1.words[2], false, ".");
    if (parse2.words.size() >= 3) {
      unsigned int version = 100 * convert_to_int(parse2.words[0]) + 10 * convert_to_int(parse2.words[1]) + convert_to_int(parse2.words[2]);
      if (version >= 150)
        version_okay = true;
    }
  }
  gui_okay(image_version, label_version_done, version_okay);
  if (!version_okay) {
    versiontext.append(", but should be 1.5.0 or higher");
  }
  gtk_label_set_text(GTK_LABEL (label_version_info), versiontext.c_str ());

  // Project configuration.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig(settings->genconfig.project_get());

  // Remote update interval.
  gtk_spin_button_set_value(GTK_SPIN_BUTTON (spinbutton_update), double (projectconfig->git_remote_update_interval_get ()));

  // Set values for repository.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (checkbutton_remote), projectconfig->git_use_remote_repository_get ());
  gtk_entry_set_text(GTK_ENTRY (entry_path), projectconfig->git_remote_repository_url_get ().c_str());

  // Set conflict handling values.
  GitConflictHandlingType conflicthandling = (GitConflictHandlingType) projectconfig->git_remote_repository_conflict_handling_get();
  switch (conflicthandling)
  {
    case gchtTakeMe:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radiobutton_conflict_me), true);
      break;
    }
    case gchtTakeServer:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (radiobutton_conflict_server), true);
      break;
    }
  }

  // Gui update.
  event_id_tasks_ready = g_timeout_add_full(G_PRIORITY_DEFAULT, 600, GSourceFunc (tasks_counter_timeout), gpointer(this), NULL);
  tasks_counter();
  set_gui();
}

GitSetupDialog::~GitSetupDialog() {
  // Resume Git tasks.
  git_command_pause(false);

  // Destroy event sources.
  gw_destroy_source(event_id_tasks_ready);
  gw_destroy_source(event_id_gui);

  // Destroy widget.
  gtk_widget_destroy(gitsetupdialog);
}

int GitSetupDialog::run() {
  return gtk_dialog_run(GTK_DIALOG (gitsetupdialog));
}

void GitSetupDialog::on_checkbutton_remote_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
  ((GitSetupDialog *) user_data)->on_checkbutton_remote();
}

void GitSetupDialog::on_checkbutton_remote() {
  set_gui();
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (checkbutton_remote)))
    gtk_widget_grab_focus(entry_path);
}

void GitSetupDialog::on_entry_changed(GtkEditable *editable, gpointer user_data) {
  ((GitSetupDialog *) user_data)->on_entry();
}

void GitSetupDialog::on_entry() {
  gw_destroy_source(event_id_gui);
  event_id_gui = g_timeout_add_full(G_PRIORITY_DEFAULT, 600, GSourceFunc (delayer), gpointer(this), NULL);
}

bool GitSetupDialog::delayer(gpointer user_data) {
  ((GitSetupDialog *) user_data)->delayed();
  return false;
}

void GitSetupDialog::delayed() {
  set_gui();
}

void GitSetupDialog::set_gui() {
  // Clear gui.
  gui_okay(image_tasks_running_ok, label_tasks_running_ok, false);
  gtk_label_set_text(GTK_LABEL (label_tasks_running_info), "Running tasks check");
  gui_okay(image_repository_accessible_ok, label_repository_accessible_done, false);
  gtk_label_set_text(GTK_LABEL (label_repository_accessible_info), "Read access check");
  gtk_label_set_text(GTK_LABEL (label_repository_accessible_output), "");
  gui_okay(image_repository_writable, label_repository_writable_done, false);
  gtk_label_set_text(GTK_LABEL (label_repository_writable_info), "Write access check");
  gtk_label_set_text(GTK_LABEL (label_repository_writable_output), "");
  gui_okay(image_action, label_action_done, false);
  gtk_label_set_text(GTK_LABEL (label_action_info), "Select action");
  if (action_copy || action_sync) {
    gtk_widget_show(image_pull);
    gtk_widget_show(label_pull_done);
    gtk_widget_show(label_pull_info);
    gtk_widget_show(label_pull_output);
    gtk_widget_show(hseparator8);
  } else {
    gtk_widget_hide(image_pull);
    gtk_widget_hide(label_pull_done);
    gtk_widget_hide(label_pull_info);
    gtk_widget_hide(label_pull_output);
    gtk_widget_hide(hseparator8);
  }
  if (action_sync) {
    gtk_widget_show(image_push);
    gtk_widget_show(label_push_done);
    gtk_widget_show(label_push_info);
    gtk_widget_show(label_push_output);
    gtk_widget_show(hseparator10);
  } else {
    gtk_widget_hide(image_push);
    gtk_widget_hide(label_push_done);
    gtk_widget_hide(label_push_info);
    gtk_widget_hide(label_push_output);
    gtk_widget_hide(hseparator10);
  }
  gui_okay(image_pull, label_pull_done, false);
  if (action_sync)
    gtk_label_set_text(GTK_LABEL (label_pull_info), "Pull changes check");
  if (action_copy)
    gtk_label_set_text(GTK_LABEL (label_pull_info), "Copy project check");
  gtk_label_set_text(GTK_LABEL (label_pull_output), "");
  gui_okay(image_push, label_push_done, false);
  gtk_label_set_text(GTK_LABEL (label_push_info), "Push changes check");
  gtk_label_set_text(GTK_LABEL (label_push_output), "");
  gtk_widget_set_sensitive(okbutton, false);

  // Use remote repository?
  bool use_repository = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (checkbutton_remote));

  // Set widgets sensitive depending on whether the repository is used.
  gtk_widget_set_sensitive(label_path, use_repository);
  gtk_widget_set_sensitive(entry_path, use_repository);
  gtk_widget_set_sensitive(image_version, use_repository);
  gtk_widget_set_sensitive(label_version_done, use_repository);
  gtk_widget_set_sensitive(label_version_info, use_repository);
  gtk_widget_set_sensitive(image_tasks_running_ok, use_repository);
  gtk_widget_set_sensitive(label_tasks_running_ok, use_repository);
  gtk_widget_set_sensitive(label_tasks_running_info, use_repository);
  gtk_widget_set_sensitive(image_repository_accessible_ok, use_repository);
  gtk_widget_set_sensitive(label_repository_accessible_done, use_repository);
  gtk_widget_set_sensitive(label_repository_accessible_info, use_repository);
  gtk_widget_set_sensitive(label_repository_accessible_output, use_repository);
  gtk_widget_set_sensitive(image_repository_writable, use_repository);
  gtk_widget_set_sensitive(label_repository_writable_done, use_repository);
  gtk_widget_set_sensitive(label_repository_writable_info, use_repository);
  gtk_widget_set_sensitive(label_repository_writable_output, use_repository);
  gtk_widget_set_sensitive(image_pull, use_repository);
  gtk_widget_set_sensitive(label_pull_done, use_repository);
  gtk_widget_set_sensitive(label_pull_info, use_repository);
  gtk_widget_set_sensitive(label_pull_output, use_repository);
  gtk_widget_set_sensitive(image_push, use_repository);
  gtk_widget_set_sensitive(label_push_done, use_repository);
  gtk_widget_set_sensitive(label_push_info, use_repository);
  gtk_widget_set_sensitive(label_push_output, use_repository);
  gtk_widget_set_sensitive(label53, use_repository);
  gtk_widget_set_sensitive(radiobutton_conflict_me, use_repository);
  gtk_widget_set_sensitive(radiobutton_conflict_server, use_repository);
  gtk_widget_set_sensitive(label_pull_push, use_repository);
  gtk_widget_set_sensitive(spinbutton_update, use_repository);
  gtk_widget_set_sensitive(label55, use_repository);
  gtk_widget_set_sensitive(image_action, use_repository);
  gtk_widget_set_sensitive(label_action_done, use_repository);
  gtk_widget_set_sensitive(label_action_info, use_repository);
  gtk_widget_set_sensitive(button_copy, use_repository);
  gtk_widget_set_sensitive(button_synchronize, use_repository);

  // If no repository used, set gui and bail out.
  if (!use_repository) {
    gtk_widget_set_sensitive(okbutton, true);
    return;
  }

  // If the version is not right, bail out.
  if (!version_okay)
    return;

  // If the repository was not changed, set the OK button sensitive.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  ProjectConfiguration * projectconfig = settings->projectconfig(project);
  if (url_get() == projectconfig->git_remote_repository_url_get())
    gtk_widget_set_sensitive(okbutton, true);

  // Set gui for the amount of tasks pending for the repository path.
  bool pending_tasks_okay = project_tasks_count == 0;
  gui_okay(image_tasks_running_ok, label_tasks_running_ok, pending_tasks_okay);
  ustring tasks_pending_message;
  if (pending_tasks_okay) {
    tasks_pending_message = "No pending tasks remaining";
  } else {
    tasks_pending_message = "Waiting till the " + convert_to_string(project_tasks_count) + " pending tasks are done";
  }
  gtk_label_set_text(GTK_LABEL (label_tasks_running_info), tasks_pending_message.c_str());

  // Pause processing git tasks if there are no pending tasks for this project.
  // This is because tasks will be produced, but these tasks are unwanted.
  // If for example a local book will be deleted, then it will be deleted 
  // from the remote repository too, which is unwanted behaviour.
  if (pending_tasks_okay)
    git_command_pause(true);

  // If there are still pending tasks, bail out.
  if (!pending_tasks_okay)
    return;

  // Test read access to the repository.
  ustring read_access_elaborate_info;
  bool read_access = test_read_access(read_access_elaborate_info);
  gui_okay(image_repository_accessible_ok, label_repository_accessible_done, read_access);
  ustring repository_accessible_info;
  if (read_access) {
    repository_accessible_info = "Read access has been granted";
  } else {
    repository_accessible_info = "Set correct value for the repository";
  }
  gtk_label_set_text(GTK_LABEL (label_repository_accessible_output), read_access_elaborate_info.c_str());
  gtk_label_set_text(GTK_LABEL (label_repository_accessible_info), repository_accessible_info.c_str());

  // Bail out if there is no read access.
  if (!read_access)
    return;

  // Test write access to the repository.
  ustring write_access_elaborate_info;
  bool write_access = test_write_access(write_access_elaborate_info);
  gui_okay(image_repository_writable, label_repository_writable_done, write_access);
  ustring repository_writable_info;
  if (write_access) {
    repository_writable_info = "Write access has been granted";
  } else {
    repository_writable_info = "Can't write to the repository";
  }
  gtk_label_set_text(GTK_LABEL (label_repository_writable_output), write_access_elaborate_info.c_str());
  gtk_label_set_text(GTK_LABEL (label_repository_writable_info), repository_writable_info.c_str());

  // Bail out if there is no write access.
  if (!write_access)
    return;

  // See whether any action is to be taken.
  gui_okay(image_action, label_action_done, action_copy || action_sync);
  ustring action_info;
  if (action_copy) {
    action_info = "Copy repository to local data";
  } else if (action_sync) {
    action_info = "Merge local data with repository";
  } else {
    action_info = "Select action";
  }
  gtk_label_set_text(GTK_LABEL (label_action_info), action_info.c_str());

  // Bail out if no action is to be taken.
  if (!action_copy && !action_sync)
    return;

  // Handle the copy action.
  if (action_copy) {
    ustring clone_output;
    bool repo_cloned = test_clone_repository(clone_output);

    gui_okay(image_pull, label_pull_done, repo_cloned);
    ustring repo_cloned_info;
    if (repo_cloned) {
      repo_cloned_info = "The repository was copied to the local data";
    } else {
      repo_cloned_info = "Can't copy the repository";
    }
    gtk_label_set_text(GTK_LABEL (label_pull_info), repo_cloned_info.c_str());
    gtk_label_set_text(GTK_LABEL (label_pull_output), clone_output.c_str());

    // If the repository was not copied, bail out.
    if (!repo_cloned)
      return;
  }

  // Handle the synchronize action.
  if (action_sync) {

    // Check whether we can pull the changes without conflicts.
    ustring pull_output;
    bool changes_pulled = test_pull_changes(pull_output);
    gui_okay(image_pull, label_pull_done, changes_pulled);
    ustring repo_pulled_info;
    if (changes_pulled) {
      repo_pulled_info = "Changes from the remote repository were merged with the local data";
    } else {
      repo_pulled_info = "Can't merge the changes from the repository";
    }
    gtk_label_set_text(GTK_LABEL (label_pull_info), repo_pulled_info.c_str());
    gtk_label_set_text(GTK_LABEL (label_pull_output), pull_output.c_str());

    // Bail out if the changes were not pulled.
    if (!changes_pulled)
      return;

    // Check whether we can push changes to the repository.
    ustring push_output;
    bool changes_pushed = test_push_changes(push_output);
    gui_okay(image_push, label_push_done, changes_pushed);
    ustring repo_pushed_info;
    if (changes_pushed) {
      repo_pushed_info = "Changes were pushed to the repository";
    } else {
      repo_pushed_info = "Can't push to the repository";
    }
    gtk_label_set_text(GTK_LABEL (label_push_info), repo_pushed_info.c_str());
    gtk_label_set_text(GTK_LABEL (label_push_output), push_output.c_str());

    // Bail out if the changes were not pushed.
    if (!changes_pushed)
      return;

  }

  // Finally set the ok button sensitive.
  gtk_widget_set_sensitive(okbutton, true);
}

bool GitSetupDialog::tasks_counter_timeout(gpointer user_data) {
  ((GitSetupDialog *) user_data)->tasks_counter();
  return true;
}

void GitSetupDialog::tasks_counter() {
  extern Settings * settings;
  project_tasks_count = git_count_tasks_project(settings->genconfig.project_get());
  if (project_tasks_count != previous_project_tasks_count) {
    previous_project_tasks_count = project_tasks_count;
    set_gui();
  }
}

void GitSetupDialog::on_okbutton_clicked(GtkButton *button, gpointer user_data) {
  ((GitSetupDialog *) user_data)->on_okbutton();
}

void GitSetupDialog::on_okbutton() {
  // Save variables.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  ProjectConfiguration * projectconfig = settings->projectconfig(project);

  // The remote repository.
  bool use_remote_repository = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (checkbutton_remote));
  projectconfig->git_use_remote_repository_set(use_remote_repository);
  projectconfig->git_remote_repository_url_set(url_get());

  // Remote update interval.
  projectconfig->git_remote_update_interval_set(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON (spinbutton_update)));

  // Save conflict handling system.
  GitConflictHandlingType conflicthandling = gchtTakeMe;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (radiobutton_conflict_server)))
    conflicthandling = gchtTakeServer;
  projectconfig->git_remote_repository_conflict_handling_set(conflicthandling);
}

ustring GitSetupDialog::url_get()
// Gets the url from the entry, leaving out unsupported protocols.
{
  ustring url = gtk_entry_get_text(GTK_ENTRY (entry_path));
  url = trim (url);
  replace_text(url, "ssh", "");
  replace_text(url, "http", "");
  return url;
}

bool GitSetupDialog::test_read_access(ustring& error)
// Tests read access to the repository.
{
  bool access = false;
  if (!url_get ().empty()) {
    GwSpawn spawn("git-ls-remote");
    spawn.arg(url_get());
    spawn.read();
    spawn.run();
    access = spawn.exitstatus == 0;
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      if (i > 0)
        error.append("\n");
      error.append(spawn.standarderr[i]);
    }
  }
  return access;
}

bool GitSetupDialog::test_write_access(ustring& error)
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
  g_file_set_contents(gw_build_filename (working_directory, filename).c_str(), "", 0, NULL);

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
    spawn.arg("master:master");
    spawn.run();
  }
  {
    GwSpawn spawn("git-push");
    spawn.workingdirectory(working_directory);
    spawn.arg(url_get());
    spawn.arg("master:master");
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
  unlink(gw_build_filename (working_directory, filename).c_str());
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
    spawn.arg("master:master");
    spawn.run();
  }

  // Remove the temporal work space.
  unix_rmdir(temporal_directory);

  // Return whether writable.
  return writable;
}

bool GitSetupDialog::test_pull_changes(ustring& error)
// This function first tries to pull the data from the remote repository 
// into a temporal copy of the local project.
// If that succeeds it does the actual pulling in the real project.
{
  // Variable whether the changes were pulled.
  bool pulled = false;

  // Copy the project to a temporal one.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  ustring temporal_project = "__git__pull__test__";
  project_copy(project, temporal_project);

  // Try to pull the changes into the temporal project.
  ustring datadirectory = project_data_directory_project(temporal_project);
  {
    GwSpawn spawn("git-pull");
    spawn.workingdirectory(datadirectory);
    spawn.arg(url_get());
    spawn.arg("master:master");  // Some git installations require the name of the branchs.
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
    spawn.arg("master:master");  // Some git installations require the name of the branchs. First noted on Kubuntu 8.04.
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

bool GitSetupDialog::test_push_changes(ustring& error)
// This function push the changes into the remote repository.
{
  // Variable whether the changes were pushed.
  bool pushed = false;

  // Try to push the changes to the remote repository.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  ustring datadirectory = project_data_directory_project(project);
  GwSpawn spawn("git-push");
  spawn.workingdirectory(datadirectory);
  spawn.arg(url_get());
  spawn.arg("master:master"); // Some git installations require the branch: master.
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

bool GitSetupDialog::test_clone_repository(ustring& error) {
  // Whether cloning went ok.
  bool cloned = false;

  // Create a temporal workspace.
  ustring temporal_directory = gw_build_filename(directories_get_temp(), "git_cloned_remote_repository");
  unix_rmdir(temporal_directory);
  gw_mkdir_with_parents(temporal_directory);

  // Clone the remote repository to the temporal workspace.
  GwSpawn spawn("git-clone");
  spawn.workingdirectory(temporal_directory);
  spawn.arg(url_get());
  spawn.progress("Reading repository", false);
  spawn.read();
  spawn.run();
  cloned = (spawn.exitstatus == 0);
  for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
    if (cloned) {
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

  // If cloned, copy the data into the project.
  if (cloned) {
    extern Settings * settings;
    ustring project = settings->genconfig.project_get();
    ustring project_data_directory = project_data_directory_project(project);
    ustring cloned_data_directory = gw_build_filename(temporal_directory, gw_path_get_basename(url_get()));
    unix_rmdir(project_data_directory);
    unix_mv(cloned_data_directory, project_data_directory);
  }

  // Remove the temporal work space.
  unix_rmdir(temporal_directory);

  // Give the result.  
  return cloned;
}

void GitSetupDialog::on_button_action_clicked(GtkButton *button, gpointer user_data) {
  ((GitSetupDialog *) user_data)->button_action_clicked(button);
}

void GitSetupDialog::button_action_clicked(GtkButton *button) {
  action_copy = (button == GTK_BUTTON (button_copy));
  action_sync = (button == GTK_BUTTON (button_synchronize));
  on_entry();
}
