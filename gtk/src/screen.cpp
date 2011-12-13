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
#include "screen.h"
#include "settings.h"
#include "gwrappers.h"
#include "settings.h"


void screen_scroll_to_iterator(GtkTextView * text_view, GtkTextIter * iter)
// Used for the editor, to get a position near the top of the screen.
{
  gtk_text_view_scroll_to_iter(text_view, iter, 0.1, true, 0, 0.3);
}


void textview_scroll_to_mark (GtkTextView * textview, GtkTextMark * mark, bool exact)
// Scrolls a "textview" so that the "mark" becomes visible on the screen.
// The advantage of this one as compared to the counterpart that scrolls to a GtkTextIter
// is that the scrolling to the mark can be called immediately after the textbuffer
// is filled, without any need to wait till the height of the lines has 
// been calculated.
// exact: scroll to the exact location - if false it scrolls as little as possible to get the mark visible.
{
  gtk_text_view_scroll_to_mark(textview, mark, 0.1, exact, 0, 0.3);
}


void dialog_position_save(DialogPositionType type, GtkWidget * dialog)
{
  // Get the window's position: x and y.
  gint x, y;
  gtk_window_get_position(GTK_WINDOW(dialog), &x, &y);

  // Get the stored positions of all dialogs.
  extern Settings *settings;
  vector < int >pos_x = settings->genconfig.dialogpositions_x_get();
  vector < int >pos_y = settings->genconfig.dialogpositions_y_get();

  // Possibly fill up the stored positions till it includes the location 
  // where to store this particular position.
  for (int i = pos_x.size(); i <= type; i++) {
    pos_x.push_back(-1);
  }
  for (int i = pos_y.size(); i <= type; i++) {
    pos_y.push_back(-1);
  }

  // Store the window position in the container: x and y.
  pos_x[type] = x;
  pos_y[type] = y;

  // Store the positions in the configuration.
  settings->genconfig.dialogpositions_x_set(pos_x);
  settings->genconfig.dialogpositions_y_set(pos_y);
}


void dialog_position_restore(DialogPositionType type, GtkWidget * dialog)
{
  // Get the positions of all dialogs.
  extern Settings *settings;
  vector < int >pos_x = settings->genconfig.dialogpositions_x_get();
  vector < int >pos_y = settings->genconfig.dialogpositions_y_get();

  // If the desired position is not (yet) there, bail out.
  if (type >= (int)pos_x.size())
    return;
  if (type >= (int)pos_y.size())
    return;

  // Get the desired position.
  int x = pos_x[type];
  int y = pos_y[type];

  // See if the dialog still is visible on the screen if it were positioned
  // like this. Let's say that at least 25% of width and height should be
  // visible on the screen.
  int window_width;
  int window_height;
  gtk_window_get_size(GTK_WINDOW(dialog), &window_width, &window_height);
  int minimum_x = 0 - (int)(0.75 * window_width);
  int minimum_y = 0 - (int)(0.75 * window_height);
  int maximum_x = settings->genconfig.screen_width_get() - (int)(0.25 * window_width);
  int maximum_y = settings->genconfig.screen_height_get() - (int)(0.25 * window_height);

  // If the desired position is out of bounds, bail out.
  if (x < minimum_x)
    return;
  if (y < minimum_y)
    return;
  if (x > maximum_x)
    return;
  if (y > maximum_y)
    return;

  // The position has passed all test: now get the job done.
  gtk_window_move(GTK_WINDOW(dialog), pos_x[type], pos_y[type]);
}


void dialog_position_reset_all()
{
  vector < int >dummy;
  extern Settings *settings;
  settings->genconfig.dialogpositions_x_set(dummy);
  settings->genconfig.dialogpositions_y_set(dummy);
}


void window_position_get_left_space(GtkWidget * widget, gint & width, gint & height, gint & x, gint & y)
// Get the space on the screen left of the widget.
{
  gint window_width, window_height, window_x, window_y;
  gtk_window_get_size(GTK_WINDOW(widget), &window_width, &window_height);
  gtk_window_get_position(GTK_WINDOW(widget), &window_x, &window_y);

  width = window_width;
  if (width > window_x)
    width = window_x;
  height = window_height;
  x = window_x - width;
  y = window_y;
}


void window_position_make_left_space(GtkWidget * widget, gint space)
// Tries to create a space of "space" at the left of the window.
{
  gint left_width, dummy;
  window_position_get_left_space(widget, left_width, dummy, dummy, dummy);
  gint window_x, window_y;
  gtk_window_get_position(GTK_WINDOW(widget), &window_x, &window_y);
  if (space > window_x) {
    gtk_window_move(GTK_WINDOW(widget), space, window_y);
  }
}


DialogPresenter::DialogPresenter(GtkWidget * widget)
{
  mywidget = widget;
  event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_timeout), gpointer(this), NULL);
}


DialogPresenter::~DialogPresenter()
{
  gw_destroy_source(event_id);
}


bool DialogPresenter::on_timeout(gpointer data)
{
  ((DialogPresenter *) data)->timeout();
  return false;
}


void DialogPresenter::timeout()
{
  gtk_window_present(GTK_WINDOW(mywidget));
}


DialogAutoScaler::DialogAutoScaler(GtkWidget * widget, int height)
{
  mywidget = widget;
  desired_height = height;
  event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 100, GSourceFunc(on_timeout), gpointer(this), NULL);
}


DialogAutoScaler::~DialogAutoScaler()
{
  gw_destroy_source(event_id);
}


bool DialogAutoScaler::on_timeout(gpointer data)
{
  ((DialogAutoScaler *) data)->timeout();
  return false;
}


void DialogAutoScaler::timeout()
{
  event_id = 0;

  while (gtk_events_pending()) gtk_main_iteration();

  extern Settings * settings;
  int screen_height = settings->genconfig.screen_height_get();  
  int maximum_height = 0.8 * screen_height;
  
  if (desired_height > maximum_height) {
    desired_height = maximum_height;
  }

  gint actual_height;
  gtk_window_get_size (GTK_WINDOW (mywidget), NULL, &actual_height);
  if (actual_height > maximum_height) {
    desired_height = maximum_height;
  }

  if (desired_height > 0) {
    gtk_widget_set_size_request (GTK_WIDGET (mywidget), -1, desired_height);
  }
}

