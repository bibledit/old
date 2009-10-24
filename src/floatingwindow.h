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


class FloatingWindow
{
public:
  FloatingWindow(GtkWidget * layout_in);
  virtual ~FloatingWindow();

private:
  GtkWidget * layout;
  GtkBuilder * gtkbuilder;
  
  GtkWidget *vbox_window;
  GtkWidget *label_title;
  GtkWidget *hscrollbar_title;
  GtkWidget *eventbox_actions;
  GtkWidget *label_actions;
  GtkWidget *vbox_client;
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
  gint box_x_position;
  gint box_y_position;
  static gboolean on_status_bar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_status_bar_button_press (GdkEventButton *event);
  static gboolean on_status_bar_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  gboolean on_status_bar_button_release (GdkEventButton *event);
  static gboolean on_status_bar_motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
  gboolean on_status_bar_motion_notify (GdkEventMotion *event);
  bool resizing_window;
  gint box_width;
  gint box_height;


  // Cursor control.
  static gboolean on_titlebar_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_titlebar_enter_notify (GdkEventCrossing *event);
  static gboolean on_titlebar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_titlebar_leave_notify (GdkEventCrossing *event);
  static gboolean on_statusbar_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_statusbar_enter_notify (GdkEventCrossing *event);
  static gboolean on_statusbar_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_statusbar_leave_notify (GdkEventCrossing *event);

  static gboolean on_actions_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_actions_enter_notify (GdkEventCrossing *event);
  static gboolean on_actions_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_actions_leave_notify (GdkEventCrossing *event);



};


#endif
