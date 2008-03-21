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


#include "bibledit-restart.h"
#include <gtk/gtk.h>


gboolean signal_progress_timeout (gpointer user_data)
{
  static double fraction = 0;
  fraction += 0.005;
  if (fraction > 1) {
    gtk_main_quit ();
    return false;
  }
  GtkProgressBar * progressbar = GTK_PROGRESS_BAR (user_data);
  gtk_progress_bar_set_fraction (progressbar, fraction);
  return true;
}


int main (int argc, char *argv[])
{
  gtk_init (&argc, &argv);

  GtkWidget *progresswindow;
  progresswindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (progresswindow), "Bibledit");
  gtk_window_set_position (GTK_WINDOW (progresswindow), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (progresswindow), 300, 40);
  gtk_widget_show (progresswindow);
  
  GtkWidget *progressbar;
  progressbar = gtk_progress_bar_new ();
  gtk_widget_show (progressbar);
  gtk_container_add (GTK_CONTAINER (progresswindow), progressbar);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), "Restarting...");

  g_timeout_add (20, GSourceFunc (signal_progress_timeout), gpointer(progressbar));
  
  gtk_main ();

  return 0;
}
