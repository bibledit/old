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

#ifndef INCLUDED_WINDOWS_H
#define INCLUDED_WINDOWS_H

#include "libraries.h"
#include <gtk/gtk.h>

enum WindowID { widShowKeyterms, widShowQuickReferences, widMerge, widResource, 
                widOutline, widCheckKeyterms, widStyles, widNotes, widReferences, 
                widEditor, widMenu, widShowVerses, widCheckUSFM };

class WindowData
{
public:
  WindowData(bool save_on_destroy);
  ~WindowData();
  vector<int> widths;
  vector<int> heights;
  vector<int> x_positions;
  vector<int> y_positions;
  vector<int> ids;
  vector<ustring> datas;
  vector<bool> shows;
  void debug();
private:
  bool my_save_on_destroy;
};

class WindowBase
{
public:
  WindowBase(WindowID id, ustring data_title, bool startup, unsigned long xembed, GtkWidget * parent_box);
  virtual ~WindowBase();
  GtkWidget *window_vbox;
  
  // Data specific for the window.
  WindowID window_id;
  ustring window_data;
  GtkWidget * window_parent_box;
  
  // Creation and display.
  void display (bool startup);
  GdkRectangle window_gdk_rectangle;
  GtkWindow * resize_window_pointer;
  GdkRectangle resize_window_rectangle;
  guint display_event_id;
  static bool on_display_timeout(gpointer data);
  bool display_timeout();

  // Focus tools.  
  GtkWidget * focus_in_signal_button;
  static gboolean on_window_focus_in_event(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);
  void on_window_focus_in(GtkWidget *widget);
  void present(bool force);
  void defocus(); 
  
  // Window deletion.
  GtkWidget * delete_signal_button;
  static bool on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void on_window_delete();
  bool my_shutdown;
  void shutdown();
  void undisplay();
  
  // Visibility.
  static gboolean on_visibility_notify_event (GtkWidget *widget, GdkEventVisibility *event, gpointer user_data);
  void visibility_notify_event(GdkEventVisibility *event);
  vector <GdkWindow *> visibility_windows;
  vector <GdkVisibilityState> visibility_states;
  GdkVisibilityState visibility_state();

  // Logic specific for attached mode.
  GtkWidget *hbox_title;
  GtkWidget *button_title;
  GtkWidget *progressbar_title;
  GtkWidget *button_close;
  GtkWidget *image_close;
  static void on_button_title_clicked (GtkButton *button, gpointer user_data);
  void on_button_title();
  static void on_button_close_clicked (GtkButton *button, gpointer user_data);
  void on_button_close();
  static void on_container_tree_callback (GtkWidget *widget, gpointer user_data);
  void container_tree_callback (GtkWidget *widget, gpointer user_data);
  void focus_if_widget_mine (GtkWidget *widget);
  GtkWidget * focused_widget_to_look_for;
  GtkWidget * last_focused_widget;
};

#endif
