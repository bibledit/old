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


#include "dialoginterfaceprefs.h"
#include "help.h"
#include "shortcuts.h"
#include "settings.h"


InterfacePreferencesDialog::InterfacePreferencesDialog (int dummy)
{
  extern Settings * settings;
  Shortcuts shortcuts (0);
  
  interfaceprefsdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (interfaceprefsdialog), "Interface Preferences");
  gtk_window_set_position (GTK_WINDOW (interfaceprefsdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  
  dialog_vbox1 = GTK_DIALOG (interfaceprefsdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  checkbutton_xref_ref = gtk_check_button_new_with_mnemonic ("After inserting a cross reference, make the References tab visible in the Tools area");
  gtk_widget_show (checkbutton_xref_ref);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), checkbutton_xref_ref, FALSE, FALSE, 0);

  shortcuts.button (checkbutton_xref_ref);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_xref_ref), settings->genconfig.inserting_xref_shows_references_get ());

  dialog_action_area1 = GTK_DIALOG (interfaceprefsdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (interfaceprefsdialog, &shortcuts, NULL);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (interfaceprefsdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (interfaceprefsdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  shortcuts.stockbutton (cancelbutton1);
  shortcuts.stockbutton (okbutton1);
  shortcuts.process ();

  g_signal_connect ((gpointer) okbutton1, "clicked", G_CALLBACK (on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default (okbutton1);
}


InterfacePreferencesDialog::~InterfacePreferencesDialog ()
{
  gtk_widget_destroy (interfaceprefsdialog);
}


int InterfacePreferencesDialog::run()
{
  return gtk_dialog_run (GTK_DIALOG (interfaceprefsdialog));
}


void InterfacePreferencesDialog::on_okbutton1_clicked (GtkButton *button, gpointer user_data)
{
  ((InterfacePreferencesDialog *) user_data)->on_ok ();
}

void InterfacePreferencesDialog::on_ok ()
{
  extern Settings * settings;
  settings->genconfig.inserting_xref_shows_references_set (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_xref_ref)));
}
