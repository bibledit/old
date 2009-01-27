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
#include "htmlbrowser.h"
#include "help.h"
#include "directories.h"
#include "gwrappers.h"
#include <libxml/nanohttp.h>
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "utilities.h"
#include "tiny_utilities.h"
#include "screen.h"

void help_open(GtkButton * button, gpointer user_data)
{
  gchar *file = (gchar *) user_data;
  ustring url = "localhost:51516/olh_";
  url.append(file);
  url.append(".html");
  htmlbrowser(url, true);
}

InDialogHelp::InDialogHelp(GtkWidget * dialog, Shortcuts * shortcuts, const gchar * topic)
{
  // Save and initialize variables.
  mydialog = dialog;
  mytopic = topic;
  process_id = 0;

  // If no help is given, take a default one.
  if (!mytopic)
    mytopic = "none";

  helpbutton = gtk_toggle_button_new();
  gtk_widget_show(helpbutton);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area), helpbutton, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS(helpbutton, GTK_CAN_DEFAULT);

  alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
  gtk_widget_show(alignment);
  gtk_container_add(GTK_CONTAINER(helpbutton), alignment);

  hbox = gtk_hbox_new(FALSE, 2);
  gtk_widget_show(hbox);
  gtk_container_add(GTK_CONTAINER(alignment), hbox);

  image = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show(image);
  gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);

  label = gtk_label_new_with_mnemonic("_Help");
  gtk_widget_show(label);
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

  if (shortcuts)
    shortcuts->label(label);

  g_signal_connect((gpointer) helpbutton, "clicked", G_CALLBACK(on_helpbutton_activated), gpointer(this));
  g_signal_connect((gpointer) dialog, "delete_event", G_CALLBACK(on_dialog_delete_event), gpointer(this));
}

InDialogHelp::~InDialogHelp()
{
  if (process_id)
    unix_kill(process_id);
}

void InDialogHelp::on_helpbutton_activated(GtkButton * button, gpointer user_data)
{
  ((InDialogHelp *) user_data)->on_helpbutton();
}

void InDialogHelp::on_helpbutton()
{
  bool button_on = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(helpbutton));
  if (button_on) {

    // Calculate the position and size for the help viewer.
    gint width, height, x, y;
    window_position_get_left_space(mydialog, width, height, x, y);

    // Assemble the url to load.    
    ustring url = "http://localhost:51516/olh_";
    url.append(mytopic);
    url.append(".html");

    // Start the helpviewer.
    GwSpawn spawn("bibledit-help");
    spawn.arg(url);
    spawn.arg(convert_to_string(width));
    spawn.arg(convert_to_string(height));
    spawn.arg(convert_to_string(x));
    spawn.arg(convert_to_string(y));
    spawn.async();
    spawn.run();
    process_id = spawn.pid;

    // Present the window.
    g_usleep(500000);
    gtk_window_present(GTK_WINDOW(mydialog));

  } else {
    if (process_id) {
      unix_kill(process_id);
      process_id = 0;
    }
  }
}

gboolean InDialogHelp::on_dialog_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  // If the dialog gets deleted, then destroy the InDialogHelp object.
  InDialogHelp *this_is_me = (InDialogHelp *) user_data;
  delete this_is_me;
  return false;
}
