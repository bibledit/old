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
#include "dialogoutpost.h"
#include "utilities.h"
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "uname.h"
#include "unixwrappers.h"
#include "directories.h"
#include "gwrappers.h"
#include "shell.h"
#include "settings.h"
#include "gui.h"
#include "dialoglistview.h"
#include "windowsoutpost.h"
#include "progresswindow.h"
#include "help.h"
#include "tiny_utilities.h"

OutpostDialog::OutpostDialog(int dummy)
{
  // Save and initialize variables.
  extern Settings *settings;
  outpost_path = settings->genconfig.outpost_path_get();
  wine_path = settings->genconfig.wine_path_get();
  outpost_command = settings->genconfig.outpost_command_get();
  changed = false;
  old_use_outpost = settings->genconfig.use_outpost_get();
  old_outpost_networked = settings->genconfig.outpost_networked_get();
  old_outpost_host = settings->genconfig.outpost_host_get();
  old_outpost_path = outpost_path;
  old_wine_path = wine_path;
  old_outpost_command = outpost_command;

  // Build GUI.
  outpostdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(outpostdialog), "Windows Outpost Setup");
  gtk_window_set_position(GTK_WINDOW(outpostdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = GTK_DIALOG(outpostdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 5);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new(10, 6, FALSE);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 4);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 4);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 4);

  vbox2 = gtk_vbox_new(FALSE, 4);
  gtk_widget_show(vbox2);
  gtk_table_attach(GTK_TABLE(table1), vbox2, 0, 6, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  checkbutton_use = gtk_check_button_new_with_mnemonic("_Use Windows Outpost");
  gtk_widget_show(checkbutton_use);
  gtk_box_pack_start(GTK_BOX(vbox2), checkbutton_use, FALSE, FALSE, 0);

  // Set (in)active.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_use), old_use_outpost);

  hbox17 = gtk_hbox_new(FALSE, 4);
  gtk_widget_show(hbox17);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox17, TRUE, TRUE, 0);

  checkbutton_host = gtk_check_button_new_with_mnemonic("Windows Outpost _runs on another host:");
  gtk_widget_show(checkbutton_host);
  gtk_box_pack_start(GTK_BOX(hbox17), checkbutton_host, FALSE, FALSE, 0);

  // Set (in)active.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_host), old_outpost_networked);

  entry_host = gtk_entry_new();
  gtk_widget_show(entry_host);
  gtk_box_pack_start(GTK_BOX(hbox17), entry_host, TRUE, TRUE, 0);

  // Set host
  gtk_entry_set_text(GTK_ENTRY(entry_host), old_outpost_host.c_str());

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_table_attach(GTK_TABLE(table1), hseparator1, 0, 6, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  image_outpost_ok = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_outpost_ok);
  gtk_table_attach(GTK_TABLE(table1), image_outpost_ok, 0, 1, 2, 3, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  label_outpost_ok = gtk_label_new("Done");
  gtk_widget_show(label_outpost_ok);
  gtk_table_attach(GTK_TABLE(table1), label_outpost_ok, 1, 2, 2, 3, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_outpost_ok), 0, 0.5);

  label_outpost_question = gtk_label_new("Locate Windows Outpost");
  gtk_widget_show(label_outpost_question);
  gtk_table_attach(GTK_TABLE(table1), label_outpost_question, 2, 3, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_outpost_question), 0, 0.5);

  button_outpost_select = gtk_button_new();
  gtk_widget_show(button_outpost_select);
  gtk_table_attach(GTK_TABLE(table1), button_outpost_select, 3, 4, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment8 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment8);
  gtk_container_add(GTK_CONTAINER(button_outpost_select), alignment8);

  hbox15 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox15);
  gtk_container_add(GTK_CONTAINER(alignment8), hbox15);

  image19 = gtk_image_new_from_stock("gtk-jump-to", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image19);
  gtk_box_pack_start(GTK_BOX(hbox15), image19, FALSE, FALSE, 0);

  label46 = gtk_label_new_with_mnemonic("_Select");
  gtk_widget_show(label46);
  gtk_box_pack_start(GTK_BOX(hbox15), label46, FALSE, FALSE, 0);

  button_outpost_open = gtk_button_new();
  gtk_widget_show(button_outpost_open);
  gtk_table_attach(GTK_TABLE(table1), button_outpost_open, 4, 5, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment5 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment5);
  gtk_container_add(GTK_CONTAINER(button_outpost_open), alignment5);

  hbox12 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox12);
  gtk_container_add(GTK_CONTAINER(alignment5), hbox12);

  image15 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image15);
  gtk_box_pack_start(GTK_BOX(hbox12), image15, FALSE, FALSE, 0);

  label39 = gtk_label_new_with_mnemonic("Op_en");
  gtk_widget_show(label39);
  gtk_box_pack_start(GTK_BOX(hbox12), label39, FALSE, FALSE, 0);

  button_outpost_find = gtk_button_new();
  gtk_widget_show(button_outpost_find);
  gtk_table_attach(GTK_TABLE(table1), button_outpost_find, 5, 6, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment6 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment6);
  gtk_container_add(GTK_CONTAINER(button_outpost_find), alignment6);

  hbox13 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox13);
  gtk_container_add(GTK_CONTAINER(alignment6), hbox13);

  image16 = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image16);
  gtk_box_pack_start(GTK_BOX(hbox13), image16, FALSE, FALSE, 0);

  label40 = gtk_label_new_with_mnemonic("F_ind");
  gtk_widget_show(label40);
  gtk_box_pack_start(GTK_BOX(hbox13), label40, FALSE, FALSE, 0);

  label_outpost_info = gtk_label_new("");
  gtk_widget_show(label_outpost_info);
  gtk_table_attach(GTK_TABLE(table1), label_outpost_info, 0, 6, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_outpost_info), 0, 0.5);

  hseparator2 = gtk_hseparator_new();
  gtk_widget_show(hseparator2);
  gtk_table_attach(GTK_TABLE(table1), hseparator2, 0, 6, 4, 5, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  image_wine_ok = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_wine_ok);
  gtk_table_attach(GTK_TABLE(table1), image_wine_ok, 0, 1, 5, 6, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  label_wine_ok = gtk_label_new("Done");
  gtk_widget_show(label_wine_ok);
  gtk_table_attach(GTK_TABLE(table1), label_wine_ok, 1, 2, 5, 6, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_wine_ok), 0, 0.5);

  label_wine_question = gtk_label_new("Locate Wine or CrossOver Office");
  gtk_widget_show(label_wine_question);
  gtk_table_attach(GTK_TABLE(table1), label_wine_question, 2, 3, 5, 6, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_wine_question), 0, 0.5);

  button_wine_select = gtk_button_new();
  gtk_widget_show(button_wine_select);
  gtk_table_attach(GTK_TABLE(table1), button_wine_select, 3, 4, 5, 6, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment9 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment9);
  gtk_container_add(GTK_CONTAINER(button_wine_select), alignment9);

  hbox16 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox16);
  gtk_container_add(GTK_CONTAINER(alignment9), hbox16);

  image20 = gtk_image_new_from_stock("gtk-jump-to", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image20);
  gtk_box_pack_start(GTK_BOX(hbox16), image20, FALSE, FALSE, 0);

  label47 = gtk_label_new_with_mnemonic("Se_lect");
  gtk_widget_show(label47);
  gtk_box_pack_start(GTK_BOX(hbox16), label47, FALSE, FALSE, 0);

  button_wine_open = gtk_button_new();
  gtk_widget_show(button_wine_open);
  gtk_table_attach(GTK_TABLE(table1), button_wine_open, 4, 5, 5, 6, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment3);
  gtk_container_add(GTK_CONTAINER(button_wine_open), alignment3);

  hbox10 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox10);
  gtk_container_add(GTK_CONTAINER(alignment3), hbox10);

  image11 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image11);
  gtk_box_pack_start(GTK_BOX(hbox10), image11, FALSE, FALSE, 0);

  label34 = gtk_label_new_with_mnemonic("O_pen");
  gtk_widget_show(label34);
  gtk_box_pack_start(GTK_BOX(hbox10), label34, FALSE, FALSE, 0);

  button_wine_search = gtk_button_new();
  gtk_widget_show(button_wine_search);
  gtk_table_attach(GTK_TABLE(table1), button_wine_search, 5, 6, 5, 6, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);

  alignment4 = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment4);
  gtk_container_add(GTK_CONTAINER(button_wine_search), alignment4);

  hbox11 = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox11);
  gtk_container_add(GTK_CONTAINER(alignment4), hbox11);

  image12 = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image12);
  gtk_box_pack_start(GTK_BOX(hbox11), image12, FALSE, FALSE, 0);

  label35 = gtk_label_new_with_mnemonic("Fi_nd");
  gtk_widget_show(label35);
  gtk_box_pack_start(GTK_BOX(hbox11), label35, FALSE, FALSE, 0);

  label_wine_info = gtk_label_new("Wine");
  gtk_widget_show(label_wine_info);
  gtk_table_attach(GTK_TABLE(table1), label_wine_info, 0, 6, 6, 7, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_wine_info), 0, 0.5);

  hseparator3 = gtk_hseparator_new();
  gtk_widget_show(hseparator3);
  gtk_table_attach(GTK_TABLE(table1), hseparator3, 0, 6, 7, 8, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  image_access_ok = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image_access_ok);
  gtk_table_attach(GTK_TABLE(table1), image_access_ok, 0, 1, 8, 9, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);

  label_access_ok = gtk_label_new("Done");
  gtk_widget_show(label_access_ok);
  gtk_table_attach(GTK_TABLE(table1), label_access_ok, 1, 2, 8, 9, (GtkAttachOptions) (0), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_access_ok), 0, 0.5);

  label_access_question = gtk_label_new("Final information");
  gtk_widget_show(label_access_question);
  gtk_table_attach(GTK_TABLE(table1), label_access_question, 2, 3, 8, 9, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_access_question), 0, 0.5);

  label_access_info = gtk_label_new("");
  gtk_widget_show(label_access_info);
  gtk_table_attach(GTK_TABLE(table1), label_access_info, 0, 6, 9, 10, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label_access_info), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG(outpostdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(outpostdialog, NULL, "outpost");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(outpostdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(outpostdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) checkbutton_use, "toggled", G_CALLBACK(on_checkbutton_use_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_host, "toggled", G_CALLBACK(on_checkbutton_host_toggled), gpointer(this));
  g_signal_connect((gpointer) entry_host, "changed", G_CALLBACK(on_entry_host_changed), gpointer(this));
  g_signal_connect((gpointer) button_outpost_select, "clicked", G_CALLBACK(on_button_outpost_select_clicked), gpointer(this));
  g_signal_connect((gpointer) button_outpost_open, "clicked", G_CALLBACK(on_button_outpost_open_clicked), gpointer(this));
  g_signal_connect((gpointer) button_outpost_find, "clicked", G_CALLBACK(on_button_outpost_find_clicked), gpointer(this));
  g_signal_connect((gpointer) button_wine_select, "clicked", G_CALLBACK(on_button_wine_select_clicked), gpointer(this));
  g_signal_connect((gpointer) button_wine_open, "clicked", G_CALLBACK(on_button_wine_open_clicked), gpointer(this));
  g_signal_connect((gpointer) button_wine_search, "clicked", G_CALLBACK(on_button_wine_search_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);

  host_event_id = 0;

  // Set gui.
  set_gui();
}

OutpostDialog::~OutpostDialog()
{
  gtk_widget_destroy(outpostdialog);
}

int OutpostDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(outpostdialog));
}

void OutpostDialog::on_checkbutton_use_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_use();
}

void OutpostDialog::on_use()
{
  set_gui();
}

void OutpostDialog::on_checkbutton_host_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_host();
}

void OutpostDialog::on_host()
{
  set_gui();
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_host)))
    gtk_widget_grab_focus(entry_host);
}

void OutpostDialog::on_entry_host_changed(GtkEditable * editable, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_entry_host();
}

void OutpostDialog::on_entry_host()
{
  gw_destroy_source(host_event_id);
  host_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 300, GSourceFunc(host_delayer), gpointer(this), NULL);
}

bool OutpostDialog::host_delayer(gpointer user_data)
{
  ((OutpostDialog *) user_data)->host_delayed();
  return false;
}

void OutpostDialog::host_delayed()
{
  set_gui();
}

void OutpostDialog::on_button_outpost_select_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_outpost_select();
}

void OutpostDialog::on_outpost_select()
{
  vector < ustring > paths(outpost_paths.begin(), outpost_paths.end());
  ListviewDialog dialog("Select the Outpost you wish to use", paths, outpost_path, true, NULL);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  outpost_path = dialog.focus;
  set_gui();
}

void OutpostDialog::on_button_outpost_open_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_outpost_open();
}

void OutpostDialog::on_outpost_open()
{
  ustring filename = gtkw_file_chooser_open(outpostdialog, "Select Outpost executable", outpost_path);
  if (filename.empty())
    return;
  outpost_path = filename;
  set_gui();
}

void OutpostDialog::on_button_outpost_find_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_outpost_find();
}

void OutpostDialog::on_outpost_find()
// Search whole system for Outposts.
{
  // Do the search.
  GwSpawn spawn("find");
  spawn.arg("/");
  spawn.arg("-name");
  spawn.arg(BIBLEDIT_WINDOWS_OUTPOST_EXE);
  spawn.read();
  spawn.progress("Searching the system for Outposts", false);
  spawn.run();
  // Read and process the results.
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    outpost_paths.insert(spawn.standardout[i]);
  }
  if (outpost_paths.empty()) {
    gtkw_dialog_error(outpostdialog, "Could't find any Outposts.\nInstall a Windows Outpost.\nSee help.");
  } else {
    ustring info;
    if (outpost_paths.size() == 1) {
      info = "Outpost was found.";
      vector < ustring > paths(outpost_paths.begin(), outpost_paths.end());
      outpost_path = paths[0];
    } else {
      info = "Outposts were found.\nSelect which one to use.";
    }
    gtkw_dialog_info(outpostdialog, info.c_str());
  }
  set_gui();
}

void OutpostDialog::on_button_wine_select_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_wine_select();
}

void OutpostDialog::on_wine_select()
{
  vector < ustring > paths(wine_paths.begin(), wine_paths.end());
  ListviewDialog dialog("Select the Wine binary you wish to use", paths, wine_path, true, NULL);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  wine_path = dialog.focus;
  set_gui();
}

void OutpostDialog::on_button_wine_open_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_wine_open();
}

void OutpostDialog::on_wine_open()
{
  ustring filename = gtkw_file_chooser_open(outpostdialog, "Select Wine binary", wine_path);
  if (filename.empty())
    return;
  wine_path = filename;
  set_gui();
}

void OutpostDialog::on_button_wine_search_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_wine_search();
}

void OutpostDialog::on_wine_search()
// Search whole system for Wine binaries.
{
  // Do the search.
  GwSpawn spawn("find");
  spawn.arg("/");
  spawn.arg("-name");
  spawn.arg("wine");
  spawn.read();
  spawn.progress("Searching the system for Wine binaries", false);
  spawn.run();
  // Process the output.  
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    if (wine_path_okay(spawn.standardout[i]))
      wine_paths.insert(spawn.standardout[i]);
  }
  if (wine_paths.empty()) {
    gtkw_dialog_error(outpostdialog, "Could't find any Wine binary.\nInstall Wine or CrossOver Office.\nSee help.");
  } else {
    ustring info;
    if (wine_paths.size() == 1) {
      info = "Wine was found.";
      vector < ustring > paths(wine_paths.begin(), wine_paths.end());
      wine_path = paths[0];
    } else {
      info = "Wine binaries were found.\nSelect which one to use.";
    }
    gtkw_dialog_info(outpostdialog, info.c_str());
  }
  set_gui();
}

void OutpostDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_ok();
}

void OutpostDialog::on_ok()
{
  // Save variables.
  extern Settings *settings;
  bool use_outpost = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_use));
  settings->genconfig.use_outpost_set(use_outpost);
  bool outpost_networked = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_host));
  settings->genconfig.outpost_networked_set(outpost_networked);
  ustring outpost_host = gtk_entry_get_text(GTK_ENTRY(entry_host));
  settings->genconfig.outpost_host_set(outpost_host);
  settings->genconfig.outpost_path_set(outpost_path);
  settings->genconfig.wine_path_set(wine_path);
  settings->genconfig.outpost_command_set(outpost_command);
  // See whether data changed.
  if (use_outpost != old_use_outpost)
    changed = true;
  if (outpost_networked != old_outpost_networked)
    changed = true;
  if (outpost_host != old_outpost_host)
    changed = true;
  if (outpost_path != old_outpost_path)
    changed = true;
  if (wine_path != old_wine_path)
    changed = true;
  if (outpost_command != old_outpost_command)
    changed = true;
}

void OutpostDialog::set_gui()
{
  // Progress
  ProgressWindow progresswindow("Checking...", false);
  progresswindow.set_fraction(0.3);

  // Do we use the Outpost? And how?
  bool use = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_use));
  bool anotherhost = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_host));

  // Set sensitivity of widgets depending on if and how we use Outpost.
  gtk_widget_set_sensitive(checkbutton_host, use);
  gtk_widget_set_sensitive(image_outpost_ok, use && !anotherhost);
  gtk_widget_set_sensitive(label_outpost_ok, use && !anotherhost);
  gtk_widget_set_sensitive(label_outpost_question, use && !anotherhost);
  gtk_widget_set_sensitive(button_outpost_select, use && !anotherhost);
  gtk_widget_set_sensitive(button_outpost_open, use && !anotherhost);
  gtk_widget_set_sensitive(button_outpost_find, use && !anotherhost);
  gtk_widget_set_sensitive(label_outpost_info, use && !anotherhost);
  gtk_widget_set_sensitive(image_wine_ok, use && !anotherhost);
  gtk_widget_set_sensitive(label_wine_ok, use && !anotherhost);
  gtk_widget_set_sensitive(label_wine_question, use && !anotherhost);
  gtk_widget_set_sensitive(button_wine_select, use && !anotherhost);
  gtk_widget_set_sensitive(button_wine_open, use && !anotherhost);
  gtk_widget_set_sensitive(button_wine_search, use && !anotherhost);
  gtk_widget_set_sensitive(label_wine_info, use && !anotherhost);
  gtk_widget_set_sensitive(image_access_ok, use);
  gtk_widget_set_sensitive(label_access_ok, use);
  gtk_widget_set_sensitive(label_access_question, use);
  gtk_widget_set_sensitive(label_access_info, use);
  gtk_widget_set_sensitive(entry_host, use && anotherhost);

  // Give information about paths to the Outpost.
  find_outpost();
  bool outpost_found = outpost_path_okay(outpost_path);
  if (!outpost_found) {
    if (!outpost_paths.empty()) {
      vector < ustring > s(outpost_paths.begin(), outpost_paths.end());
      outpost_path = s[0];
    }
    outpost_found = outpost_path_okay(outpost_path);
  }
  ustring outpost_info;
  if (outpost_found) {
    outpost_info.append("Using Outpost");
  } else {
    outpost_info.append("Outpost not available");
  }
  outpost_info.append(" at " + outpost_path);
  if (outpost_paths.size() > 1) {
    outpost_info.append("\nA total of " + convert_to_string(outpost_paths.size()) + " Outposts has been found.");
  }
  gtk_label_set_text(GTK_LABEL(label_outpost_info), outpost_info.c_str());
  gui_okay(image_outpost_ok, label_outpost_ok, outpost_found);

  // Selection of Outpost possible if more then one available.
  gtk_widget_set_sensitive(button_outpost_select, (outpost_paths.size() > 1) && use && !anotherhost);

  // Progress.
  progresswindow.set_fraction(0.5);

  // Information about Wine.
  find_wine();
  bool wine_found = wine_path_okay(wine_path);
  if (!wine_found) {
    if (!wine_paths.empty()) {
      vector < ustring > s(wine_paths.begin(), wine_paths.end());
      wine_path = s[0];
    }
    wine_found = wine_path_okay(wine_path);
  }
  WineType winetype = wtUnknown;
  if (wine_found)
    winetype = (wine_get_type(wine_path));
  ustring wine_info;
  if (wine_found) {
    wine_info.append("Selected ");
    if (winetype == wtWine)
      wine_info.append("standard");
    else
      wine_info.append("CrossOver Office");
    wine_info.append(" Wine binary:");
  } else {
    wine_info.append("Wine not available at");
  }
  wine_info.append(" " + wine_path);
  if (wine_paths.size() > 1) {
    wine_info.append("\nA total of " + convert_to_string(wine_paths.size()) + " Wine binaries has been found.");
  }
  gtk_label_set_text(GTK_LABEL(label_wine_info), wine_info.c_str());
  gui_okay(image_wine_ok, label_wine_ok, wine_found);

  // Selection of Wine binary possible if more then one available.
  gtk_widget_set_sensitive(button_wine_select, (wine_paths.size() > 1) && use && !anotherhost);

  // Progress.
  progresswindow.set_fraction(0.7);

  // If we use cxoffice, get the bottle to run the Outpost in.
  ustring bottle;
  if (winetype == wtCxoffice) {
    size_t position = outpost_path.find("drive_c");
    if (position != string::npos) {
      bottle = outpost_path.substr(0, --position);
      bottle = gw_path_get_basename(bottle);
    }
  }
  // Contact host.
  ustring host = gtk_entry_get_text(GTK_ENTRY(entry_host));
  bool host_contacted_ping = true;
  bool host_contacted_telnet = true;
  if (anotherhost) {
    GwSpawn spawn("ping");
#ifdef WIN32
    spawn.arg("-n");
#else
    spawn.arg("-c");
#endif
    spawn.arg("1");
    spawn.arg(host);
    spawn.run();
    host_contacted_ping = (spawn.exitstatus == 0);
    host_contacted_telnet = windowsoutpost_telnet(host);
  }
  // Final information.
  ustring access_info;
  if (anotherhost) {
    access_info = "Host " + host + " ";
    if (host_contacted_ping || host_contacted_telnet)
      access_info.append("is accessible");
    else
      access_info.append("cannot be contacted through the network.");
  } else {
    if (outpost_found && wine_found) {
      outpost_command = wine_path + " ";
      if (winetype == wtCxoffice) {
        if (!bottle.empty()) {
          outpost_command.append("--bottle " + bottle);
        }
      }
      outpost_command.append(shell_quote_space(outpost_path));
      access_info = "To start the Outpost, Bibledit will issue command:\n";
      access_info.append(outpost_command);
    } else {
      access_info = "There is not enough information yet";
    }
  }

  // Access info
  gtk_label_set_text(GTK_LABEL(label_access_info), access_info.c_str());
  bool guiok;
  if (anotherhost)
    guiok = host_contacted_ping || host_contacted_telnet;
  else
    guiok = outpost_found && wine_found;
  gui_okay(image_access_ok, label_access_ok, guiok);

  // Can we say "Ok"?
  bool ok = guiok || !use;
  gtk_widget_set_sensitive(okbutton, ok);
}

bool OutpostDialog::outpost_path_okay(const ustring & path)
{
  bool ok = g_file_test(path.c_str(), G_FILE_TEST_IS_REGULAR);
  if (gw_path_get_basename(path) != BIBLEDIT_WINDOWS_OUTPOST_EXE)
    ok = false;
  return ok;
}

void OutpostDialog::find_outpost()
{
  // Collect standard Wine and CrossOver Office data locations.
  vector < ustring > directories;
  directories.push_back(gw_build_filename(g_get_home_dir(), ".wine"));
  ustring cxdirectory = gw_build_filename(g_get_home_dir(), ".cxoffice");
  ReadDirectories cxbottles(cxdirectory, "", "");
  for (unsigned int i = 0; i < cxbottles.directories.size(); i++) {
    directories.push_back(gw_build_filename(cxdirectory, cxbottles.directories[i]));
  }
  // Go through them to see if the Outpost is there.
  ustring filename;
  for (unsigned int i = 0; i < directories.size(); i++) {
    filename = gw_build_filename(directories[i], "drive_c/Program Files/Bibledit Windows Outpost", BIBLEDIT_WINDOWS_OUTPOST_EXE);
    if (outpost_path_okay(filename)) {
      outpost_paths.insert(filename);
    }
  }
}

bool OutpostDialog::wine_path_okay(const ustring & path)
{
  bool ok = gw_find_program_in_path(path);
  if (gw_path_get_basename(path) != "wine")
    ok = false;
  if (g_file_test(path.c_str(), G_FILE_TEST_IS_DIR))
    ok = false;
  if (wine_get_type(path) == wtUnknown)
    ok = false;
  return ok;
}

void OutpostDialog::find_wine()
{
  // Try standard wine.
  ustring filename = "wine";
  if (wine_path_okay(filename))
    wine_paths.insert(filename);
  // Try standard cxoffice.
  filename = gw_build_filename(g_get_home_dir(), "cxoffice/bin/wine");
  if (wine_path_okay(filename))
    wine_paths.insert(filename);
}

WineType OutpostDialog::wine_get_type(const ustring & path)
/*
Get the type of the Wine binary: Nothing, or Standard Wine or CrossOver Office.
Differences between regular wine and cx wine can be found by running wine --help
The output is different in both cases.
*/
{
  WineType winetype = wtUnknown;
  bool version_line = false;
  bool cxapp_line = false;
  GwSpawn spawn((char *)path.c_str());
  spawn.arg("--help");
  spawn.read();
  spawn.run();
  for (unsigned int i = 0; i < spawn.standardout.size(); i++)
    spawn.standarderr.push_back(spawn.standardout[i]);
  for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
    ustring line(spawn.standarderr[i]);
    if (line.find("wine --version") != string::npos)
      version_line = true;
    if (line.find("--cx-app") != string::npos)
      cxapp_line = true;
  }
  if (version_line)
    winetype = wtWine;
  if (cxapp_line)
    winetype = wtCxoffice;
  return winetype;
}
