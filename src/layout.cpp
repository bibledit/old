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

#include "layout.h"
#include "gtkwrappers.h"
#include "settings.h"
#include "gui_features.h"
#include "screen.h"
#include "gwrappers.h"

ScreenLayoutDimensions::ScreenLayoutDimensions(GtkWidget * window)
{
  mywindow = GTK_WINDOW(window);
  counter = 0;
}

ScreenLayoutDimensions::~ScreenLayoutDimensions()
{
}

void ScreenLayoutDimensions::verify()
// Set the dimensions in the screen.
{
  // Open configuration and get values.
  extern Settings *settings;
  
  // If the screen resolution got changed, or if the windows are too big, recalculate the values.
  bool recalculate = false;
  GdkScreen *screen = gtk_window_get_screen(mywindow);
  int real_screen_width = gdk_screen_get_width(screen);
  int stored_screen_width = settings->genconfig.screen_width_get();
  int real_screen_height = gdk_screen_get_height(screen);
  int stored_screen_height = settings->genconfig.screen_height_get();
  if (real_screen_width != stored_screen_width)
    recalculate = true;
  if (real_screen_height != stored_screen_height)
    recalculate = true;

  // Variables.
  int width, height, x, y;

  // Attached windows: Window size.
  width = settings->genconfig.window_width_get();
  height = settings->genconfig.window_height_get();
  x = settings->genconfig.window_x_position_get();
  y = settings->genconfig.window_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  // Detached windows: Menu area.
  width = settings->genconfig.menu_area_width_get();
  height = settings->genconfig.menu_area_height_get();
  x = settings->genconfig.menu_area_x_position_get();
  y = settings->genconfig.menu_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  // Detached windows: Text area.
  width = settings->genconfig.text_area_width_get();
  height = settings->genconfig.text_area_height_get();
  x = settings->genconfig.text_area_x_position_get();
  y = settings->genconfig.text_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  // Detached windows: Notes area.
  width = settings->genconfig.notes_area_width_get();
  height = settings->genconfig.notes_area_height_get();
  x = settings->genconfig.notes_area_x_position_get();
  y = settings->genconfig.notes_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  // Detached windows: Tools area.
  width = settings->genconfig.tools_area_width_get();
  height = settings->genconfig.tools_area_height_get();
  x = settings->genconfig.tools_area_x_position_get();
  y = settings->genconfig.tools_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  if (recalculate) {
    gw_message("Recalculating windows positions and sizes");
    // Store screen size.
    settings->genconfig.screen_width_set(real_screen_width);
    settings->genconfig.screen_height_set(real_screen_height);

    // Remove stored dialog positions.
    dialog_position_reset_all();

    // Attached windows: Window sizes.
    width = real_screen_width * 80 / 100;
    height = real_screen_height * 70 / 100;
    x = 0;
    y = 0;
    settings->genconfig.window_width_set(width);
    settings->genconfig.window_height_set(height);
    settings->genconfig.window_x_position_set(x);
    settings->genconfig.window_y_position_set(y);

    // Detached windows: Menu area window sizes.
    width = real_screen_width * 20 / 100;
    height = real_screen_height * 5 / 100;
    x = 0;
    y = 0;
    settings->genconfig.menu_area_width_set(width);
    settings->genconfig.menu_area_height_set(height);
    settings->genconfig.menu_area_x_position_set(x);
    settings->genconfig.menu_area_y_position_set(y);

    // Detached windows: Text area window sizes.
    width = real_screen_width * 66 / 100;
    height = real_screen_height * 50 / 100;
    x = 0;
    y = real_screen_height * 16 / 100;
    settings->genconfig.text_area_width_set(width);
    settings->genconfig.text_area_height_set(height);
    settings->genconfig.text_area_x_position_set(x);
    settings->genconfig.text_area_y_position_set(y);

    // Detached windows: Notes area window dimensions.
    width = real_screen_width * 66 / 100;
    height = real_screen_height * 16 / 100;
    x = 0;
    y = real_screen_height * 82 / 100;
    settings->genconfig.notes_area_width_set(width);
    settings->genconfig.notes_area_height_set(height);
    settings->genconfig.notes_area_x_position_set(x);
    settings->genconfig.notes_area_y_position_set(y);

    // Detached windows: Tools area window sizes.
    width = real_screen_width * 30 / 100;
    height = real_screen_height * 90 / 100;
    x = real_screen_width - (real_screen_width * 30 / 100);
    y = 0;
    settings->genconfig.tools_area_width_set(width);
    settings->genconfig.tools_area_height_set(height);
    settings->genconfig.tools_area_x_position_set(x);
    settings->genconfig.tools_area_y_position_set(y);
  }
}


void ScreenLayoutDimensions::apply()
// Applies the stored dimensions to the main window.
{
  // The dimension are applies with a delay so as to give the window a change to settle.
  g_timeout_add(100, GSourceFunc(on_timeout), gpointer(this));
}


void ScreenLayoutDimensions::save()
// Stores the dimensions of the main window.
{
  // Settings.
  extern Settings * settings;
  
  // Maximized state.
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(GTK_WIDGET (mywindow));
  GdkWindow *gdk_window = toplevel_widget->window;
  GdkWindowState window_state = gdk_window_get_state (gdk_window);  
  bool maximized = window_state & GDK_WINDOW_STATE_MAXIMIZED;
  settings->genconfig.window_maximized_set (maximized);

  // Save window dimensions if the window is not maximized.
  if (!maximized) {
    gint width, height, x, y;
    gtk_window_get_size (mywindow, &width, &height);
    gtk_window_get_position (mywindow, &x, &y);
    settings->genconfig.window_width_set (width);
    settings->genconfig.window_height_set (height);
    settings->genconfig.window_x_position_set (x);
    settings->genconfig.window_y_position_set (y);
  }
}

bool ScreenLayoutDimensions::on_timeout(gpointer data)
{
  ((ScreenLayoutDimensions *) data)->timeout();
  return false;
}

void ScreenLayoutDimensions::timeout()
// Apply the dimensions after a timeout.
{
  extern Settings * settings;
  gint width = settings->genconfig.window_width_get ();
  gint height = settings->genconfig.window_height_get ();
  gint x = settings->genconfig.window_x_position_get ();
  gint y = settings->genconfig.window_y_position_get ();
  gtk_window_resize (mywindow, width, height);
  gtk_window_move (mywindow, x, y);
  if (settings->genconfig.window_maximized_get ()) {
    gtk_window_maximize(GTK_WINDOW (mywindow));
  }
  delete this;
}

