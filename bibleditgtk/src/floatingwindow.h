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


#ifndef INCLUDED_FLOATING_WINDOW_H
#define INCLUDED_FLOATING_WINDOW_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "windowdata.h"


class FloatingWindow
{
public:
  FloatingWindow(GtkWidget * layout_in, WindowID window_id_in, ustring title_in, bool startup);
  // Virtual method since this object will be the base for other objects.
  virtual ~FloatingWindow();
private:
  GtkWidget * layout;
  GtkBuilder * gtkbuilder;
  GtkWidget *vbox_window;
public:
  GtkWidget *label_title;
private:
  GtkWidget *label_close;
public:
  GtkWidget *vbox_client;
private:
  GtkWidget *label_status1;
  GtkWidget *label_status2;
  GtkWidget *hscrollbar_status;
  void clear_previous_root_coordinates ();
  gint previous_root_x;
  gint previous_root_y;
  static gboolean on_title_bar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_title_bar_button_press (GdkEventButton *event);
  static gboolean on_title_bar_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_title_bar_button_release (GdkEventButton *event);
  static gboolean on_title_bar_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
  gboolean on_title_bar_motion_notify (GdkEventMotion *event);
  bool dragging_window;
  static gboolean on_status_bar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_status_bar_button_press (GdkEventButton *event);
  static gboolean on_status_bar_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_status_bar_button_release (GdkEventButton *event);
  static gboolean on_status_bar_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
  gboolean on_status_bar_motion_notify (GdkEventMotion *event);
  bool resizing_window;

  // Cursor control.
  static gboolean on_titlebar_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_titlebar_enter_notify (GdkEventCrossing *event);
  static gboolean on_titlebar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_titlebar_leave_notify (GdkEventCrossing *event);
  static gboolean on_statusbar_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_statusbar_enter_notify (GdkEventCrossing *event);
  static gboolean on_statusbar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_statusbar_leave_notify (GdkEventCrossing *event);
  static gboolean on_label_close_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_label_close_enter_notify (GdkEventCrossing *event);
  static gboolean on_label_close_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_label_close_leave_notify (GdkEventCrossing *event);

  // Close link.
  static gboolean on_label_close_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_label_close_button_press (GdkEventButton *event);

public:  
  ustring title;
private:
  WindowID window_id;

  // Focus tools.
public:
  GtkWidget * last_focused_widget;
  void focus_set(bool active = true);
  GtkWidget * focus_in_signal_button;
  void focus_if_widget_mine (GtkWidget *widget);
  bool focused;
private:
  static void on_container_tree_callback (GtkWidget *widget, gpointer user_data);
  void container_tree_callback (GtkWidget *widget, gpointer user_data);
  GtkWidget * focused_widget_to_look_for;
public:
private:
  static gboolean on_widget_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  void on_widget_button_press (GtkWidget *widget, GdkEventButton *event);
  void title_set (bool focused);
public:
  void connect_focus_signals (GtkWidget * widget);
private:
  static void on_widget_grab_focus(GtkWidget * widget, gpointer user_data);
  void widget_grab_focus(GtkWidget * widget);

  // Size and position.
  void display (bool startup);
  GdkRectangle my_gdk_rectangle;
public:
  GdkRectangle rectangle_get ();
  void rectangle_set (const GdkRectangle& rectangle);
private:
  guint resize_event_id;
  static bool on_resize_timeout (gpointer data);
  void resize_timeout ();

  // Window deletion.
  bool my_shutdown;
public:
  GtkWidget * delete_signal_button;
  void shutdown();
private:
  void undisplay();

  // Status bar.
public:
  void status1 (const ustring& text);
  void status2 (const ustring& text);
private:

};


#endif

