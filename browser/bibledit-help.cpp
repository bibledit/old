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


#include "bibledit-help.h"
#include <string>
#include <iostream>


int main (int argc, char *argv[])
{
#ifndef WIN32
  // Do not run as root.
  if (getuid () == 0)
    return 1;
#endif

  // Initialize bits.
  g_thread_init(NULL);
  g_type_init();
  gtk_init (&argc, &argv);

  // URL to load.
  gchar * url = NULL;
  if (argc > 1) url = argv[1];

  // Size and position.
  gint width = 640;
  if (argc > 2) width = strtol (argv[2], &argv[2], 0);
  gint height = 480;
  if (argc > 3) height = strtol (argv[3], &argv[3], 0);
  gint x = 0;
  if (argc > 4) x = strtol (argv[4], &argv[4], 0);
  gint y = 0;
  if (argc > 5) y = strtol (argv[5], &argv[5], 0);
  
  // Display the helpwindow.      
  helpwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (helpwindow), "Help");
  gtk_window_set_default_size (GTK_WINDOW (helpwindow), width, height);
  gtk_widget_show (helpwindow);
  gtk_window_move (GTK_WINDOW (helpwindow), x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (helpwindow), scrolledwindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  webview = webkit_web_view_new();
  gtk_widget_show (webview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), webview);
  
  // Optionally load the url.
  if (url) {
    webkit_web_view_open (WEBKIT_WEB_VIEW(webview), url);
  }
  
  // Connect signal handlers.
  g_signal_connect ((gpointer) helpwindow, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

  // Main loop.  
  gtk_main ();

  // Done.
  return 0;
}
