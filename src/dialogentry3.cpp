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
#include "dialogentry3.h"
#include "help.h"

Entry3Dialog::Entry3Dialog(const string & title, bool allow_empty_values, const string & info1, const ustring & value1, const string & info2, const ustring & value2, const string & info3, const ustring & value3)
{
  // Save variables.
  my_allow_empty_values = allow_empty_values;

  entry3dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(entry3dialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(entry3dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(entry3dialog), TRUE);

  dialog_vbox1 = GTK_DIALOG(entry3dialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 2);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 2);

  label1 = gtk_label_new_with_mnemonic(info1.c_str());
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  entry1 = gtk_entry_new();
  gtk_widget_show(entry1);
  gtk_box_pack_start(GTK_BOX(vbox1), entry1, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);

  label2 = gtk_label_new_with_mnemonic(info2.c_str());
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(vbox1), label2, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  entry2 = gtk_entry_new();
  gtk_widget_show(entry2);
  gtk_box_pack_start(GTK_BOX(vbox1), entry2, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);

  label3 = gtk_label_new_with_mnemonic(info3.c_str());
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(vbox1), label3, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 0, 0.5);

  entry3 = gtk_entry_new();
  gtk_widget_show(entry3);
  gtk_box_pack_start(GTK_BOX(vbox1), entry3, FALSE, FALSE, 0);
  gtk_entry_set_activates_default(GTK_ENTRY(entry3), TRUE);

  dialog_action_area1 = GTK_DIALOG(entry3dialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(entry3dialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(entry3dialog), cancelbutton, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton, GTK_CAN_DEFAULT);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(entry3dialog), okbutton, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) entry1, "changed", G_CALLBACK(on_entry_changed), gpointer(this));
  g_signal_connect((gpointer) entry2, "changed", G_CALLBACK(on_entry_changed), gpointer(this));
  g_signal_connect((gpointer) entry3, "changed", G_CALLBACK(on_entry_changed), gpointer(this));

  gtk_label_set_mnemonic_widget(GTK_LABEL(label1), entry1);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label2), entry2);
  gtk_label_set_mnemonic_widget(GTK_LABEL(label3), entry3);

  if (!allow_empty_values)
    gtk_widget_set_sensitive(okbutton, false);

  gtk_entry_set_text(GTK_ENTRY(entry1), value1.c_str());
  gtk_entry_set_text(GTK_ENTRY(entry2), value2.c_str());
  gtk_entry_set_text(GTK_ENTRY(entry3), value3.c_str());

  gtk_widget_grab_focus(entry1);
  gtk_widget_grab_default(okbutton);
}

Entry3Dialog::~Entry3Dialog()
{
  gtk_widget_destroy(entry3dialog);
}

int Entry3Dialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(entry3dialog));
}

void Entry3Dialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((Entry3Dialog *) user_data)->on_okbutton();
}

void Entry3Dialog::on_okbutton()
{
}

void Entry3Dialog::on_entry_changed(GtkEditable * editable, gpointer user_data)
{
  ((Entry3Dialog *) user_data)->on_entry();
}

void Entry3Dialog::on_entry()
// Activates the ok button only when we've valuesin the entries.
{
  entered_value1 = gtk_entry_get_text(GTK_ENTRY(entry1));
  entered_value2 = gtk_entry_get_text(GTK_ENTRY(entry2));
  entered_value3 = gtk_entry_get_text(GTK_ENTRY(entry3));
  bool sensitive;
  sensitive = !(entered_value1.empty() || entered_value2.empty() || entered_value3.empty());
  if (my_allow_empty_values)
    sensitive = true;
  gtk_widget_set_sensitive(okbutton, sensitive);
}
