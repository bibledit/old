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

  entry_user = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "entry_user"));
  gtk_entry_set_text (GTK_ENTRY (entry_user), settings->genconfig.bibledit_web_user_get().c_str());

  button_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_url"));
  shortcuts.button (button_url);

  label_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_url"));
  gtk_label_set_text (GTK_LABEL (label_url), "");

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
  settings->genconfig.bibledit_web_user_set(gtk_entry_get_text (GTK_ENTRY (entry_user)));
}


void ReferenceExchangeDialog::on_url_test_clicked(GtkButton * button, gpointer user_data)
{
  ((ReferenceExchangeDialog *) user_data)->on_url_test();
}


void ReferenceExchangeDialog::on_url_test()
{
  GwSpawn spawn ("curl");
  spawn.arg ("-sS"); // Makes curl silent, but still show errors.
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
