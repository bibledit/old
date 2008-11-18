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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_WINDOWS_H
#define INCLUDED_WINDOWS_H

#include "libraries.h"
#include <gtk/gtk.h>

enum WindowID
{ widShowKeyterms, widShowQuickReferences, widMerge, widResource, widOutline, widCheckKeyterms, widStyles, widNotes, widReferences, widEditor };

void window_delete(GtkWidget * window, WindowID id, const ustring& data, bool shutdown);

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
  WindowBase(WindowID id, ustring data_title, GtkAccelGroup *accelerator_group, bool startup);
  virtual ~WindowBase();
  GtkWidget *window;
  
  WindowID window_id;
  ustring window_data;
  
  void display (bool startup);
  GdkRectangle window_gdk_rectangle;
  GtkWindow * resize_window_pointer;
  GdkRectangle resize_window_rectangle;
  guint display_event_id;
  static bool on_display_timeout(gpointer data);
  bool display_timeout();
  
  GtkWidget * focus_in_signal_button;
  static gboolean on_window_focus_in_event(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);
  void on_window_focus_in(GtkWidget *widget);
  time_t focused_time;
  bool act_on_focus_in_signal;
  void present();
  bool focused();
  
  guint focus_event_id;
  static bool on_focus_timeout(gpointer data);
  void focus_timeout();
  
  GtkWidget * delete_signal_button;
  static bool on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void on_window_delete();
  bool my_shutdown;
  void shutdown();
};

#endif
