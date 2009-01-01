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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

void ScreenLayoutDimensions::verify()
// Set the dimensions in the screen.
{
  // Open configuration and get values.
  extern Settings *settings;
  // If the screen resolution got changed, or if the windows are too big, 
  // recalculate the values.
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

  int width, height, x, y;

  width = settings->genconfig.menu_area_width_get();
  height = settings->genconfig.menu_area_height_get();
  x = settings->genconfig.menu_area_x_position_get();
  y = settings->genconfig.menu_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  width = settings->genconfig.text_area_width_get();
  height = settings->genconfig.text_area_height_get();
  x = settings->genconfig.text_area_x_position_get();
  y = settings->genconfig.text_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

  width = settings->genconfig.notes_area_width_get();
  height = settings->genconfig.notes_area_height_get();
  x = settings->genconfig.notes_area_x_position_get();
  y = settings->genconfig.notes_area_y_position_get();
  if ((width == 0) || (height == 0) || (x < 0) || (y < 0) || (x + width > real_screen_width) || (y + height > real_screen_height))
    recalculate = true;

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

    // Menu area window at 0% from top, at left, width 50%, height 10%.
    width = real_screen_width * 50 / 100;
    height = real_screen_height * 10 / 100;
    x = 0;
    y = 0;
    settings->genconfig.menu_area_width_set(width);
    settings->genconfig.menu_area_height_set(height);
    settings->genconfig.menu_area_x_position_set(x);
    settings->genconfig.menu_area_y_position_set(y);

    // Text area window at 10% from top, at left, width 70%, height 60%.
    width = real_screen_width * 66 / 100;
    height = real_screen_height * 56 / 100;
    x = 0;
    y = real_screen_height * 14 / 100;
    settings->genconfig.text_area_width_set(width);
    settings->genconfig.text_area_height_set(height);
    settings->genconfig.text_area_x_position_set(x);
    settings->genconfig.text_area_y_position_set(y);

    // Notes area window at 70% from top, at left, width 70%, height 30%.
    width = real_screen_width * 66 / 100;
    height = real_screen_height * 16 / 100;
    x = 0;
    y = real_screen_height * 82 / 100;
    settings->genconfig.notes_area_width_set(width);
    settings->genconfig.notes_area_height_set(height);
    settings->genconfig.notes_area_x_position_set(x);
    settings->genconfig.notes_area_y_position_set(y);

    // Tools area window at top, 70% off left, width 30%, height 100%.
    width = real_screen_width * 26 / 100;
    height = real_screen_height * 90 / 100;
    x = real_screen_width - (real_screen_width * 30 / 100);
    y = 0;
    settings->genconfig.tools_area_width_set(width);
    settings->genconfig.tools_area_height_set(height);
    settings->genconfig.tools_area_x_position_set(x);
    settings->genconfig.tools_area_y_position_set(y);
  }
}
