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
#include "dialogcheckbutton.h"
#include "help.h"
#include "utilities.h"


CheckbuttonDialog::CheckbuttonDialog(const ustring & title, const ustring & info, const vector < ustring > &labels, ustring bitpattern)
{
  Shortcuts shortcuts(0);

  checkbuttondialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(checkbuttondialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(checkbuttondialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(checkbuttondialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(checkbuttondialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  label = gtk_label_new(info.c_str());
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), label, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  for (unsigned int i = 0; i < labels.size(); i++) {

    GtkWidget *checkbutton;

    checkbutton = gtk_check_button_new_with_mnemonic(labels[i].c_str());
    gtk_widget_show(checkbutton);
    gtk_box_pack_start(GTK_BOX(dialog_vbox1), checkbutton, FALSE, FALSE, 0);

    checkbuttons.push_back(checkbutton);

    shortcuts.button(checkbutton);

    if (bitpattern_take(bitpattern)) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton), true);
    }

  }

  dialog_action_area1 = GTK_DIALOG(checkbuttondialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(checkbuttondialog, NULL, &shortcuts, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(checkbuttondialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(checkbuttondialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  shortcuts.stockbutton(cancelbutton);
  shortcuts.stockbutton(okbutton);
  shortcuts.process();

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton);
  gtk_widget_grab_focus(okbutton);
}


CheckbuttonDialog::~CheckbuttonDialog()
{
  gtk_widget_destroy(checkbuttondialog);
}


int CheckbuttonDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(checkbuttondialog));
}


void CheckbuttonDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((CheckbuttonDialog *) user_data)->on_okbutton();
}


void CheckbuttonDialog::on_okbutton()
{
  bitpattern.clear();
  for (unsigned int i = 0; i < checkbuttons.size(); i++) {
    bitpattern_add(bitpattern, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbuttons[i])));
  }
}


