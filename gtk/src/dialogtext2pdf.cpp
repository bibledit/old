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
#include "dialogtext2pdf.h"
#include "help.h"
#include "utilities.h"

Text2PdfDialog::Text2PdfDialog(vector < ustring > *commands)
// Dialog to show the commands used in the text2pdf object.
{
  mycommands = commands;

  text2pdfdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(text2pdfdialog), "Formatter commands");
  gtk_window_set_position(GTK_WINDOW(text2pdfdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(text2pdfdialog), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (text2pdfdialog));
  gtk_widget_show(dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_widget_set_size_request(scrolledwindow1, 800, 600);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new();
  gtk_widget_show(textview1);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), textview1);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(text2pdfdialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(text2pdfdialog, NULL, NULL, NULL);

  cancelbutton = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(text2pdfdialog), cancelbutton, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton), true);

  okbutton = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton);
  gtk_dialog_add_action_widget(GTK_DIALOG(text2pdfdialog), okbutton, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton), true);

  g_signal_connect((gpointer) okbutton, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));
  g_signal_connect((gpointer) text2pdfdialog, "delete_event", G_CALLBACK(on_text2pdfdialog_delete_event), gpointer(this));

  // Set text.
  // Loading a huge chunk of text would take a long time.
  // Temporally removing the buffer from the view speeds it up a lot.
  ustring text;
  for (unsigned int i = 0; i < mycommands->size(); i++) {
    text.append(mycommands->at(i));
    text.append("\n");
  }
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  g_object_ref(buffer);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview1), NULL);
  gtk_text_buffer_set_text(buffer, text.c_str(), -1);
  gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview1), buffer);
  g_object_unref(buffer);

  // Place cursor at start of buffer.
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(buffer, &iter);
  gtk_text_buffer_place_cursor(buffer, &iter);

  // Set buffer unmodified so as to prevent time-consuming actions being taken on it if there was no change.
  gtk_text_buffer_set_modified(buffer, false);

  gtk_widget_grab_focus(textview1);
  gtk_widget_grab_default(okbutton);
}

Text2PdfDialog::~Text2PdfDialog()
{
  gtk_widget_destroy(text2pdfdialog);
}

int Text2PdfDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(text2pdfdialog));
}

void Text2PdfDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((Text2PdfDialog *) user_data)->on_okbutton();
}

void Text2PdfDialog::on_okbutton()
// Ok handler.
{
  // Get text if it was modified.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  if (gtk_text_buffer_get_modified(buffer)) {
    vector < ustring > commands;
    textbuffer_get_lines(buffer, commands, false);
    mycommands->clear();
    for (unsigned int i = 0; i < commands.size(); i++) {
      mycommands->push_back(commands[i]);
    }
  }
  on_text2pdfdialog_delete();
}

gboolean Text2PdfDialog::on_text2pdfdialog_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
// Event handler.
{
  ((Text2PdfDialog *) user_data)->on_text2pdfdialog_delete();
  return FALSE;
}

void Text2PdfDialog::on_text2pdfdialog_delete()
// Event handler.
{
  // The following line hugely speeds up the destruction of the textview.
  // Without that it takes a long time if a lot of text is loaded in the buffer.
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
  gtk_text_buffer_set_text(buffer, "", -1);
}
