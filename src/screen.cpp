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


#include "libraries.h"
#include "screen.h"
#include "settings.h"
#include "gwrappers.h"


void screen_scroll_to_iterator (GtkTextView *text_view, GtkTextIter *iter)
// Used for the editor, to get a position near the top of the screen.
{
  gtk_text_view_scroll_to_iter (text_view, iter, 0.1, true, 0, 0.3); 
}


void screen_set_cursor_hand_or_regular (GtkTextView *text_view, gint x, gint y)
// Looks at all tags covering the position (x, y) in the textview,
// and if one of them is a link, change the cursor to the "hands" cursor
// typically used by web browsers.
{
  // Static variables.
  static gboolean hovering_over_link = false;  
  GdkCursor *hand_cursor = gdk_cursor_new (GDK_HAND2);
  GdkCursor *regular_cursor = gdk_cursor_new (GDK_XTERM);

  // Do the job. 
  GSList *tags = NULL, *tagp = NULL;
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  gboolean hovering = FALSE;
  buffer = gtk_text_view_get_buffer (text_view);
  gtk_text_view_get_iter_at_location (text_view, &iter, x, y);
  tags = gtk_text_iter_get_tags (&iter);
  for (tagp = tags;  tagp != NULL;  tagp = tagp->next) {
    GtkTextTag *tag = (GtkTextTag *) tagp->data;
    gint id = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (tag), "id"));
    if (id != 0) {
      hovering = TRUE;
      break;
    }
  }
  if (hovering != hovering_over_link) {
    hovering_over_link = hovering;
    if (hovering_over_link)
      gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), hand_cursor);
    else
      gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), regular_cursor);
  }
  if (tags) 
    g_slist_free (tags);
}


gboolean screen_visibility_notify_event (GtkWidget *text_view, GdkEventVisibility *event, gpointer user_data)
// Update the cursor image if the window becomes visible
// (e.g. when a window covering it got iconified).
{
  gint wx, wy, bx, by;
  gdk_window_get_pointer (text_view->window, &wx, &wy, NULL);
  gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view), GTK_TEXT_WINDOW_WIDGET, wx, wy, &bx, &by);
  screen_set_cursor_hand_or_regular (GTK_TEXT_VIEW (text_view), bx, by);
  return false;
}


void dialog_position_save (DialogPositionType type, GtkWidget * dialog)
{
  // Get the window's position: x and y.
  gint x, y;
  gtk_window_get_position (GTK_WINDOW (dialog), &x, &y);
  
  // Get the stored positions of all dialogs.
  extern Settings * settings;
  vector <int> pos_x = settings->genconfig.dialogpositions_x_get ();
  vector <int> pos_y = settings->genconfig.dialogpositions_y_get ();
  
  // Possibly fill up the stored positions till it includes the location 
  // where to store this particular position.
  for (int i = pos_x.size(); i <= type; i++) {
    pos_x.push_back (-1);
  }
  for (int i = pos_y.size(); i <= type; i++) {
    pos_y.push_back (-1);
  }

  // Store the window position in the container: x and y.
  pos_x[type] = x;
  pos_y[type] = y;
  
  // Store the positions in the configuration.
  settings->genconfig.dialogpositions_x_set (pos_x);
  settings->genconfig.dialogpositions_y_set (pos_y);
}


void dialog_position_restore (DialogPositionType type, GtkWidget * dialog)
{
  // Get the positions of all dialogs.
  extern Settings * settings;
  vector <int> pos_x = settings->genconfig.dialogpositions_x_get ();
  vector <int> pos_y = settings->genconfig.dialogpositions_y_get ();
  
  // If the desired position is not (yet) there, bail out.
  if (type >= (int) pos_x.size()) return;
  if (type >= (int) pos_y.size()) return;

  // Get the desired position.
  int x = pos_x [type];
  int y = pos_y [type];

  // See if the dialog still is visible on the screen if it were positioned
  // like this. Let's say that at least 25% of width and height should be
  // visible on the screen.
  int window_width;
  int window_height;
  gtk_window_get_size (GTK_WINDOW (dialog), &window_width, &window_height);
  int minimum_x = 0 - (int) (0.75 * window_width);
  int minimum_y = 0 - (int) (0.75 * window_height);  
  int maximum_x = settings->genconfig.screen_width_get () - (int) (0.25 * window_width);
  int maximum_y = settings->genconfig.screen_height_get () - (int) (0.25 * window_height);
  
  // If the desired position is out of bounds, bail out.
  if (x < minimum_x) return;
  if (y < minimum_y) return;
  if (x > maximum_x) return;
  if (y > maximum_y) return;

  // The position has passed all test: now get the job done.
  gtk_window_move (GTK_WINDOW (dialog), pos_x [type], pos_y [type]);
}


void dialog_position_reset_all ()
{
  vector <int> dummy;
  extern Settings * settings;
  settings->genconfig.dialogpositions_x_set (dummy);
  settings->genconfig.dialogpositions_y_set (dummy);
}


void dialog_position_get_left_space (GtkWidget * dialog, gint& width, gint& height, gint& x, gint& y)
// Get the space on the screen left of the dialog.
{
  gint screen_width, screen_height;
  GdkScreen * screen = gtk_window_get_screen (GTK_WINDOW (dialog));
  screen_width = gdk_screen_get_width (screen);
  screen_height = gdk_screen_get_height (screen);

  gint dialog_width, dialog_height, dialog_x, dialog_y;
  gtk_window_get_size (GTK_WINDOW (dialog), &dialog_width, &dialog_height);
  gtk_window_get_position (GTK_WINDOW (dialog), &dialog_x, &dialog_y);
  
  width = dialog_width;
  if (width > dialog_x) width = dialog_x;
  height = dialog_height;
  x = dialog_x - width;
  y = dialog_y;
}
