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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_WINDOW_LAYOUT_H
#define INCLUDED_WINDOW_LAYOUT_H

#include <gtk/gtk.h>
#include "ustring.h"

class WindowLayout
{
public:
  WindowLayout(int dummy);
  ~WindowLayout();
  GtkWidget * signal_button;
protected:
  GtkWidget *menuwindow;
  GtkWidget *menulabel;
  GtkWidget *textwindow;
  GtkWidget *textlabel;
  GtkWidget *noteswindow;
  GtkWidget *noteslabel;
  GtkWidget *toolswindow;
  GtkWidget *toolslabel;
private:
  static gboolean on_window_focus_in_event(GtkWidget *widget, GdkEventFocus *event, gpointer user_data);
  void on_window_focus_in(GtkWidget *widget);
  bool block_focus_signal;
  GtkWidget * focus_widget;

  guint event_id;
  static bool on_timeout(gpointer data);
  void timeout();

  static bool on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  bool on_window_delete();
};

#endif
