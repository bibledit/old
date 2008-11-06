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
#include "gwrappers.h"

void window_display(GtkWidget * window, WindowID id, const ustring& data, bool startup)
// Does the bookkeeping needed to display the window.
// window: pointer to the GtkWindow.
// id: the id of the window.
// data: the data attached to the window.
// startup: whether windows are handled at program startup.
{
  extern Settings * settings;

  // The parameters of all the windows.
  WindowData window_data(false);

  // The new window's position.
  GdkRectangle new_window_rectangle;
  new_window_rectangle.x = 0;
  new_window_rectangle.y = 0;
  new_window_rectangle.width = 0;
  new_window_rectangle.height = 0;

  // At this stage we take it that the windows shows at program startup.
  // We extract the position and size of the window from the general configuration.
  // It does not matter here whether the space for the window is already taken up by another window,
  // because the user wishes it to be so.
  for (unsigned int i = 0; i < window_data.widths.size(); i++) {
    if ((window_data.ids[i] == id) && (window_data.datas[i] == data) && startup) {
      new_window_rectangle.x = window_data.x_positions[i];
      new_window_rectangle.y = window_data.y_positions[i];
      new_window_rectangle.width = window_data.widths[i];
      new_window_rectangle.height = window_data.heights[i];
    }
  }

  // Reject null values, except for x or y, because the window might have positioned at 0,0 by the user.
  if ((new_window_rectangle.width == 0) || (new_window_rectangle.height == 0))
    startup = false;

  // When a new window needs to be allocated, there are a few steps to be taken.
  if (!startup) {

    // Step 1: The area rectangle where the window should fit in is defined, e.g. the tools area, or the text area. 
    GdkRectangle area_rectangle;
    area_rectangle.x = 0;
    area_rectangle.y = 0;
    area_rectangle.width = 0;
    area_rectangle.height = 0;
    extern Settings * settings;
    switch (id)
    {
      case widShowKeyterms:
      case widShowQuickReferences:
      case widMerge:
      case widResource:
      case widOutline:
      case widCheckKeyterms:
      case widStyles:
      {
        area_rectangle.x = settings->genconfig.tools_area_x_position_get();
        area_rectangle.y = settings->genconfig.tools_area_y_position_get();
        area_rectangle.width = settings->genconfig.tools_area_width_get();
        area_rectangle.height = settings->genconfig.tools_area_height_get();
        break;
      }
      case widNotes:
      {
        area_rectangle.x = settings->genconfig.notes_area_x_position_get();
        area_rectangle.y = settings->genconfig.notes_area_y_position_get();
        area_rectangle.width = settings->genconfig.notes_area_width_get();
        area_rectangle.height = settings->genconfig.notes_area_height_get();
        break;
      }
        /*
         int text_area_width = settings->genconfig.text_area_width_get();
         int text_area_height = settings->genconfig.text_area_height_get();
         int text_area_x = settings->genconfig.text_area_x_position_get();
         int text_area_y = settings->genconfig.text_area_y_position_get();
         */
    }

    // Step 2: A GdkRegion is made of that area.  
    GdkRegion * area_region = gdk_region_rectangle(&area_rectangle);

    // Step 3: GdkRegions are made of each of the open windows, and each region is subtracted from the area region.
    for (unsigned int i = 0; i < settings->session.open_windows.size(); i++) {
      GtkWindow * open_window = settings->session.open_windows[i];
      GdkRectangle rectangle;
      gtk_window_get_size(open_window, &rectangle.width, &rectangle.height);
      gtk_window_get_position(open_window, &rectangle.x, &rectangle.y);
      GdkRegion * region = gdk_region_rectangle(&rectangle);
      gdk_region_subtract(area_region, region);
      gdk_region_destroy(region);
    }

    // Step 4: The rectangles that the area region consists of are requested,
    // and the biggest suitable rectangle is chosen for the window.
    // A rectangle is considered suitable if it has at least 10% of the width, and 10% of the height of the area rectangle.
    GdkRectangle *gdk_rectangles= NULL;
    gint rectangle_count = 0;
    gdk_region_get_rectangles(area_region, &gdk_rectangles, &rectangle_count);
    for (int i = 0; i < rectangle_count; ++i) {
      GdkRectangle & rectangle = gdk_rectangles[i];
      if (rectangle.width >= (area_rectangle.width / 10)) {
        if (rectangle.height >= (area_rectangle.height / 10)) {
          if ((rectangle.width * rectangle.height) > (new_window_rectangle.width * new_window_rectangle.height)) {
            new_window_rectangle = rectangle;
          }
        }
      }
    }
    g_free(gdk_rectangles);

    // Step 5: The area region is destroyed.
    gdk_region_destroy(area_region);

    // Step 6: If no area big enough is found, then the window that takes most space in the area is chosen, 
    // the longest side is halved, and the new window is put in that freed area.
    if ((new_window_rectangle.width == 0) || (new_window_rectangle.height == 0)) {
      GtkWindow * biggest_window= NULL;
      int largest_intersection = 0;
      for (unsigned int i = 0; i < settings->session.open_windows.size(); i++) {
        GtkWindow * open_window = settings->session.open_windows[i];
        GdkRectangle window_rectangle;
        gtk_window_get_size(open_window, &window_rectangle.width, &window_rectangle.height);
        gtk_window_get_position(open_window, &window_rectangle.x, &window_rectangle.y);
        GdkRectangle intersection_rectangle;
        if (gdk_rectangle_intersect(&area_rectangle, &window_rectangle, &intersection_rectangle)) {
          int intersection = intersection_rectangle.width * intersection_rectangle.height;
          if (intersection > largest_intersection) {
            biggest_window = open_window;
            largest_intersection = intersection;
          }
        }
      }
      if (biggest_window) {
        GdkRectangle window_rectangle;
        gtk_window_get_size(biggest_window, &window_rectangle.width, &window_rectangle.height);
        gtk_window_get_position(biggest_window, &window_rectangle.x, &window_rectangle.y);
        new_window_rectangle = window_rectangle;
        if (window_rectangle.width > window_rectangle.height) {
          window_rectangle.width /= 2;
          new_window_rectangle.width /= 2;
          new_window_rectangle.x += window_rectangle.width;
        } else {
          window_rectangle.height /= 2;
          new_window_rectangle.height /= 2;
          new_window_rectangle.y += window_rectangle.height;
        }
        gtk_window_resize(GTK_WINDOW (biggest_window), window_rectangle.width, window_rectangle.height);
      }
    }
  }

  // Set the window's position if there's something to be set.
  if (new_window_rectangle.width && new_window_rectangle.height) {
    gtk_window_resize(GTK_WINDOW (window), new_window_rectangle.width, new_window_rectangle.height);
    gtk_window_move(GTK_WINDOW (window), new_window_rectangle.x, new_window_rectangle.y);
  }

  // Store a pointer to this window in the Session.
  settings->session.open_windows.push_back(GTK_WINDOW (window));
}

void window_delete(GtkWidget * window, WindowID id, const ustring& data, bool shutdown)
// Does the bookkeeping needed for deleting a window.
// When a window closes, the sizes of other windows are not affected. 
// Thus if the same window is opened again, it will go in the same free space as it was in before.
{
  // Window position.
  gint width, height, x, y;
  gtk_window_get_size(GTK_WINDOW(window), &width, &height);
  gtk_window_get_position(GTK_WINDOW(window), &x, &y);

  // Get the parameters of all the windows.
  WindowData window_data(true);

  // Ensure that the window has its entry in the settings.
  bool window_found = false;
  for (unsigned int i = 0; i < window_data.widths.size(); i++) {
    if ((window_data.ids[i] == id) && (window_data.datas[i] == data)) {
      window_found = true;
    }
  }
  if (!window_found) {
    window_data.x_positions.push_back(0);
    window_data.y_positions.push_back(0);
    window_data.widths.push_back(0);
    window_data.heights.push_back(0);
    window_data.ids.push_back(id);
    window_data.datas.push_back(data);
    window_data.shows.push_back(false);
  }

  // Set data for the window.
  for (unsigned int i = 0; i < window_data.ids.size(); i++) {
    if ((id == window_data.ids[i]) && (data == window_data.datas[i])) {
      // Set the position and size of the window.
      window_data.x_positions[i] = x;
      window_data.y_positions[i] = y;
      window_data.widths[i] = width;
      window_data.heights[i] = height;
      // The "showing" flag is set on program shutdown, else it is cleared.
      window_data.shows[i] = shutdown;
    }
  }

  // Remove the pointer to this window from the Session.
  GtkWindow * current_window= GTK_WINDOW (window);
  extern Settings * settings;
  vector <GtkWindow *> old_windows = settings->session.open_windows;
  vector <GtkWindow *> new_windows;
  for (unsigned int i = 0; i < old_windows.size(); i++) {
    if (current_window != old_windows[i]) {
      new_windows.push_back(old_windows[i]);
    }
  }
  settings->session.open_windows = new_windows;
}

WindowData::WindowData(bool save_on_destroy) {
  // Save variable.
  my_save_on_destroy = save_on_destroy;

  // Get the window data.
  extern Settings * settings;
  widths = settings->genconfig.window_widths_get();
  heights = settings->genconfig.window_heights_get();
  x_positions = settings->genconfig.window_x_positions_get();
  y_positions = settings->genconfig.window_y_positions_get();
  ids = settings->genconfig.window_ids_get();
  datas = settings->genconfig.window_datas_get();
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
  if (ids.size() != datas.size())
    data_consistent = false;
  if (datas.size() != shows.size())
    data_consistent = false;
  if (!data_consistent) {
    // Clear it all out.
    widths.clear();
    heights.clear();
    x_positions.clear();
    y_positions.clear();
    ids.clear();
    datas.clear();
    shows.clear();
  }
}

WindowData::~WindowData() {
  // Store the data.
  if (my_save_on_destroy) {
    extern Settings * settings;
    settings->genconfig.window_widths_set(widths);
    settings->genconfig.window_heights_set(heights);
    settings->genconfig.window_x_positions_set(x_positions);
    settings->genconfig.window_y_positions_set(y_positions);
    settings->genconfig.window_ids_set(ids);
    settings->genconfig.window_datas_set(datas);
    settings->genconfig.window_shows_set(shows);
  }
}

void WindowData::debug()
// Prints the state.
{
  cout << "WindowData object " << this << " state" << endl;
  for (unsigned int i = 0; i < widths.size(); i++) {
    cout << "window " << i << ", width " << widths[i] << ", height " << heights[i] << ", x " << x_positions[i] << ", y " << y_positions[i] << ", id " << ids[i] << ", data " << datas[i] << ", show " << shows[i] << endl;
  }
}

WindowBase::WindowBase(WindowID id, ustring data_title, GtkAccelGroup *accelerator_group, bool startup)
// Base class for each window.
{
  // If there's no title/data, then the configuration file gets inconsistent. Therefore put something there.
  if (data_title.empty()) {
    data_title.append("Untitled");
  }

  // Initialize variables.
  my_shutdown = false;
  act_on_focus_in_signal = true;
  focus_event_id = 0;
  window_id = id;
  window_data = data_title;
  focused_time = time(0);

  // Signalling buttons.
  focus_in_signal_button = gtk_button_new();
  delete_signal_button = gtk_button_new();

  // Craete the window.
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window), data_title.c_str());

  // Use accelerator in each window to make them look and feel the same.
  gtk_window_add_accel_group (GTK_WINDOW (window), accelerator_group);

  // The extra window should not appear in the taskbar in addition to the main window of Bibledit.
  // If it were to appear in the taskbar, then it looks as if there are many programs running.
  // And worse even, if it is in the taskbar, and a window gets focused so focusing all the other ones as well,
  // then the window in the taskbar would keep flashing.
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW (window), true);

  // Signal handlers.
  g_signal_connect ((gpointer) window, "focus_in_event", G_CALLBACK (on_window_focus_in_event), gpointer (this));
  g_signal_connect ((gpointer) window, "delete_event", G_CALLBACK (on_window_delete_event), gpointer (this));

  // The window needs to be positioned before it shows.
  window_display(window, window_id, window_data, startup);

  // Show it.
  gtk_widget_show_all(window);

  // Get the sizes of the frame that the window manager puts around the window.
  /*
   The size of the borders in pixels. There's the top border, the left, the right, and the bottom.
   We may have to get this setting when defining the areas, so that they are kept for future use.
   Or we may have to retrieve them from the main window, which probably is a better idea, so that
   they are automatically available all times even after changing the styles.
   We may have to see how to position them.
   GtkWidget * toplevel_widget = gtk_widget_get_toplevel(window);
   GdkWindow *gdk_window = toplevel_widget->window;
   gint x, y;
   gdk_window_get_origin(gdk_window, &x, &y);
   GdkRectangle gdk_rectangle;
   gdk_rectangle.x = 0;
   gdk_rectangle.y = 0;
   gdk_rectangle.width = 0;
   gdk_rectangle.height = 0;
   gdk_window_get_frame_extents(gdk_window, &gdk_rectangle);
   */
}

WindowBase::~WindowBase() {
  window_delete(window, window_id, window_data, my_shutdown);
  gtk_widget_destroy(window);
  gtk_widget_destroy(focus_in_signal_button);
  gtk_widget_destroy(delete_signal_button);
  gw_destroy_source(focus_event_id);
}

gboolean WindowBase::on_window_focus_in_event(GtkWidget *widget, GdkEventFocus *event, gpointer user_data) {
  ((WindowBase *) user_data)->on_window_focus_in(widget);
  return FALSE;
}

void WindowBase::on_window_focus_in(GtkWidget *widget) {
  if (act_on_focus_in_signal) {
    gtk_button_clicked(GTK_BUTTON (focus_in_signal_button));
    focused_time = time(0);
    /* 
    Finer-grained timing.
    The Linux kernel maintains a global variable called jiffies, 
    which represents the number of timer ticks since the machine started. 
    This variable is initialized to zero and increments each timer interrupt. 
    You can read jiffies with the get_jiffies_64 function, 
    and then convert this value to milliseconds (msec) with jiffies_to_msecs 
    or to microseconds (usec) with jiffies_to_usecs. 
    The jiffies' global and associated functions are provided in include/linux/jiffies.h.
    */
  }
}

bool WindowBase::focused()
{
  return GTK_WIDGET_HAS_FOCUS(window);
}

void WindowBase::present()
// Presents the window.
{
  // The window is going to be presented, and therefore will fire the "focus_in_event".
  // Don't act on this signal for a short while, lest the focusing goes on and on in an endless loop.
  act_on_focus_in_signal = false;
  // Restart the timeout.
  gw_destroy_source(focus_event_id);
  focus_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 1000, GSourceFunc (on_focus_timeout), gpointer(this), NULL);
  // Present the window.
  gtk_window_present(GTK_WINDOW (window));
}

bool WindowBase::on_focus_timeout(gpointer data) {
  ((WindowBase*) data)->focus_timeout();
  return false;
}

void WindowBase::focus_timeout() {
  // After a while the window should act on the "focus_in_event" again.
  act_on_focus_in_signal = true;
}

bool WindowBase::on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
  ((WindowBase *) user_data)->on_window_delete();
  return false;
}

void WindowBase::on_window_delete() {
  gtk_button_clicked(GTK_BUTTON (delete_signal_button));
}

void WindowBase::shutdown()
// Program shutdown.
{
  my_shutdown = true;
}

