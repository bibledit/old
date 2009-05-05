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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#include "bibledit-shutdown.h"
#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>


int main (int argc, char *argv[])
{
#ifndef WIN32
  // Do not run as root.
  if (getuid () == 0)
    return 1;
#endif

  // Database to read commands from.
  gchar * filename = NULL;
  if (argc > 1) filename = argv[1];
  if (filename == NULL) 
    return 1;
  if (!g_file_test(filename, G_FILE_TEST_IS_REGULAR))
    return 1;

  // Initialize Gtk.
  gtk_init (&argc, &argv);

  // Display the helpwindow.      
  splashscreen = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (splashscreen), "Bibledit");
  gtk_window_set_position (GTK_WINDOW (splashscreen), GTK_WIN_POS_CENTER);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (splashscreen), TRUE);
  gtk_window_set_skip_pager_hint (GTK_WINDOW (splashscreen), TRUE);
  gtk_window_set_type_hint (GTK_WINDOW (splashscreen), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
  gtk_window_set_gravity (GTK_WINDOW (splashscreen), GDK_GRAVITY_CENTER);

  GtkWidget * vbox = gtk_vbox_new (FALSE, 10);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (splashscreen), vbox);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 50);

  GtkWidget * label = gtk_label_new ("Tidying up");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  progressbar = gtk_progress_bar_new ();
  gtk_widget_show (progressbar);
  gtk_box_pack_start (GTK_BOX (vbox), progressbar, FALSE, FALSE, 0);

  gtk_widget_show_all (splashscreen);
    
  g_signal_connect ((gpointer) splashscreen, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

  while (gtk_events_pending())
    gtk_main_iteration();

  // Read databases to vacuum.
  ifstream in(filename);
  if (!in) {
    return 1;
  }
  vector <string> files;
  {
    string file;
    while (getline(in, file)) {
      files.push_back(file);
    }
  }
  for (unsigned int i = 0; i < files.size(); i++) {
    double fraction = ((double) i) / ((double) files.size());
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), fraction);
    while (gtk_events_pending())
      gtk_main_iteration();
    string filename = files[i];
    if (filename.empty())
      continue;
    sqlite3 *db;
    int rc;
    char *error = NULL;
    try {
      rc = sqlite3_open(filename.c_str(), &db);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      sqlite3_busy_timeout(db, 2000);
      rc = sqlite3_exec(db, "vacuum;", NULL, NULL, &error);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
    }
    catch(exception & ex) {
      g_critical("%s", ex.what());
    }
    sqlite3_close(db);
  }

  //gtk_main ();

  return 0;
}
