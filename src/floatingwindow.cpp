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


#include "floatingwindow.h"
#include "settings.h"
#include "gwrappers.h"
#include "directories.h"
#include "dialogradiobutton.h"


FloatingWindow::FloatingWindow(GtkWidget * layout_in, WindowID window_id_in, ustring title_in, bool startup)
// Base class for each floating window.
{
  // If there's no title the configuration file would get inconsistent. 
  // Put something there.
  if (title_in.empty()) {
    title_in.append("Untitled");
  }

  // Initialize variables.
  layout = layout_in;
  title = title_in;
  window_id = window_id_in;
  dragging_window = false;
  resizing_window = false;
  my_shutdown = false;
  clear_previous_root_coordinates ();
  last_focused_widget = NULL;
  focused = false;
  resize_event_id = 0;
    
  // Signalling buttons.
  focus_in_signal_button = gtk_button_new();
  delete_signal_button = gtk_button_new();

  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.floatingwindow.xml").c_str(), NULL);

  vbox_window = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbox_window"));

  GtkWidget *eventbox_title;
  eventbox_title = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "eventbox_title"));
  label_title = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_title"));
  title_set (focused);
  g_signal_connect ((gpointer) eventbox_title, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_title, "button_press_event", G_CALLBACK (on_title_bar_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_title, "button_release_event", G_CALLBACK (on_title_bar_button_release_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_title, "motion_notify_event", G_CALLBACK (on_title_bar_motion_notify_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_title, "enter_notify_event", G_CALLBACK (on_titlebar_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_title, "leave_notify_event", G_CALLBACK (on_titlebar_leave_notify_event), gpointer (this));
    
  GtkWidget *eventbox_close;
  eventbox_close = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "eventbox_close"));
  label_close = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_close"));
  g_signal_connect ((gpointer) eventbox_close, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_close, "enter_notify_event", G_CALLBACK (on_label_close_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_close, "leave_notify_event", G_CALLBACK (on_label_close_leave_notify_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_close, "button_press_event", G_CALLBACK (on_label_close_button_press_event), gpointer (this));

  GtkWidget *eventbox_client;
  eventbox_client = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "eventbox_client"));
  vbox_client = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbox_client"));
  g_signal_connect ((gpointer) eventbox_client, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));

  GtkWidget *eventbox_status1;
  eventbox_status1 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "eventbox_status1"));
  label_status1 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_status1"));
  g_signal_connect ((gpointer) eventbox_status1, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));

  GtkWidget *eventbox_status2;
  eventbox_status2 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "eventbox_status2"));
  label_status2 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_status2"));
  g_signal_connect ((gpointer) eventbox_status2, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));

  hscrollbar_status = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "hscrollbar_status"));
  g_signal_connect ((gpointer) hscrollbar_status, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "button_press_event", G_CALLBACK (on_status_bar_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "button_release_event", G_CALLBACK (on_status_bar_button_release_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "motion_notify_event", G_CALLBACK (on_status_bar_motion_notify_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "enter_notify_event", G_CALLBACK (on_statusbar_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "leave_notify_event", G_CALLBACK (on_statusbar_leave_notify_event), gpointer (this));

  // Do the display handling.
  display(startup);
}


FloatingWindow::~FloatingWindow()
{
  gw_destroy_source (resize_event_id);
  on_titlebar_leave_notify (NULL);
  gtk_widget_destroy(vbox_window);
  undisplay();
  gtk_widget_destroy(focus_in_signal_button);
  gtk_widget_destroy(delete_signal_button);
}


gboolean FloatingWindow::on_title_bar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_title_bar_button_press(event);
}


gboolean FloatingWindow::on_title_bar_button_press (GdkEventButton *event)
{
  clear_previous_root_coordinates ();
  dragging_window = true;
  resizing_window = false;
  return false;
}


gboolean FloatingWindow::on_title_bar_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_title_bar_button_release(event);
}


gboolean FloatingWindow::on_title_bar_button_release (GdkEventButton *event)
{
  dragging_window = false;
  resizing_window = false;
  clear_previous_root_coordinates ();
  return false;
}


gboolean FloatingWindow::on_title_bar_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_title_bar_motion_notify(event);
}


gboolean FloatingWindow::on_title_bar_motion_notify (GdkEventMotion *event)
{
  if (dragging_window) {
    gint event_x = event->x_root;
    gint event_y = event->y_root;
    if (previous_root_x >= 0) {
      bool move_box = false;
      if (event_x != previous_root_x) {
        my_gdk_rectangle.x = my_gdk_rectangle.x + event_x - previous_root_x;
        move_box = true;
      }
      if (event_y != previous_root_y) {
        my_gdk_rectangle.y = my_gdk_rectangle.y + event_y - previous_root_y;
        move_box = true;
      }
      if (move_box) {
        rectangle_set (my_gdk_rectangle);
      }
    }
    previous_root_x = event_x;
    previous_root_y = event_y;
  }
  return false;
}


gboolean FloatingWindow::on_status_bar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_status_bar_button_press(event);
}


gboolean FloatingWindow::on_status_bar_button_press (GdkEventButton *event)
{
  clear_previous_root_coordinates ();
  dragging_window = false;
  resizing_window = true;
  return false;
}


gboolean FloatingWindow::on_status_bar_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_status_bar_button_release(event);
}


gboolean FloatingWindow::on_status_bar_button_release (GdkEventButton *event)
{
  dragging_window = false;
  resizing_window = false;
  clear_previous_root_coordinates ();
  return false;
}


gboolean FloatingWindow::on_status_bar_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_status_bar_motion_notify(event);
}


gboolean FloatingWindow::on_status_bar_motion_notify (GdkEventMotion *event)
{
  if (resizing_window) {
    gw_destroy_source (resize_event_id);
    resize_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 200, GSourceFunc(on_resize_timeout), gpointer(this), NULL);
    gtk_widget_hide (vbox_client);
    gint event_x = event->x_root;
    gint event_y = event->y_root;
    if (previous_root_x >= 0) {
      bool resize_box = false;
      if (event_x != previous_root_x) {
        my_gdk_rectangle.width = my_gdk_rectangle.width + event_x - previous_root_x;
        resize_box = true;
      }
      if (event_y != previous_root_y) {
        my_gdk_rectangle.height = my_gdk_rectangle.height + event_y - previous_root_y;
        resize_box = true;
      }
      if (resize_box) {
        gtk_widget_set_size_request (vbox_window, my_gdk_rectangle.width, my_gdk_rectangle.height);
      }
    }
    previous_root_x = event_x;
    previous_root_y = event_y;
  }
  return false;
}


gboolean FloatingWindow::on_titlebar_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_titlebar_enter_notify(event);
}


gboolean FloatingWindow::on_titlebar_enter_notify (GdkEventCrossing *event)
{
  // Set the cursor to a shape that shows that the title bar can be moved around.
  GtkWidget *toplevel_widget = gtk_widget_get_toplevel(label_title);
  GdkWindow *gdk_window = toplevel_widget->window;
  GdkCursor *cursor = gdk_cursor_new(GDK_FLEUR);
  gdk_window_set_cursor(gdk_window, cursor);
  gdk_cursor_unref (cursor);
  return false;
}


gboolean FloatingWindow::on_titlebar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_titlebar_leave_notify(event);
}


gboolean FloatingWindow::on_titlebar_leave_notify (GdkEventCrossing *event)
{
  // Restore the original cursor.
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(label_title);
  GdkWindow *gdk_window = toplevel_widget->window;
  gdk_window_set_cursor(gdk_window, NULL);
  return false;
}


gboolean FloatingWindow::on_statusbar_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_statusbar_enter_notify(event);
}


gboolean FloatingWindow::on_statusbar_enter_notify (GdkEventCrossing *event)
{
  // Set the cursor to a shape that shows that the status bar can be used to resize the window.
  GtkWidget *toplevel_widget = gtk_widget_get_toplevel(hscrollbar_status);
  GdkWindow *gdk_window = toplevel_widget->window;
  GdkCursor *cursor = gdk_cursor_new(GDK_BOTTOM_RIGHT_CORNER);
  gdk_window_set_cursor(gdk_window, cursor);
  gdk_cursor_unref (cursor);
  return false;
}


gboolean FloatingWindow::on_statusbar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_statusbar_leave_notify(event);
}


gboolean FloatingWindow::on_statusbar_leave_notify (GdkEventCrossing *event)
{
  // Restore the original cursor.
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(hscrollbar_status);
  GdkWindow *gdk_window = toplevel_widget->window;
  gdk_window_set_cursor(gdk_window, NULL);
  return false;
}


void FloatingWindow::clear_previous_root_coordinates ()
{
  previous_root_x = -1;
  previous_root_y = -1;
}


gboolean FloatingWindow::on_label_close_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_label_close_enter_notify(event);
}


gboolean FloatingWindow::on_label_close_enter_notify (GdkEventCrossing *event)
{
  // Set the cursor to a shape that shows that the action label can be clicked.
  GtkWidget *toplevel_widget = gtk_widget_get_toplevel(label_close);
  GdkWindow *gdk_window = toplevel_widget->window;
  GdkCursor *cursor = gdk_cursor_new(GDK_HAND2);
  gdk_window_set_cursor(gdk_window, cursor);
  gdk_cursor_unref (cursor);
  return false;
}


gboolean FloatingWindow::on_label_close_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_label_close_leave_notify(event);
}


gboolean FloatingWindow::on_label_close_leave_notify (GdkEventCrossing *event)
{
  // Restore the original cursor.
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(label_close);
  GdkWindow *gdk_window = toplevel_widget->window;
  gdk_window_set_cursor(gdk_window, NULL);
  return false;
}


gboolean FloatingWindow::on_label_close_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_label_close_button_press(event);
}


gboolean FloatingWindow::on_label_close_button_press (GdkEventButton *event)
{
  gtk_button_clicked(GTK_BUTTON(delete_signal_button));
  return false;
}


void FloatingWindow::display(bool startup)
// Does the bookkeeping necessary for displaying the floating box.
// startup: whether the box is started at program startup.
{
  // Settings.
  extern Settings *settings;

  // The parameters of all the windows.
  WindowData window_parameters(false);

  // Clear the new window's position.
  my_gdk_rectangle.x = 0;
  my_gdk_rectangle.y = 0;
  my_gdk_rectangle.width = 0;
  my_gdk_rectangle.height = 0;

  // At program startup extract the position and size of the window from the general configuration.
  // It does not matter whether the space for the window is already taken up by another window,
  // because the user wishes to use the coordinates that he has set for this window.
  for (unsigned int i = 0; i < window_parameters.widths.size(); i++) {
    if ((window_parameters.ids[i] == window_id) && (window_parameters.titles[i] == title) && startup) {
      my_gdk_rectangle.x = window_parameters.x_positions[i];
      my_gdk_rectangle.y = window_parameters.y_positions[i];
      my_gdk_rectangle.width = window_parameters.widths[i];
      my_gdk_rectangle.height = window_parameters.heights[i];
    }
  }

  // Reject zero width and zero height values on startup.
  if ((my_gdk_rectangle.width == 0) || (my_gdk_rectangle.height == 0))
    startup = false;

  // When a new window needs to be allocated, there are a few steps to be taken.
  if (!startup) {

    // Step 1: The area rectangle where the window should fit in is defined. 
    GdkRectangle area_rectangle;
    area_rectangle.x = 0;
    area_rectangle.y = 0;
    area_rectangle.width = 0;
    area_rectangle.height = 0;
    {
      guint width, height;
      gtk_layout_get_size (GTK_LAYOUT (layout), &width, &height);
      area_rectangle.width = width;
      area_rectangle.height = height;
    }

    // Step 2: An available region is made of that whole area.
    GdkRegion *available_region = gdk_region_rectangle(&area_rectangle);

    // Step 3: The regions of each of the open windows is substracted from the available region.
    for (unsigned int i = 0; i < settings->session.open_floating_windows.size(); i++) {
      FloatingWindow * floating_window = (FloatingWindow *) settings->session.open_floating_windows[i];
      GdkRectangle rectangle = floating_window->rectangle_get();
      GdkRegion *region = gdk_region_rectangle(&rectangle);
      gdk_region_subtract(available_region, region);
      gdk_region_destroy(region);
    }

    // Step 4: The rectangles that the area region consists of are requested,
    // and the biggest suitable rectangle is chosen for the window.
    // A rectangle is considered suitable if it has at least 10% of the width, and 10% of the height of the area rectangle.
    GdkRectangle *gdk_rectangles = NULL;
    gint rectangle_count = 0;
    gdk_region_get_rectangles(available_region, &gdk_rectangles, &rectangle_count);
    for (int i = 0; i < rectangle_count; ++i) {
      GdkRectangle & rectangle = gdk_rectangles[i];
      if (rectangle.width >= (area_rectangle.width / 10)) {
        if (rectangle.height >= (area_rectangle.height / 10)) {
          if ((rectangle.width * rectangle.height) > (my_gdk_rectangle.width * my_gdk_rectangle.height)) {
            my_gdk_rectangle = rectangle;
          }
        }
      }
    }
    g_free(gdk_rectangles);

    // Step 5: The available region is destroyed.
    gdk_region_destroy(available_region);

    // Step 6: If no area big enough is found, then the window that takes most space in the area is chosen, 
    // the longest side is halved, and the new window is put in that freed area.
    if ((my_gdk_rectangle.width == 0) || (my_gdk_rectangle.height == 0)) {
      FloatingWindow * resize_window_pointer = NULL;
      int largest_size = 0;
      for (unsigned int i = 0; i < settings->session.open_floating_windows.size(); i++) {
        FloatingWindow *floating_window = (FloatingWindow *) settings->session.open_floating_windows[i];
        GdkRectangle rectangle = floating_window->rectangle_get ();
        int size = rectangle.width * rectangle.height;
        if (size > largest_size) {
          resize_window_pointer = floating_window;
          largest_size = size;
        }
      }
      if (resize_window_pointer) {
        GdkRectangle resize_window_rectangle = resize_window_pointer->rectangle_get();
        my_gdk_rectangle = resize_window_pointer->rectangle_get();
        if (resize_window_rectangle.width > resize_window_rectangle.height) {
          resize_window_rectangle.width /= 2;
          my_gdk_rectangle.width /= 2;
          my_gdk_rectangle.x += resize_window_rectangle.width;
        } else {
          resize_window_rectangle.height /= 2;
          my_gdk_rectangle.height /= 2;
          my_gdk_rectangle.y += resize_window_rectangle.height;
        }
        resize_window_pointer->rectangle_set (resize_window_rectangle);
      }
    }
  }
  // Add the window to the layout and set its position and size.
  gtk_layout_put (GTK_LAYOUT (layout), vbox_window, my_gdk_rectangle.x, my_gdk_rectangle.y);
  rectangle_set (my_gdk_rectangle);
  // Store a pointer to this window in the Session.
  settings->session.open_floating_windows.push_back(gpointer (this));
}


void FloatingWindow::undisplay()
// Does the bookkeeping needed for deleting a box.
// When a box closes, the sizes of other boxes are not affected. 
// Thus if the same window is opened again, it will go in the same free space as it was in before.
{
  // Get the parameters of all the windows.
  WindowData window_params(true);

  // Ensure that the window has its entry in the settings.
  bool window_found = false;
  for (unsigned int i = 0; i < window_params.widths.size(); i++) {
    if ((window_params.ids[i] == window_id) && (window_params.titles[i] == title)) {
      window_found = true;
    }
  }
  if (!window_found) {
    window_params.x_positions.push_back(0);
    window_params.y_positions.push_back(0);
    window_params.widths.push_back(0);
    window_params.heights.push_back(0);
    window_params.ids.push_back(window_id);
    window_params.titles.push_back(title);
    window_params.shows.push_back(false);
  }
  // Set data for the window.
  for (unsigned int i = 0; i < window_params.ids.size(); i++) {
    if ((window_id == window_params.ids[i]) && (title == window_params.titles[i])) {
      // Set the position and size of the window.
      window_params.x_positions[i] = my_gdk_rectangle.x;
      window_params.y_positions[i] = my_gdk_rectangle.y;
      window_params.widths[i] = my_gdk_rectangle.width;
      window_params.heights[i] = my_gdk_rectangle.height;
      // The "showing" flag is set on program shutdown, else it is cleared.
      window_params.shows[i] = my_shutdown;
    }
  }

  // Remove the pointer to this window from the Session.
  gpointer current_floating_window = gpointer (this);
  extern Settings *settings;
  vector <gpointer>old_windows = settings->session.open_floating_windows;
  vector <gpointer>new_windows;
  for (unsigned int i = 0; i < old_windows.size(); i++) {
    if (current_floating_window != old_windows[i]) {
      new_windows.push_back(old_windows[i]);
    }
  }
  settings->session.open_floating_windows = new_windows;
}


void FloatingWindow::shutdown()
// Program shutdown.
{
  my_shutdown = true;
}


void FloatingWindow::focus_set(bool active)
// Sets the focus of the window.
{
  // Bail out if there's no focus change.
  if (active == focused) {
    return;
  }
  // Store whether focused.
  focused = active;
  // If we focus, then grab the widget that was focused last.
  if (active) {
    if (last_focused_widget) {
      gtk_widget_grab_focus (last_focused_widget);
    }
  }
  // Update title bar.
  title_set (focused);
  // Set the window on top of any others that share same intersection.
  // It has been observed that widgets that are last added to the layout are shown on top of any others.
  // Therefore remove the window from the layout, and add it again so that it becomes the last one added.
  if (active) {
    // The following works to set the window above others, but the by-effects are undesirable,
    // therefore it is better at this stage to not do that.
    // One of the by-effects is that the selection in the editor gets lost.
    // Another one is that the comboboxes get greyed out.
    // g_object_ref (G_OBJECT (vbox_window));
    // gtk_container_remove (GTK_CONTAINER (layout), vbox_window);
    // gtk_layout_put (GTK_LAYOUT (layout), vbox_window, my_gdk_rectangle.x, my_gdk_rectangle.y);
    // g_object_unref (G_OBJECT (vbox_window));
  }
  // If we got focus, then alert the other windows.
  if (active) {
    gtk_button_clicked(GTK_BUTTON(focus_in_signal_button));
  }
}


GdkRectangle FloatingWindow::rectangle_get ()
{
  return my_gdk_rectangle;
}


void FloatingWindow::rectangle_set (const GdkRectangle& rectangle)
{
  my_gdk_rectangle = rectangle;
  gtk_layout_move (GTK_LAYOUT (layout), vbox_window, my_gdk_rectangle.x, my_gdk_rectangle.y);
  gtk_widget_set_size_request (vbox_window, my_gdk_rectangle.width, my_gdk_rectangle.height); 
}


void FloatingWindow::focus_if_widget_mine (GtkWidget *widget)
// It looks through all widgets it has, to find out whether "widget" belongs to the object.
{
  focused_widget_to_look_for = widget;
  if (GTK_IS_CONTAINER(vbox_window)) {
    gtk_container_foreach(GTK_CONTAINER(vbox_window), on_container_tree_callback, gpointer(this));
  }
}


void FloatingWindow::on_container_tree_callback (GtkWidget *widget, gpointer user_data)
{
  ((FloatingWindow *) user_data)->container_tree_callback(widget, user_data);
}


void FloatingWindow::container_tree_callback (GtkWidget *widget, gpointer user_data)
// Recursive callback that fires the focus signal if the widget belongs to the object.
{
  if (widget == focused_widget_to_look_for) {
    last_focused_widget = widget;
    focus_set ();
  }
  if (GTK_IS_CONTAINER(widget)) {
    gtk_container_foreach(GTK_CONTAINER(widget), on_container_tree_callback, user_data);
  }
}


gboolean FloatingWindow::on_widget_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  ((FloatingWindow *) user_data)->on_widget_button_press (widget, event);
  return FALSE;
}


void FloatingWindow::on_widget_button_press (GtkWidget *widget, GdkEventButton *event)
{
  focus_set ();
}


void FloatingWindow::title_set (bool focused)
// Set the title.
{
  ustring spaces;
  for (unsigned int i = 0; i < 500; i++) 
    spaces.append (" ");
  char *data;
  data = g_strdup_printf("<span foreground=\"%s\" background=\"%s\" weight=\"%s\">     %s     %s</span>", 
                         focused ? "white" : "black", 
                         focused ? "blue" : "grey",
                         focused ? "bold" : "normal",
                         title.c_str(), 
                         spaces.c_str());
  gtk_label_set_markup (GTK_LABEL (label_title), data);
  g_free(data);
}


void FloatingWindow::on_widget_grab_focus(GtkWidget * widget, gpointer user_data)
{
  ((FloatingWindow *) user_data)->widget_grab_focus(widget);
}


void FloatingWindow::widget_grab_focus(GtkWidget * widget)
{
  if (widget != last_focused_widget) 
    focus_set ();
  last_focused_widget = widget;
}


void FloatingWindow::connect_focus_signals (GtkWidget * widget)
// Connects relevant focus signals of "widget".
{
  // When the user presses a mouse button in a widget, it should focus.
  g_signal_connect ((gpointer) widget, "button_press_event", G_CALLBACK (on_widget_button_press_event), gpointer (this));
  // When a widget has grabbed focus, it should store this state for later use.
  g_signal_connect_after((gpointer) widget, "grab_focus", G_CALLBACK(on_widget_grab_focus), gpointer(this));
  
}


void FloatingWindow::status1 (const ustring& text)
{
  gtk_label_set_text (GTK_LABEL (label_status1), text.c_str());
}


void FloatingWindow::status2 (const ustring& text)
{
  gtk_label_set_text (GTK_LABEL (label_status2), text.c_str());
}


bool FloatingWindow::on_resize_timeout (gpointer data)
{
  ((FloatingWindow *) data)->resize_timeout();
  return false;
}


void FloatingWindow::resize_timeout ()
/*
The USFM editor may take a lot of time resizing.
This makes the GUI unresponsive.
The solution is the following:
When resizing starts, the client area gets hidden.
After resizing has stopped for a while, the client area gets shown again.
*/
{
  resize_event_id = 0;
  gtk_widget_show (vbox_client);
}

