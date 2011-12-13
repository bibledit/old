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


#include "dialogyesnoalways.h"
#include "directories.h"
#include "gwrappers.h"
#include "settings.h"


bool yes_no_always_dialog (const ustring& message, YesNoAlwaysDialogType type, bool default_yes, bool default_always)
/*
This is a dialog that asks the user for either Yes or No, and optional the user can choose to always give this answer
for the remainder of the session.
message: message.
type : dialog type.
default_yes: Make the Yes button the default response for the dialog.
default_always: Set the dialog so that the answer the user now chooses is remembered for the rest of the session.
*/
{
  extern Settings * settings;
  int response = settings->session.yes_no_always_dialog_response[type];
  if (!settings->session.yes_no_always_dialog_enabled[type]) {
    YesNoAlwaysDialog dialog (message, default_yes);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialog.checkbutton), default_always);
    response = dialog.run ();
    settings->session.yes_no_always_dialog_enabled[type] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog.checkbutton));
    settings->session.yes_no_always_dialog_response[type] = response;
  }
  return (response == GTK_RESPONSE_YES);
}


YesNoAlwaysDialog::YesNoAlwaysDialog(const ustring& message, bool default_yes)
{
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.yesnoalwaysdialog.xml").c_str(), NULL);

  dialog = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "dialog"));

  label = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label"));
  gtk_label_set_text (GTK_LABEL (label), message.c_str());

  checkbutton = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "checkbutton"));

  // Add the buttons. In the standard question dialog, the Yes is at the right, then the No following to the left.
  gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_NO, GTK_RESPONSE_NO);
  gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_YES, GTK_RESPONSE_YES);
  
  if (default_yes)
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
  else
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_NO);
}


YesNoAlwaysDialog::~YesNoAlwaysDialog()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(dialog);
}


int YesNoAlwaysDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}


