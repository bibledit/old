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
#include "dialogprojectnote.h"
#include "help.h"
#include "combobox.h"

ProjectNoteDialog::ProjectNoteDialog(GtkWidget * parent, const vector < ustring > &projects_in, const ustring & project_in, const ustring & created_on, const ustring & created_by, const ustring & edited_on, const ustring & logbook)
{
  dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Project note");
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(dialog), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));

  dialog_vbox1 = GTK_DIALOG(dialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label_note_project = gtk_label_new_with_mnemonic("Pro_ject");
  gtk_widget_show(label_note_project);
  gtk_box_pack_start(GTK_BOX(vbox1), label_note_project, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_project), 0, 0.5);

  combobox_note_project = gtk_combo_box_new_text();
  gtk_widget_show(combobox_note_project);
  gtk_box_pack_start(GTK_BOX(vbox1), combobox_note_project, FALSE, FALSE, 0);

  label_note_created_on = gtk_label_new("Created on");
  gtk_widget_show(label_note_created_on);
  gtk_box_pack_start(GTK_BOX(vbox1), label_note_created_on, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_created_on), 0, 0.5);

  label_note_created_by = gtk_label_new("Created by");
  gtk_widget_show(label_note_created_by);
  gtk_box_pack_start(GTK_BOX(vbox1), label_note_created_by, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_created_by), 0, 0.5);

  label_note_edited_on = gtk_label_new("Edited on");
  gtk_widget_show(label_note_edited_on);
  gtk_box_pack_start(GTK_BOX(vbox1), label_note_edited_on, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_edited_on), 0, 0.5);

  label_note_logbook = gtk_label_new("Logbook");
  gtk_widget_show(label_note_logbook);
  gtk_box_pack_start(GTK_BOX(vbox1), label_note_logbook, FALSE, FALSE, 0);
  gtk_misc_set_alignment(GTK_MISC(label_note_logbook), 0, 0.5);

  textview_note_logbook = gtk_text_view_new();
  gtk_widget_show(textview_note_logbook);
  gtk_box_pack_start(GTK_BOX(vbox1), textview_note_logbook, TRUE, TRUE, 0);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_note_logbook), FALSE);
  gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(textview_note_logbook), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview_note_logbook), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview_note_logbook), FALSE);

  dialog_action_area1 = GTK_DIALOG(dialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(dialog, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(dialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  gtk_label_set_mnemonic_widget(GTK_LABEL(label_note_project), combobox_note_project);

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);

  combobox_set_strings(combobox_note_project, projects_in);
  combobox_set_string(combobox_note_project, project_in);
  gtk_label_set_text(GTK_LABEL(label_note_created_by), created_by.c_str());
  gtk_label_set_text(GTK_LABEL(label_note_created_on), created_on.c_str());
  gtk_label_set_text(GTK_LABEL(label_note_edited_on), edited_on.c_str());
  GtkTextBuffer *textbuffer_logbook = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_note_logbook));
  gtk_text_buffer_set_text(textbuffer_logbook, logbook.c_str(), -1);

}

ProjectNoteDialog::~ProjectNoteDialog()
{
  gtk_widget_destroy(dialog);
}

int ProjectNoteDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(dialog));
}

void ProjectNoteDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((ProjectNoteDialog *) user_data)->on_okbutton1();
}

void ProjectNoteDialog::on_okbutton1()
{
  project = combobox_get_active_string(combobox_note_project);
}


