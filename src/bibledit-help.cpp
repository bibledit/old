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


#include "bibledit-help.h"
#include <gtk/gtk.h>
#include <gtkmozembed.h>
#include "convert.h"


gboolean on_helpwindow_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  gtk_main_quit ();
  return false;
}


int main (int argc, char *argv[])
{
  // Do not run as root.
  if (getuid () == 0)
    return 1;

  // Initialize GTK
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
  GtkWidget *helpwindow;
  helpwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (helpwindow), "Help");
  gtk_window_set_default_size (GTK_WINDOW (helpwindow), width, height);
  gtk_widget_show (helpwindow);
  gtk_window_move (GTK_WINDOW (helpwindow), x, y);

  GtkWidget *mozembed;
  mozembed = gtk_moz_embed_new ();
  gtk_widget_show (mozembed);
  gtk_container_add (GTK_CONTAINER (helpwindow), mozembed);
  g_signal_connect ((gpointer) helpwindow, "delete_event", G_CALLBACK (on_helpwindow_delete_event), NULL);
  if (url) gtk_moz_embed_load_url (GTK_MOZ_EMBED (mozembed), url);

  // Main loop.  
  gtk_main ();

  // Done.
  return 0;
}
