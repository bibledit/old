/*
** Copyright (©) 2003-2015 Teus Benschop.
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


#include <executable/bibledit.h>
#include <libgen.h>
#include <iostream>
#include "library/bibledit.h"


int main (int argc, char *argv[])
{
  application = gtk_application_new ("org.bibledit.linux", G_APPLICATION_FLAGS_NONE);

  g_signal_connect (application, "activate", G_CALLBACK (activate), NULL);

  // Get the executable path and base the document root on it.
  char *linkname = (char *) malloc (256);
  if (readlink ("/proc/self/exe", linkname, 256)) {};
  string webroot = dirname (linkname);
  free (linkname);
  bibledit_initialize_library (webroot.c_str(), webroot.c_str());
  
  bibledit_start_library ();

  status = g_application_run (G_APPLICATION (application), argc, argv);

  g_object_unref (application);
  
  bibledit_stop_library ();

  while (bibledit_is_running ()) { };
  
  bibledit_shutdown_library ();

  return status;
}


void activate (GtkApplication *app)
{
  GList *list = gtk_application_get_windows (app);

  if (list) {
    gtk_window_present (GTK_WINDOW (list->data));
    return;
  }

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Bibledit");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_window_set_application (GTK_WINDOW (window), application);

  gchar * iconfile = g_build_filename ("bibledit.xpm", NULL);
  gtk_window_set_default_icon_from_file (iconfile, NULL);
  g_free (iconfile);

  g_signal_connect (window, "destroy", G_CALLBACK (on_signal_destroy), NULL);

  button = gtk_link_button_new_with_label ("http://localhost:8080", "Open Bibledit");
  gtk_container_add (GTK_CONTAINER (window), button);

  g_signal_connect ((gpointer) button, "clicked", G_CALLBACK(on_button_clicked), NULL);

  gtk_widget_show_all (window);

  gtk_main();
}


void on_button_clicked (GtkButton * button, gpointer user_data)
{
  if (user_data) {};
  if (button) {};
}


void on_signal_destroy (gpointer user_data)
{
  if (user_data) {};
  gtk_main_quit ();
}

