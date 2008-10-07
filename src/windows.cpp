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

#include "windows.h"
#include "settings.h"

void window_set_state(GtkWidget * window, WindowID id, bool startup)
// Sets the state of a window.
// window: pointer to the GtkWindow.
// id: the id of the window.
// initial: whether the windows shows at program startup.
  {
    // Settings.
    extern Settings * settings;

    // The positions of all the windows.
    vector<int> widths = settings->genconfig.window_widths_get();
    vector<int> heights = settings->genconfig.window_heights_get();
    vector<int> x_positions = settings->genconfig.window_x_positions_get();
    vector<int> y_positions = settings->genconfig.window_y_positions_get();

    // Our window's position.
    int width = 0;
    int height = 0;
    int x = 0;
    int y = 0;

    // Under certain circunstances the position of the window calculates a new state.
    if (widths.size() != heights.size())
      startup = false;
    if (heights.size() != x_positions.size())
      startup = false;
    if (x_positions.size() != y_positions.size())
      startup = false;
    if ((unsigned int)id >= widths.size())
      startup = false;
    if ((unsigned int)id >= heights.size())
      startup = false;
    if ((unsigned int)id >= x_positions.size())
      startup = false;
    if ((unsigned int)id >= y_positions.size())
      startup = false;

    // Position stored previous time.
    if (startup)
      {
        width = widths[id];
        height = heights[id];
        x = x_positions[id];
        y = y_positions[id];
      }

    // Zero values are not to be used.
    if (width == 0)
      startup = false;
    if (height == 0)
      startup = false;
    if (x == 0)
      startup = false;
    if (y == 0)
      startup = false;

    // Initial position calculation.
    if (!startup)
      {
        int tools_area_width = settings->genconfig.tools_area_width_get();
        int tools_area_height = settings->genconfig.tools_area_height_get();
        int tools_area_x = settings->genconfig.tools_area_x_position_get();
        int tools_area_y = settings->genconfig.tools_area_y_position_get();
        switch (id)
          {
        case widShowKeyterms:
          {
            width = tools_area_width;
            height = tools_area_height;
            x = tools_area_x;
            y = tools_area_y;
            break;
          }
          }
      }

    // Set the window's position.
    gtk_window_resize(GTK_WINDOW (window), width, height);
    gtk_window_move(GTK_WINDOW (window), x, y);

    // Store the showing state of the window.
    if (!startup)
      {
        vector<int> window_ids = settings->genconfig.window_ids_get();
        window_ids.push_back(id);
        settings->genconfig.window_ids_set(window_ids);
      }

  }

void window_store_state(GtkWidget * window, WindowID id)
  {
    // Settings.
    extern Settings * settings;

    // Window position.
    gint width, height, x, y;
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    gtk_window_get_position(GTK_WINDOW(window), &x, &y);

    // Store it.
    vector<int> widths = settings->genconfig.window_widths_get();
    for (int i = widths.size(); i <= id; i++)
      {
        widths.push_back(0);
      }
    widths[id] = width;
    settings->genconfig.window_widths_set(widths);

    vector<int> heights = settings->genconfig.window_heights_get();
    for (int i = heights.size(); i <= id; i++)
      {
        heights.push_back(0);
      }
    heights[id] = height;
    settings->genconfig.window_heights_set(heights);

    vector<int> x_positions = settings->genconfig.window_x_positions_get();
    for (int i = x_positions.size(); i <= id; i++)
      {
        x_positions.push_back(0);
      }
    x_positions[id] = x;
    settings->genconfig.window_x_positions_set(x_positions);

    vector<int> y_positions = settings->genconfig.window_y_positions_get();
    for (int i = y_positions.size(); i <= id; i++)
      {
        y_positions.push_back(0);
      }
    y_positions[id] = y;
    settings->genconfig.window_y_positions_set(y_positions);

    // Remove the window from the showing ones.
    vector<int> window_ids = settings->genconfig.window_ids_get();
    vector<int> new_ids;
    for (unsigned int i = 0; i < window_ids.size(); i++)
      {
        if (id != window_ids[i])
          new_ids.push_back(window_ids[i]);
      }
    settings->genconfig.window_ids_set(new_ids);
  }

