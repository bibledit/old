/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


#include "dialogreferencesettings.h"
#include "settings.h"
#include "help.h"
#include "shortcuts.h"
#include "directories.h"
#include "gwrappers.h"


ReferenceSettingsDialog::ReferenceSettingsDialog(int dummy)
{
  extern Settings *settings;
  Shortcuts shortcuts(0);

  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.referencesettingsdialog.xml").c_str(), NULL);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  checkbutton_verse_text = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_verse_text"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_verse_text), settings->genconfig.reference_window_show_verse_text_get());
  shortcuts.button (checkbutton_verse_text);
  
  checkbutton_relevant_bits = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_relevant_bits"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_relevant_bits), settings->genconfig.reference_window_show_relevant_bits_get());
  shortcuts.button (checkbutton_relevant_bits);

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);
  shortcuts.process();
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  gtk_widget_grab_default(okbutton);

  g_signal_connect((gpointer) checkbutton_verse_text, "toggled", G_CALLBACK(on_checkbutton_verse_text_toggled), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  set_gui();
}


ReferenceSettingsDialog::~ReferenceSettingsDialog()
{
  gtk_widget_destroy(dialog);
}


int ReferenceSettingsDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void ReferenceSettingsDialog::on_checkbutton_verse_text_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  ((ReferenceSettingsDialog *) user_data)->set_gui();
}


void ReferenceSettingsDialog::set_gui()
{
  gtk_widget_set_sensitive (checkbutton_relevant_bits, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse_text)));
}


void ReferenceSettingsDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReferenceSettingsDialog *) user_data)->on_ok();
}


void ReferenceSettingsDialog::on_ok()
{
  extern Settings *settings;
  settings->genconfig.reference_window_show_verse_text_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_verse_text)));
  settings->genconfig.reference_window_show_relevant_bits_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_relevant_bits)));
}


