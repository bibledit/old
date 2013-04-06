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
#include "dialoggui.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"
#include "utilities.h"
#include "gtkwrappers.h"
#include "directories.h"
#include "gwrappers.h"


GuiDialog::GuiDialog(int dummy)
{
  Shortcuts shortcuts(0);
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.guidialog.xml").c_str(), NULL);
  featuresdialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "guidialog"));
  radiobutton_basic = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_basic"));
  shortcuts.button(radiobutton_basic);
  radiobutton_full = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_full"));
  shortcuts.button(radiobutton_full);
  radiobutton_user = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_user"));
  shortcuts.button(radiobutton_user);
  checkbutton_printing = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_printing"));
  shortcuts.button(checkbutton_printing);
  checkbutton_project_management = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_project_management"));
  shortcuts.button(checkbutton_project_management);
  checkbutton_references_management = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_references_management"));
  shortcuts.button(checkbutton_references_management);
  checkbutton_styles = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_styles"));
  shortcuts.button(checkbutton_styles);
  checkbutton_styles_management = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_styles_management"));
  shortcuts.button(checkbutton_styles_management);
  checkbutton_project_notes = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_project_notes"));
  shortcuts.button(checkbutton_project_notes);
  checkbutton_project_notes_management = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_project_notes_management"));
  shortcuts.button(checkbutton_project_notes_management);
  checkbutton_references_and_find = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_references_find"));
  shortcuts.button(checkbutton_references_and_find);
  checkbutton_replace = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_replace"));
  shortcuts.button(checkbutton_replace);
  checkbutton_checks = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_checks"));
  shortcuts.button(checkbutton_checks);
  checkbutton_tools = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_tools"));
  shortcuts.button(checkbutton_tools);
  checkbutton_preferences = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_preferences"));
  shortcuts.button(checkbutton_preferences);
  checkbutton_remember_verse_chapter = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_remember_verse_chapter"));
  shortcuts.button(checkbutton_remember_verse_chapter);
  checkbutton_start_maximized = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_start_maximized"));
  shortcuts.button(checkbutton_start_maximized);
  InDialogHelp * indialoghelp = new InDialogHelp(featuresdialog, gtkbuilder, &shortcuts, "menu-preferences/user-interface");
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
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
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_remember_verse_chapter), settings->genconfig.remember_verse_per_chapter_get());
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_start_maximized), settings->genconfig.start_program_maximized_get());
}


GuiDialog::~GuiDialog()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(featuresdialog);
}


int GuiDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(featuresdialog));
}


void GuiDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((GuiDialog *) user_data)->on_okbutton();
}


void GuiDialog::on_okbutton()
// Store the new features.
{
  // Settings object.
  extern Settings *settings;

  // Get the old and new features.
  unsigned int old_mode = settings->genconfig.features_mode_get();
  unsigned int new_mode = mode_get();
  ustring old_list = settings->genconfig.features_list_get();
  ustring new_list = list_get();

  // Store the data.
  settings->genconfig.features_mode_set(new_mode);
  settings->genconfig.features_list_set(new_list);
  settings->genconfig.remember_verse_per_chapter_set(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_remember_verse_chapter))); 
  settings->genconfig.start_program_maximized_set(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_start_maximized))); 

  // Restart needed if there was a change in the features.
  if ((old_mode != new_mode) || (old_list != new_list)) {
    gtkw_dialog_info(featuresdialog, "The changes will take effect after Bibledit-Gtk has been restarted");
  }
}


void GuiDialog::on_togglebutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((GuiDialog *) user_data)->on_togglebutton();
}


void GuiDialog::on_togglebutton()
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


void GuiDialog::mode_set(unsigned int mode)
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


unsigned int GuiDialog::mode_get()
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


void GuiDialog::list_set(ustring pattern)
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


ustring GuiDialog::list_get()
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


void GuiDialog::on_checkbutton_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((GuiDialog *) user_data)->on_checkbutton(togglebutton);
}


void GuiDialog::on_checkbutton(GtkToggleButton * togglebutton)
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


void GuiDialog::check(GtkWidget * button, bool on)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), on);
}


