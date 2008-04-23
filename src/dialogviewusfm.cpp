/*
** Copyright (©) 2003-2008 Teus Benschop.
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


#include "dialogviewusfm.h"
#include "help.h"
#include "gtkwrappers.h"


ViewUSFMDialog::ViewUSFMDialog (const ustring& filename)
{
  viewusfmdialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (viewusfmdialog), "View USFM code");
  gtk_window_set_position (GTK_WINDOW (viewusfmdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_default_size (GTK_WINDOW (viewusfmdialog), 640, 480);
  gtk_window_set_type_hint (GTK_WINDOW (viewusfmdialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (viewusfmdialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  textview1 = gtk_text_view_new ();
  gtk_widget_show (textview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), textview1);

  dialog_action_area1 = GTK_DIALOG (viewusfmdialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp (viewusfmdialog, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (viewusfmdialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (viewusfmdialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) okbutton1, "clicked", G_CALLBACK (on_okbutton_clicked), gpointer (this));

  // Load the file.
  gchar *contents;
  g_file_get_contents (filename.c_str(), &contents, NULL, NULL);
  gtk_widget_set_sensitive (okbutton1, contents != NULL);
  textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview1));
  gtk_text_buffer_set_text (textbuffer, contents, -1);
  gtk_text_buffer_set_modified (textbuffer, false);
  if (contents) g_free (contents);  

  // Place cursor at the start.
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter (textbuffer, &iter);
  gtk_text_buffer_place_cursor (textbuffer, &iter);
  
  gtk_widget_grab_focus (textview1);
  gtk_widget_grab_default (okbutton1);
  
  changed = false;
  myfilename = filename;
}


ViewUSFMDialog::~ViewUSFMDialog ()
{
  gtk_widget_destroy (viewusfmdialog);
}


int ViewUSFMDialog::run ()
{
  return gtk_dialog_run (GTK_DIALOG (viewusfmdialog));
}


void ViewUSFMDialog::on_okbutton_clicked (GtkButton *button, gpointer user_data)
{
  ((ViewUSFMDialog *) user_data)->on_okbutton ();
}


void ViewUSFMDialog::on_okbutton ()
{
  if (gtk_text_buffer_get_modified (textbuffer)) {
    if (gtkw_dialog_question (viewusfmdialog, "Do you wish to save the changes?", GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      GtkTextIter startiter, enditer;
      gtk_text_buffer_get_start_iter (textbuffer, &startiter);      
      gtk_text_buffer_get_end_iter (textbuffer, &enditer);      
      g_file_set_contents (myfilename.c_str (), gtk_text_buffer_get_text (textbuffer, &startiter, &enditer, true), -1, NULL);
      changed = true;
    }
  }
}
