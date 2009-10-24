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


FloatingWindow::FloatingWindow(GtkWidget * layout_in)
// Base class for each floating window.
{
  layout = layout_in;
  dragging_window = false;
  resizing_window = false;
  clear_previous_root_coordinates ();

  box_x_position = 100;
  box_y_position = 30;
  box_width = 250;
  box_height = 120;

  gtkbuilder = gtk_builder_new ();
  gtk_builder_add_from_file (gtkbuilder, gw_build_filename (directories_get_package_data(), "gtkbuilder.floatingwindow.xml").c_str(), NULL);

  vbox_window = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbox_window"));
  gtk_layout_put (GTK_LAYOUT (layout), vbox_window, box_x_position, box_y_position);
  gtk_widget_set_size_request (vbox_window, box_width, box_height);

  label_title = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_title"));

  hscrollbar_title = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "hscrollbar_title"));
  GtkObject * adjustment = gtk_adjustment_new (0, 0, 1, 0, 0, 1);
  gtk_range_set_adjustment (GTK_RANGE (hscrollbar_title), GTK_ADJUSTMENT (adjustment));

  eventbox_actions = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "eventbox_actions"));

  label_actions = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_actions"));

  vbox_client = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "vbox_client"));

  label_status1 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_status1"));

  label_status2 = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_status2"));

  hscrollbar_status = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "hscrollbar_status"));

  g_signal_connect ((gpointer) hscrollbar_title, "button_press_event", G_CALLBACK (on_title_bar_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_title, "button_release_event", G_CALLBACK (on_title_bar_button_release_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_title, "motion_notify_event", G_CALLBACK (on_title_bar_motion_notify_event), gpointer (this));

  g_signal_connect ((gpointer) hscrollbar_title, "enter_notify_event", G_CALLBACK (on_titlebar_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_title, "leave_notify_event", G_CALLBACK (on_titlebar_leave_notify_event), gpointer (this));
  
  g_signal_connect ((gpointer) hscrollbar_status, "button_press_event", G_CALLBACK (on_status_bar_button_press_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "button_release_event", G_CALLBACK (on_status_bar_button_release_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "motion_notify_event", G_CALLBACK (on_status_bar_motion_notify_event), gpointer (this));

  g_signal_connect ((gpointer) hscrollbar_status, "enter_notify_event", G_CALLBACK (on_statusbar_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) hscrollbar_status, "leave_notify_event", G_CALLBACK (on_statusbar_leave_notify_event), gpointer (this));

  g_signal_connect ((gpointer) eventbox_actions, "enter_notify_event", G_CALLBACK (on_actions_enter_notify_event), gpointer (this));
  g_signal_connect ((gpointer) eventbox_actions, "leave_notify_event", G_CALLBACK (on_actions_leave_notify_event), gpointer (this));

}


FloatingWindow::~FloatingWindow()
{
  gtk_widget_destroy(vbox_window);
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
        box_x_position = box_x_position + event_x - previous_root_x;
        move_box = true;
      }
      if (event_y != previous_root_y) {
        box_y_position = box_y_position + event_y - previous_root_y;
        move_box = true;
      }
      if (move_box) {
        gtk_layout_move (GTK_LAYOUT (layout), vbox_window, box_x_position, box_y_position);
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
    gint event_x = event->x_root;
    gint event_y = event->y_root;
    if (previous_root_x >= 0) {
      bool resize_box = false;
      if (event_x != previous_root_x) {
        box_width = box_width + event_x - previous_root_x;
        resize_box = true;
      }
      if (event_y != previous_root_y) {
        box_height = box_height + event_y - previous_root_y;
        resize_box = true;
      }
      if (resize_box) {
        gtk_widget_set_size_request (vbox_window, box_width, box_height);
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
  GtkWidget *toplevel_widget = gtk_widget_get_toplevel(hscrollbar_title);
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
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(hscrollbar_title);
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


gboolean FloatingWindow::on_actions_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_actions_enter_notify(event);
}


gboolean FloatingWindow::on_actions_enter_notify (GdkEventCrossing *event)
{
  // Set the cursor to a shape that shows that the action label can be clicked.
  GtkWidget *toplevel_widget = gtk_widget_get_toplevel(label_actions);
  GdkWindow *gdk_window = toplevel_widget->window;
  GdkCursor *cursor = gdk_cursor_new(GDK_HAND2);
  gdk_window_set_cursor(gdk_window, cursor);
  gdk_cursor_unref (cursor);
  return false;
}


gboolean FloatingWindow::on_actions_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return ((FloatingWindow *) user_data)->on_actions_leave_notify(event);
}


gboolean FloatingWindow::on_actions_leave_notify (GdkEventCrossing *event)
{
  // Restore the original cursor.
  GtkWidget * toplevel_widget = gtk_widget_get_toplevel(label_actions);
  GdkWindow *gdk_window = toplevel_widget->window;
  gdk_window_set_cursor(gdk_window, NULL);
  return false;
}


