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
#include "dialogentry.h"
#include "help.h"


EntryDialog::EntryDialog(const ustring & title, const ustring & info, const ustring & value)
{
  my_always_ok = false;

  entrydialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(entrydialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(entrydialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(entrydialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(entrydialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label = gtk_label_new(info.c_str());
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(vbox1), label, FALSE, FALSE, 4);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

  entry = gtk_entry_new();
  gtk_widget_show(entry);
  gtk_box_pack_start(GTK_BOX(vbox1), entry, FALSE, FALSE, 4);
  gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

  dialog_action_area1 = GTK_DIALOG(entrydialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(entrydialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(entrydialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(entrydialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  gtk_widget_set_sensitive(okbutton, false);

  g_signal_connect((gpointer) entry, "changed", G_CALLBACK(on_entry_changed), gpointer(this));
  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  // Set text and widget size.
  gtk_entry_set_text(GTK_ENTRY(entry), value.c_str());
  gtk_entry_set_width_chars(GTK_ENTRY(entry), value.length());
  on_entry();

  gtk_widget_grab_focus(entry);
  gtk_widget_grab_default(okbutton);
}


EntryDialog::~EntryDialog()
{
  gtk_widget_destroy(entrydialog);
}


int EntryDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(entrydialog));
}


void EntryDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((EntryDialog *) user_data)->on_okbutton();
}


void EntryDialog::on_okbutton()
{
  entered_value = gtk_entry_get_text(GTK_ENTRY(entry));
}


void EntryDialog::on_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((EntryDialog *) user_data)->on_entry();
}


void EntryDialog::on_entry()
// Activates the ok button only when we've a value in the entry or 
// when it is "always_ok".
{
  string value = gtk_entry_get_text(GTK_ENTRY(entry));
  gtk_widget_set_sensitive(okbutton, !value.empty() || my_always_ok);
}


void EntryDialog::text_invisible()
// This function makes the text in the entry invisible.
{
  gtk_entry_set_visibility(GTK_ENTRY(entry), false);
}


void EntryDialog::always_ok()
{
  my_always_ok = true;
  on_entry();
}


