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
#include <glib/gi18n.h>

OutpostDialog::OutpostDialog(int dummy)
{
  // Save and initialize variables.

  // Build GUI.
  outpostdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(outpostdialog), _("Windows Outpost Setup"));
  gtk_window_set_position(GTK_WINDOW(outpostdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(outpostdialog));
  gtk_widget_show(dialog_vbox1);
  gtk_box_set_spacing (GTK_BOX (dialog_vbox1), 5);

  checkbutton_use = gtk_check_button_new_with_mnemonic (_("_Use Windows Outpost"));
  gtk_widget_show (checkbutton_use);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), checkbutton_use, FALSE, FALSE, 0);

  // Set whether to use the outpost.
  extern Settings *settings;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_use), settings->genconfig.use_outpost_get());

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(outpostdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(outpostdialog, NULL, NULL, "menu-preferences/windows-outpost");

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(outpostdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(outpostdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
}


OutpostDialog::~OutpostDialog()
{
  gtk_widget_destroy(outpostdialog);
}


int OutpostDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(outpostdialog));
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
}
