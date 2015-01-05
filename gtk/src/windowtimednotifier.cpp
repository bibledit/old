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

#include "windowtimednotifier.h"
#include "gwrappers.h"
#include "directories.h"

TimedNotifierWindow::TimedNotifierWindow(const gchar * notification)
{
  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (Directories->get_package_data(), "gtkbuilder.timednotifywindow.xml").c_str(), NULL);
  window = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "window"));
  label = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label"));

  gtk_label_set_text (GTK_LABEL (label), notification);
  
  g_signal_connect((gpointer) window, "delete_event", G_CALLBACK(on_delete_event), gpointer(this));

  gtk_widget_show(window);
  gtk_window_present(GTK_WINDOW(window));

  g_timeout_add(1000, GSourceFunc(on_timeout), gpointer(this));
}

TimedNotifierWindow::~TimedNotifierWindow()
{
  g_object_unref (gtkbuilder);
  gtk_widget_destroy(window);
}

gboolean TimedNotifierWindow::on_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  // Window cannot be deleted.
  return true;
}

gboolean TimedNotifierWindow::on_timeout(gpointer data)
{
  return ((TimedNotifierWindow *) data)->timeout();
}

bool TimedNotifierWindow::timeout()
{
  delete this;
  return false;
}
