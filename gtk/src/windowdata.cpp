/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


#include "windowdata.h"
#include "settings.h"
#include "gwrappers.h"


WindowData::WindowData(bool save_on_destroy)
{
  // Save variable.
  my_save_on_destroy = save_on_destroy;

  // Get the window data.
  extern Settings *settings;
  widths = settings->genconfig.window_widths_get();
  heights = settings->genconfig.window_heights_get();
  x_positions = settings->genconfig.window_x_positions_get();
  y_positions = settings->genconfig.window_y_positions_get();
  ids = settings->genconfig.window_ids_get();
  titles = settings->genconfig.window_titles_get();
  shows = settings->genconfig.window_shows_get();

  // If the configuration file has been fiddled with, then it may occur that the data is not consistent.
  // Check on this.
  bool data_consistent = true;
  if (widths.size() != heights.size())
    data_consistent = false;
  if (heights.size() != x_positions.size())
    data_consistent = false;
  if (x_positions.size() != y_positions.size())
    data_consistent = false;
  if (y_positions.size() != ids.size())
    data_consistent = false;
  if (ids.size() != titles.size())
    data_consistent = false;
  if (titles.size() != shows.size())
    data_consistent = false;
  if (!data_consistent) {
    // Clear it all out.
    widths.clear();
    heights.clear();
    x_positions.clear();
    y_positions.clear();
    ids.clear();
    titles.clear();
    shows.clear();
    // And save it.
    my_save_on_destroy = true;
  }
}


WindowData::~WindowData()
{
  // Store the data.
  if (my_save_on_destroy) {
    extern Settings *settings;
    settings->genconfig.window_widths_set(widths);
    settings->genconfig.window_heights_set(heights);
    settings->genconfig.window_x_positions_set(x_positions);
    settings->genconfig.window_y_positions_set(y_positions);
    settings->genconfig.window_ids_set(ids);
    settings->genconfig.window_titles_set(titles);
    settings->genconfig.window_shows_set(shows);
  }
}


void WindowData::debug()
// Prints the state.
{
  cout << "WindowData object " << this << " state" << endl;
  for (unsigned int i = 0; i < widths.size(); i++) {
    cout << "window " << i << ", width " << widths[i] << ", height " << heights[i] << ", x " << x_positions[i] << ", y " << y_positions[i] << ", id " << ids[i] << ", title " << titles[i] << ", show " << shows[i] << endl;
  }
}


