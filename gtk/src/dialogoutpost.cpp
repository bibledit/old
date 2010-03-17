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
#include "dialoglistview.h"
#include "windowsoutpost.h"
#include "progresswindow.h"
#include "help.h"
#include "tiny_utilities.h"


OutpostDialog::OutpostDialog(int dummy)
{
  // Save and initialize variables.
  extern Settings *settings;
  wine_path = settings->genconfig.wine_path_get();
  outpost_command = settings->genconfig.outpost_command_get();
  changed = false;
  old_use_outpost = settings->genconfig.use_outpost_get();
  old_outpost_networked = settings->genconfig.outpost_networked_get();
  old_outpost_host = settings->genconfig.outpost_host_get();
  old_wine_path = wine_path;
  old_bottle = settings->genconfig.wine_bottle_get();
  old_outpost_command = outpost_command;

  // Build GUI.
  outpostdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(outpostdialog), "Windows Outpost Setup");
  gtk_window_set_position(GTK_WINDOW(outpostdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = GTK_DIALOG(outpostdialog)->vbox;
  gtk_widget_show(dialog_vbox1);
  gtk_box_set_spacing (GTK_BOX (dialog_vbox1), 5);

  checkbutton_use = gtk_check_button_new_with_mnemonic ("_Use Windows Outpost");
  gtk_widget_show (checkbutton_use);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), checkbutton_use, FALSE, FALSE, 0);

  // Set whether to use the outpost.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_use), old_use_outpost);

  hbox17 = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (hbox17);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox17, TRUE, TRUE, 0);

  checkbutton_host = gtk_check_button_new_with_mnemonic ("Windows Outpost _runs on another host:");
  gtk_widget_show (checkbutton_host);
  gtk_box_pack_start (GTK_BOX (hbox17), checkbutton_host, FALSE, FALSE, 0);

  // Set whether the outpost runs on another host.
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_host), old_outpost_networked);

  entry_host = gtk_entry_new ();
  gtk_widget_show (entry_host);
  gtk_box_pack_start (GTK_BOX (hbox17), entry_host, TRUE, TRUE, 0);

  // Set host
  gtk_entry_set_text(GTK_ENTRY(entry_host), old_outpost_host.c_str());

  hseparator6 = gtk_hseparator_new ();
  gtk_widget_show (hseparator6);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hseparator6, TRUE, TRUE, 0);

  hbox_wine = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox_wine);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox_wine, TRUE, TRUE, 0);

  label_wine_question = gtk_label_new ("Locate Wine or CrossOver Office");
  gtk_widget_show (label_wine_question);
  gtk_box_pack_start (GTK_BOX (hbox_wine), label_wine_question, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_wine_question), 0, 0.5);

  button_wine_select = gtk_button_new ();
  gtk_widget_show (button_wine_select);
  gtk_box_pack_start (GTK_BOX (hbox_wine), button_wine_select, FALSE, FALSE, 0);

  alignment9 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment9);
  gtk_container_add (GTK_CONTAINER (button_wine_select), alignment9);

  hbox16 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox16);
  gtk_container_add (GTK_CONTAINER (alignment9), hbox16);

  image20 = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image20);
  gtk_box_pack_start (GTK_BOX (hbox16), image20, FALSE, FALSE, 0);

  label47 = gtk_label_new_with_mnemonic ("Se_lect");
  gtk_widget_show (label47);
  gtk_box_pack_start (GTK_BOX (hbox16), label47, FALSE, FALSE, 0);

  button_wine_open = gtk_button_new ();
  gtk_widget_show (button_wine_open);
  gtk_box_pack_start (GTK_BOX (hbox_wine), button_wine_open, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_wine_open), alignment3);

  hbox10 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox10);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox10);

  image11 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image11);
  gtk_box_pack_start (GTK_BOX (hbox10), image11, FALSE, FALSE, 0);

  label34 = gtk_label_new_with_mnemonic ("O_pen");
  gtk_widget_show (label34);
  gtk_box_pack_start (GTK_BOX (hbox10), label34, FALSE, FALSE, 0);

  button_wine_search = gtk_button_new ();
  gtk_widget_show (button_wine_search);
  gtk_box_pack_start (GTK_BOX (hbox_wine), button_wine_search, FALSE, FALSE, 0);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_wine_search), alignment4);

  hbox11 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox11);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox11);

  image12 = gtk_image_new_from_stock ("gtk-find", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image12);
  gtk_box_pack_start (GTK_BOX (hbox11), image12, FALSE, FALSE, 0);

  label35 = gtk_label_new_with_mnemonic ("Fi_nd");
  gtk_widget_show (label35);
  gtk_box_pack_start (GTK_BOX (hbox11), label35, FALSE, FALSE, 0);

  label_wine_info = gtk_label_new ("");
  gtk_widget_show (label_wine_info);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label_wine_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_wine_info), 0, 0.5);

  hseparator5 = gtk_hseparator_new ();
  gtk_widget_show (hseparator5);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hseparator5, TRUE, TRUE, 0);

  hbox_bottle = gtk_hbox_new (FALSE, 5);
  gtk_widget_show (hbox_bottle);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox_bottle, TRUE, TRUE, 0);

  label_bottle_question = gtk_label_new ("Enter bottle for the Outpost");
  gtk_widget_show (label_bottle_question);
  gtk_box_pack_start (GTK_BOX (hbox_bottle), label_bottle_question, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_bottle_question), 0, 0.5);

  entry_bottle = gtk_entry_new ();
  gtk_widget_show (entry_bottle);
  gtk_box_pack_start (GTK_BOX (hbox_bottle), entry_bottle, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry_bottle), 9679);

  gtk_entry_set_text (GTK_ENTRY (entry_bottle), old_bottle.c_str());
  
  hseparator4 = gtk_hseparator_new ();
  gtk_widget_show (hseparator4);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hseparator4, TRUE, TRUE, 0);

  label_access_info = gtk_label_new ("");
  gtk_widget_show (label_access_info);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), label_access_info, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label_access_info), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG(outpostdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(outpostdialog, NULL, NULL, "menu-preferences/windows-outpost");

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
  g_signal_connect((gpointer) entry_host, "changed", G_CALLBACK(on_entry_changed), gpointer(this));
  g_signal_connect((gpointer) button_wine_select, "clicked", G_CALLBACK(on_button_wine_select_clicked), gpointer(this));
  g_signal_connect((gpointer) button_wine_open, "clicked", G_CALLBACK(on_button_wine_open_clicked), gpointer(this));
  g_signal_connect((gpointer) button_wine_search, "clicked", G_CALLBACK(on_button_wine_search_clicked), gpointer(this));
  g_signal_connect((gpointer) entry_bottle, "changed", G_CALLBACK(on_entry_changed), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);

  gui_changed_event_id = 0;

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


void OutpostDialog::on_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((OutpostDialog *) user_data)->on_entry();
}


void OutpostDialog::on_entry()
{
  gw_destroy_source(gui_changed_event_id);
  gui_changed_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 300, GSourceFunc(gui_delayer), gpointer(this), NULL);
}


bool OutpostDialog::gui_delayer(gpointer user_data)
{
  ((OutpostDialog *) user_data)->gui_delayed();
  return false;
}


void OutpostDialog::gui_delayed()
{
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
  settings->genconfig.wine_path_set(wine_path);
  settings->genconfig.wine_bottle_set(bottle());
  settings->genconfig.outpost_command_set(outpost_command);
  // See whether data changed.
  if (use_outpost != old_use_outpost)
    changed = true;
  if (outpost_networked != old_outpost_networked)
    changed = true;
  if (outpost_host != old_outpost_host)
    changed = true;
  if (wine_path != old_wine_path)
    changed = true;
  if (old_bottle != bottle())
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
  gtk_widget_set_sensitive(entry_host, use && anotherhost);
  gtk_widget_set_sensitive(hbox_wine, use && !anotherhost);
  gtk_widget_set_sensitive(label_wine_info, use && !anotherhost);
  gtk_widget_set_sensitive(hbox_bottle, use && !anotherhost);
  gtk_widget_set_sensitive(label_access_info, use);

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

  // Selection of Wine binary possible if more then one available.
  gtk_widget_set_sensitive(button_wine_select, (wine_paths.size() > 1) && use && !anotherhost);

  // Progress.
  progresswindow.set_fraction(0.7);

  // Whether the user can enter a bottle.
  
  if (winetype == wtWine) {
    gtk_widget_set_sensitive(hbox_bottle, false);
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
    if (wine_found) {
      outpost_command = wine_path + " ";
      if (winetype == wtCxoffice) {
        if (!bottle().empty()) {
          outpost_command.append("--bottle " + bottle());
        }
      }
      outpost_command.append(shell_quote_space(windowsoutpost_path()));
      access_info = "To start the Outpost, Bibledit-Gtk will issue command:\n";
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
    guiok = wine_found;

  // Can we say "Ok"?
  bool ok = guiok || !use;
  gtk_widget_set_sensitive(okbutton, ok);
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


ustring OutpostDialog::bottle ()
// Gets the bottle as entered by the user.
{
  ustring value = gtk_entry_get_text (GTK_ENTRY (entry_bottle));
  return value;
}


