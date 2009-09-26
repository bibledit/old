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
#include "dialogtextview.h"
#include "help.h"

TextViewDialog::TextViewDialog(const ustring & title, const ustring & info, bool editable, gchar * contents)
{
  textviewdialog = gtk_dialog_new();
  gtk_widget_set_size_request(textviewdialog, 640, 480);
  gtk_window_set_title(GTK_WINDOW(textviewdialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(textviewdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(textviewdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG(textviewdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  if (!info.empty()) {
    label1 = gtk_label_new(info.c_str());
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(dialog_vbox1), label1, FALSE, FALSE, 0);
    gtk_label_set_line_wrap(GTK_LABEL(label1), TRUE);
    gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);
  }

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), textview1);

  gtk_text_view_set_editable(GTK_TEXT_VIEW(textview1), editable);
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  if (contents)
    gtk_text_buffer_set_text(buffer, contents, -1);

  dialog_action_area1 = GTK_DIALOG(textviewdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(textviewdialog, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(textviewdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(textviewdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), gpointer(this));

  gtk_widget_grab_focus(textview1);
  gtk_widget_grab_default(okbutton1);
}

TextViewDialog::~TextViewDialog()
{
  gtk_widget_destroy(textviewdialog);
}

int TextViewDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(textviewdialog));
}

void TextViewDialog::on_okbutton1_clicked(GtkButton * button, gpointer user_data)
{
  ((TextViewDialog *) user_data)->on_okbutton();
}

void TextViewDialog::on_okbutton()
{
  GtkTextIter startiter, enditer;
  gtk_text_buffer_get_start_iter(buffer, &startiter);
  gtk_text_buffer_get_end_iter(buffer, &enditer);
  newcontents = gtk_text_buffer_get_text(buffer, &startiter, &enditer, true);
}

// Todo too tall?

