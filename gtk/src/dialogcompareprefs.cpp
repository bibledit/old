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
#include "dialogcompareprefs.h"
#include "help.h"
#include "settings.h"
#include <glib/gi18n.h>

ComparePreferencesDialog::ComparePreferencesDialog(int dummy)
{
  comparepreferencesdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (comparepreferencesdialog), _("Enter"));
  gtk_window_set_position (GTK_WINDOW (comparepreferencesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (comparepreferencesdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (comparepreferencesdialog));
  gtk_widget_show (dialog_vbox1);

  checkbutton_disregard_notes = gtk_check_button_new_with_mnemonic (_("Disregard changes in notes"));
  gtk_widget_show (checkbutton_disregard_notes);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), checkbutton_disregard_notes, FALSE, FALSE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(comparepreferencesdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(comparepreferencesdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(comparepreferencesdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(comparepreferencesdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);
  
  extern Settings * settings;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_disregard_notes), settings->genconfig.compare_disregard_notes_get());
}

ComparePreferencesDialog::~ComparePreferencesDialog()
{
  gtk_widget_destroy(comparepreferencesdialog);
}

int ComparePreferencesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(comparepreferencesdialog));
}

void ComparePreferencesDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((ComparePreferencesDialog *) user_data)->on_okbutton();
}

void ComparePreferencesDialog::on_okbutton()
{
  extern Settings * settings;
  settings->genconfig.compare_disregard_notes_set(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbutton_disregard_notes)));  
}

