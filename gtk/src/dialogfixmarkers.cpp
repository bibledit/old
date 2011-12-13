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

#include "libraries.h"
#include <glib.h>
#include "dialogfixmarkers.h"
#include "help.h"
#include "shortcuts.h"
#include "fixmarkers.h"

FixMarkersDialog::FixMarkersDialog(int dummy)
{
  Shortcuts shortcuts(0);

  fixmarkersdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(fixmarkersdialog), "Simple text corrections");
  gtk_window_set_position(GTK_WINDOW(fixmarkersdialog), GTK_WIN_POS_CENTER_ON_PARENT);

  dialog_vbox1 = GTK_DIALOG(fixmarkersdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label1 = gtk_label_new("This tool can make some automatic simple text corrections.");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), hseparator1, TRUE, TRUE, 0);

  checkbutton_space_after_opener = gtk_check_button_new_with_mnemonic("Insert a space after an opening USFM marker that doesn't have one");
  gtk_widget_show(checkbutton_space_after_opener);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton_space_after_opener, FALSE, FALSE, 0);

  shortcuts.button(checkbutton_space_after_opener);

  dialog_action_area1 = GTK_DIALOG(fixmarkersdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(fixmarkersdialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(fixmarkersdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(fixmarkersdialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_focus(okbutton);
  gtk_widget_grab_default(okbutton);
}

FixMarkersDialog::~FixMarkersDialog()
{
  gtk_widget_destroy(fixmarkersdialog);
}

int FixMarkersDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(fixmarkersdialog));
}

void FixMarkersDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((FixMarkersDialog *) user_data)->on_okbutton();
}

void FixMarkersDialog::on_okbutton()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_space_after_opener))) {
    fix_markers_insert_space_after_opener();
  }
}
