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
#include "dialogfeatures.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "utilities.h"
#include "gtkwrappers.h"

FeaturesDialog::FeaturesDialog(int dummy)
{
  Shortcuts shortcuts(0);

  featuresdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(featuresdialog), "Features setup");
  gtk_window_set_position(GTK_WINDOW(featuresdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(featuresdialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(featuresdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  GSList *radiobutton_basic_group = NULL;

  radiobutton_basic = gtk_radio_button_new_with_mnemonic(NULL, "Only the basics are available");
  gtk_widget_show(radiobutton_basic);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_basic, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_basic), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_basic));

  shortcuts.button(radiobutton_basic);

  radiobutton_full = gtk_radio_button_new_with_mnemonic(NULL, "Full features are available");
  gtk_widget_show(radiobutton_full);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_full, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_full), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_full));

  shortcuts.button(radiobutton_full);

  radiobutton_user = gtk_radio_button_new_with_mnemonic(NULL, "User defined features");
  gtk_widget_show(radiobutton_user);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), radiobutton_user, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_user), radiobutton_basic_group);
  radiobutton_basic_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_user));

  shortcuts.button(radiobutton_user);

  checkbutton_printing = gtk_check_button_new_with_mnemonic("Printing");
  gtk_widget_show(checkbutton_printing);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_printing, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_printing);

  checkbutton_project_management = gtk_check_button_new_with_mnemonic("Project management");
  gtk_widget_show(checkbutton_project_management);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_project_management, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_project_management);

  checkbutton_references_management = gtk_check_button_new_with_mnemonic("References management");
  gtk_widget_show(checkbutton_references_management);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_references_management, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_references_management);

  checkbutton_styles = gtk_check_button_new_with_mnemonic("Styles");
  gtk_widget_show(checkbutton_styles);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_styles, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_styles);

  checkbutton_styles_management = gtk_check_button_new_with_mnemonic("Styles management");
  gtk_widget_show(checkbutton_styles_management);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_styles_management, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_styles_management);

  checkbutton_project_notes = gtk_check_button_new_with_mnemonic("Project notes");
  gtk_widget_show(checkbutton_project_notes);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_project_notes, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_project_notes);

  checkbutton_project_notes_management = gtk_check_button_new_with_mnemonic("Project notes management");
  gtk_widget_show(checkbutton_project_notes_management);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_project_notes_management, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_project_notes_management);

  checkbutton_references_and_find = gtk_check_button_new_with_mnemonic("References and find");
  gtk_widget_show(checkbutton_references_and_find);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_references_and_find, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_references_and_find);

  checkbutton_replace = gtk_check_button_new_with_mnemonic("Replace");
  gtk_widget_show(checkbutton_replace);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_replace, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_replace);

  checkbutton_checks = gtk_check_button_new_with_mnemonic("Checks");
  gtk_widget_show(checkbutton_checks);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_checks, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_checks);

  checkbutton_tools = gtk_check_button_new_with_mnemonic("Tools");
  gtk_widget_show(checkbutton_tools);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_tools, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_tools);

  checkbutton_preferences = gtk_check_button_new_with_mnemonic("Preferences");
  gtk_widget_show(checkbutton_preferences);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_preferences, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_preferences);

  dialog_action_area1 = GTK_DIALOG(featuresdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(featuresdialog, &shortcuts, "preferences_features");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(featuresdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(featuresdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) radiobutton_basic, "toggled", G_CALLBACK(on_togglebutton_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_full, "toggled", G_CALLBACK(on_togglebutton_toggled), gpointer(this));
  g_signal_connect((gpointer) radiobutton_user, "toggled", G_CALLBACK(on_togglebutton_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  g_signal_connect((gpointer) checkbutton_printing, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_project_management, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_references_management, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_styles, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_styles_management, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_project_notes, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_project_notes_management, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_references_and_find, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_replace, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_checks, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_tools, "toggled", G_CALLBACK(on_checkbutton_toggled), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);

  // Set the gui.
  extern Settings *settings;
  mode_set(settings->genconfig.features_mode_get());
  list_set(settings->genconfig.features_list_get());
  on_togglebutton();
}

FeaturesDialog::~FeaturesDialog()
{
  gtk_widget_destroy(featuresdialog);
}

int FeaturesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(featuresdialog));
}

void FeaturesDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((FeaturesDialog *) user_data)->on_okbutton();
}

void FeaturesDialog::on_okbutton()
// Store the new features.
{
  // Settings object.
  extern Settings *settings;

  // Get the old and new features.
  unsigned int old_mode = settings->genconfig.features_mode_get();
  unsigned int new_mode = mode_get();
  ustring old_list = settings->genconfig.features_list_get();
  ustring new_list = list_get();

  // Store the features.
  settings->genconfig.features_mode_set(new_mode);
  settings->genconfig.features_list_set(new_list);

  // Restart needed if there was a change in the features.
  if ((old_mode != new_mode) || (old_list != new_list)) {
    gtkw_dialog_info(featuresdialog, "The changes will take effect after Bibledit has been restarted");
  }
}

void FeaturesDialog::on_togglebutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((FeaturesDialog *) user_data)->on_togglebutton();
}

void FeaturesDialog::on_togglebutton()
{
  bool usersettings = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_user));
  gtk_widget_set_sensitive(checkbutton_printing, usersettings);
  gtk_widget_set_sensitive(checkbutton_project_management, usersettings);
  gtk_widget_set_sensitive(checkbutton_references_management, usersettings);
  gtk_widget_set_sensitive(checkbutton_styles, usersettings);
  gtk_widget_set_sensitive(checkbutton_styles_management, usersettings);
  gtk_widget_set_sensitive(checkbutton_project_notes, usersettings);
  gtk_widget_set_sensitive(checkbutton_project_notes_management, usersettings);
  gtk_widget_set_sensitive(checkbutton_references_and_find, usersettings);
  gtk_widget_set_sensitive(checkbutton_replace, usersettings);
  gtk_widget_set_sensitive(checkbutton_checks, usersettings);
  gtk_widget_set_sensitive(checkbutton_tools, usersettings);
  gtk_widget_set_sensitive(checkbutton_preferences, usersettings);
}

void FeaturesDialog::mode_set(unsigned int mode)
// Sets the mode, e.g. basic, full or user-defined.
{
  switch (mode) {
  case 0:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_basic), true);
      break;
    }
  case 1:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_full), true);
      break;
    }
  default:
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_user), true);
    }
  }
}

unsigned int FeaturesDialog::mode_get()
// Gets the mode, e.g. basic, full, or user-defined.
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_basic))) {
    return 0;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_full))) {
    return 1;
  }
  return 2;
}

void FeaturesDialog::list_set(ustring pattern)
// Set the list of features.
{
  // Note: The order of this list must be the same ad in function list_get ().
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_printing), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_project_management), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_references_management), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_styles), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_styles_management), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_project_notes), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_project_notes_management), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_references_and_find), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_replace), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_checks), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_tools), bitpattern_take(pattern));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_preferences), bitpattern_take(pattern));
}

ustring FeaturesDialog::list_get()
// Get the list of features.
{
  ustring pattern;
  // Note: The order of this list must be the same ad in function list_set ().
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_printing)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_project_management)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_references_management)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_styles)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_styles_management)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_project_notes)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_project_notes_management)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_references_and_find)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_replace)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_checks)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_tools)));
  bitpattern_add(pattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_preferences)));
  return pattern;
}

void FeaturesDialog::on_checkbutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((FeaturesDialog *) user_data)->on_checkbutton(togglebutton);
}

void FeaturesDialog::on_checkbutton(GtkToggleButton * togglebutton)
// Sets interdependent checkboxes.
{
  bool switches_on = gtk_toggle_button_get_active(togglebutton);

  GtkWidget *button = GTK_WIDGET(togglebutton);
  if (button == checkbutton_printing) {
  }
  if (button == checkbutton_project_management) {
  }
  if (button == checkbutton_references_management) {
    if (switches_on)
      check(checkbutton_references_and_find, true);
  }
  if (button == checkbutton_styles) {
    if (!switches_on)
      check(checkbutton_styles_management, false);
  }
  if (button == checkbutton_styles_management) {
    if (switches_on)
      check(checkbutton_styles, true);
  }
  if (button == checkbutton_project_notes) {
    if (!switches_on)
      check(checkbutton_project_notes_management, false);
  }
  if (button == checkbutton_project_notes_management) {
    if (switches_on)
      check(checkbutton_project_notes, true);
  }
  if (button == checkbutton_references_and_find) {
    if (!switches_on) {
      check(checkbutton_references_management, false);
      check(checkbutton_replace, false);
      check(checkbutton_checks, false);
    }
  }
  if (button == checkbutton_replace) {
    if (switches_on)
      check(checkbutton_references_and_find, true);
  }
  if (button == checkbutton_checks) {
    if (switches_on)
      check(checkbutton_references_and_find, true);
  }
  if (button == checkbutton_tools) {
  }
  if (button == checkbutton_preferences) {
  }
}

void FeaturesDialog::check(GtkWidget * button, bool on)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), on);
}
