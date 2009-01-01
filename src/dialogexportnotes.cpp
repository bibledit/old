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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "libraries.h"
#include "dialogexportnotes.h"
#include "help.h"

ExportNotesDialog::ExportNotesDialog(int dummy)
{
  exportnotesdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(exportnotesdialog), "Export Notes");
  gtk_window_set_position(GTK_WINDOW(exportnotesdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(exportnotesdialog), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(exportnotesdialog), TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(exportnotesdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(exportnotesdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox1), 4);

  filechooserwidget1 = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_SAVE);
  gtk_widget_show(filechooserwidget1);
  gtk_box_pack_start(GTK_BOX(vbox1), filechooserwidget1, TRUE, TRUE, 4);
  g_object_set(filechooserwidget1, "local-only", FALSE, NULL);

  hseparator1 = gtk_hseparator_new();
  gtk_widget_show(hseparator1);
  gtk_box_pack_start(GTK_BOX(vbox1), hseparator1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Save in format");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  GSList *radiobutton1_group = NULL;

  radiobutton1 = gtk_radio_button_new_with_mnemonic(NULL, "_Bibledit Notes version 3");
  gtk_widget_show(radiobutton1);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton1, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton1), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton1));

  radiobutton2 = gtk_radio_button_new_with_mnemonic(NULL, "_ScriptureNotes version 2.0");
  gtk_widget_show(radiobutton2);
  gtk_box_pack_start(GTK_BOX(vbox1), radiobutton2, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(radiobutton2), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radiobutton2));

  dialog_action_area1 = GTK_DIALOG(exportnotesdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(exportnotesdialog, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(exportnotesdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(exportnotesdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);
}

ExportNotesDialog::~ExportNotesDialog()
{
  gtk_widget_destroy(exportnotesdialog);
}

int
 ExportNotesDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(exportnotesdialog));
}

void ExportNotesDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((ExportNotesDialog *) user_data)->on_okbutton();
}

void ExportNotesDialog::on_okbutton()
{
  // Get filename to save notes to.
  gchar *name;
  name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserwidget1));
  if (name)
    filename = name;
  g_free(name);
  // Get export format.
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1)))
    exportnotesformat = BibleditVersion3;
  else
    exportnotesformat = ScriptureNotesVersion20;
  save_all_notes = true;
}
