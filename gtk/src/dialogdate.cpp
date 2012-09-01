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
#include "dialogdate.h"
#include "date_time_utils.h"
#include "help.h"


DateDialog::DateDialog(guint32 * seconds_since_epoch, bool showtime) :
datewidget (seconds_since_epoch, showtime)
/*
By default this dialog shows the calendar only.
If showtime is true it shows the time also.
*/
{
  // Store variabeles.
  my_seconds_since_epoch = seconds_since_epoch;
  myshowtime = showtime;

  datedialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(datedialog), "Date");
  gtk_window_set_position(GTK_WINDOW(datedialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_type_hint(GTK_WINDOW(datedialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG(datedialog));
  gtk_widget_show(dialog_vbox1);

  gtk_box_pack_start(GTK_BOX(dialog_vbox1), datewidget.hbox, TRUE, TRUE, 0);

  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG(datedialog));
  gtk_widget_show(dialog_action_area1);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1), GTK_BUTTONBOX_END);

  new InDialogHelp(datedialog, NULL, NULL, NULL);

  cancelbutton1 = gtk_button_new_from_stock("gtk-cancel");
  gtk_widget_show(cancelbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(datedialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default (GTK_WIDGET (cancelbutton1), true);

  okbutton1 = gtk_button_new_from_stock("gtk-ok");
  gtk_widget_show(okbutton1);
  gtk_dialog_add_action_widget(GTK_DIALOG(datedialog), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (GTK_WIDGET (okbutton1), true);

  g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton_clicked), gpointer(this));

  gtk_widget_grab_default(okbutton1);
}


DateDialog::~DateDialog()
{
  gtk_widget_destroy(datedialog);
}


int DateDialog::run()
{
  return gtk_dialog_run(GTK_DIALOG(datedialog));
}


void DateDialog::on_okbutton_clicked(GtkButton * button, gpointer user_data)
{
  ((DateDialog *) user_data)->on_okbutton();
}


void DateDialog::on_okbutton()
{
}


