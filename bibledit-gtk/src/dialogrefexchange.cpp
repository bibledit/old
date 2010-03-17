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
#include "dialogrefexchange.h"
#include "settings.h"
#include "shell.h"
#include "windowsoutpost.h"
#include "help.h"
#include "directories.h"
#include "gwrappers.h"


ReferenceExchangeDialog::ReferenceExchangeDialog(int dummy)
{
  extern Settings *settings;
  Shortcuts shortcuts (0);
  
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.referenceexchangedialog.xml").c_str(), NULL);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  entry_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "entry_url"));
  gtk_entry_set_text (GTK_ENTRY (entry_url), settings->genconfig.bibledit_web_url_get().c_str());

  button_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_url"));
  shortcuts.button (button_url);

  label_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_url"));
  gtk_label_set_text (GTK_LABEL (label_url), "");

  checkbutton_bibleworks = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_bibleworks"));
  shortcuts.button (checkbutton_bibleworks);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_bibleworks), settings->genconfig.reference_exchange_send_to_bibleworks_get());

  checkbutton_santafe = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_santafe"));
  shortcuts.button (checkbutton_santafe);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_santafe), settings->genconfig.reference_exchange_send_to_santafefocus_get());

  checkbutton_onlinebible = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton_onlinebible"));
  shortcuts.button (checkbutton_onlinebible);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_onlinebible), settings->genconfig.reference_exchange_send_to_onlinebible_get());

  GSList *radiobutton_receive_group = NULL;

  radiobutton_off = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_off"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_off), radiobutton_receive_group);
  radiobutton_receive_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_off));
  shortcuts.button (radiobutton_off);

  radiobutton_bibleworks = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_bibleworks"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_bibleworks), radiobutton_receive_group);
  radiobutton_receive_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_bibleworks));
  shortcuts.button (radiobutton_bibleworks);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_bibleworks), settings->genconfig.reference_exchange_receive_from_bibleworks_get());

  radiobutton_santafe = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_santafe"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_santafe), radiobutton_receive_group);
  radiobutton_receive_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_santafe));
  shortcuts.button (radiobutton_santafe);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_santafe), settings->genconfig.reference_exchange_receive_from_santafefocus_get());

  radiobutton_onlinebible = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "radiobutton_onlinebible"));
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton_onlinebible), radiobutton_receive_group);
  radiobutton_receive_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton_onlinebible));
  shortcuts.button (radiobutton_onlinebible);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton_onlinebible), settings->genconfig.reference_exchange_receive_from_onlinebible_get());

  vbox_outpost = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbox_outpost"));

  InDialogHelp * indialoghelp = new InDialogHelp(dialog, gtkbuilder, &shortcuts, NULL);
  cancelbutton = indialoghelp->cancelbutton;
  okbutton = indialoghelp->okbutton;
  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
  shortcuts.stockbutton (cancelbutton);
  shortcuts.stockbutton (okbutton);
  
  shortcuts.process ();

  g_signal_connect((gpointer) button_url, "clicked", G_CALLBACK(on_url_test_clicked), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) checkbutton_bibleworks, "toggled", G_CALLBACK(on_button_outpost_requirement_toggled), gpointer(this));
  g_signal_connect_after((gpointer) radiobutton_bibleworks, "toggled", G_CALLBACK(on_button_outpost_requirement_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_santafe, "toggled", G_CALLBACK(on_button_outpost_requirement_toggled), gpointer(this));
  g_signal_connect_after((gpointer) radiobutton_santafe, "toggled", G_CALLBACK(on_button_outpost_requirement_toggled), gpointer(this));
  g_signal_connect((gpointer) checkbutton_onlinebible, "toggled", G_CALLBACK(on_button_outpost_requirement_toggled), gpointer(this));
  g_signal_connect_after((gpointer) radiobutton_onlinebible, "toggled", G_CALLBACK(on_button_outpost_requirement_toggled), gpointer(this));

  // Set gui.
  on_outpost();
}


ReferenceExchangeDialog::~ReferenceExchangeDialog()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int ReferenceExchangeDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


void ReferenceExchangeDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_okbutton();
}


void ReferenceExchangeDialog::on_okbutton()
{
  extern Settings *settings;
  settings->genconfig.bibledit_web_url_set(gtk_entry_get_text (GTK_ENTRY (entry_url)));
  settings->genconfig.reference_exchange_send_to_bibleworks_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_bibleworks)));
  settings->genconfig.reference_exchange_receive_from_bibleworks_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_bibleworks)));
  settings->genconfig.reference_exchange_send_to_santafefocus_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_santafe)));
  settings->genconfig.reference_exchange_receive_from_santafefocus_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_santafe)));
  settings->genconfig.reference_exchange_send_to_onlinebible_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_onlinebible)));
  settings->genconfig.reference_exchange_receive_from_onlinebible_set(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_onlinebible)));
}


void ReferenceExchangeDialog::on_button_outpost_requirement_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_outpost();
}


void ReferenceExchangeDialog::on_outpost()
// Shows a warning if the Windows Outpost is needed but does not run presently.
{
  bool outpost_needed = false;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_bibleworks)))
    outpost_needed = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_bibleworks)))
    outpost_needed = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_santafe)))
    outpost_needed = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_santafe)))
    outpost_needed = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_onlinebible)))
    outpost_needed = true;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_onlinebible)))
    outpost_needed = true;
  bool outpost_running = program_is_running(BIBLEDIT_WINDOWS_OUTPOST_EXE);
  if (outpost_needed && (!outpost_running)) {
    gtk_widget_show(vbox_outpost);
  } else {
    gtk_widget_hide(vbox_outpost);
  }
}


void ReferenceExchangeDialog::on_url_test_clicked(GtkButton * button, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_url_test();
}


void ReferenceExchangeDialog::on_url_test()
{
  GwSpawn spawn ("curl");
  spawn.arg ("-sS"); // Make curl silent, but show errors.
  ustring url = gtk_entry_get_text (GTK_ENTRY (entry_url));
  url.append ("/ipc/setmessage.php");
  spawn.arg (url);
  spawn.read ();
  spawn.run ();
  ustring message;
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    message.append (spawn.standardout[i] + "\n");
  }
  for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
    message.append (spawn.standarderr[i] + "\n");
  }
  if (message.empty()) {
    if (spawn.exitstatus == 0) {
      message = "Access okay";
    }
  }
  gtk_label_set_text (GTK_LABEL (label_url), message.c_str());
}
