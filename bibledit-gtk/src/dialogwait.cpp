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
#include <gtk/gtk.h>
#include "dialogwait.h"


WaitDialog::WaitDialog(int milliseconds, int width, int height)
{
  waitdialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(waitdialog), "Please wait ...");
  gtk_window_set_position(GTK_WINDOW(waitdialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal(GTK_WINDOW(waitdialog), TRUE);
  if (width && height)
    gtk_window_set_default_size(GTK_WINDOW(waitdialog), width, height);

  dialog_vbox1 = GTK_DIALOG(waitdialog)->vbox;
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new("Please hold on ... processing data");
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG(waitdialog)->action_area;
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  g_timeout_add(milliseconds, GSourceFunc(static_on_timeout), gpointer(this));
}


WaitDialog::~WaitDialog()
{
  gtk_widget_destroy(waitdialog);
}


int WaitDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(waitdialog));
}


gboolean WaitDialog::static_on_timeout(gpointer data)
{
  return ((WaitDialog *) data)->on_timeout();
}


bool WaitDialog::on_timeout()
{
  gtk_widget_hide_all(waitdialog);
  gtk_dialog_response(GTK_DIALOG(waitdialog), GTK_RESPONSE_OK);
  return false;
}


