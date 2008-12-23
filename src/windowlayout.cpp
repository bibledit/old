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

#include "libraries.h"
#include <glib.h>
#include "windowlayout.h"
#include "help.h"
#include "settings.h"
#include "gwrappers.h"

WindowLayout::WindowLayout(int dummy)
// Screen Layout Windows.
{
  block_focus_signal = true;
  event_id = 0;

  extern Settings *settings;

  signal_button = gtk_button_new();

  // Create menu window and set its position and size.
  menuwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(menuwindow), "Menu area");
  gtk_window_set_default_size(GTK_WINDOW(menuwindow), 100, 500);

  menulabel = gtk_label_new("Move and resize\nthe menu area\ntill it suits you");
  gtk_widget_show(menulabel);
  gtk_container_add(GTK_CONTAINER(menuwindow), menulabel);
  gtk_label_set_justify(GTK_LABEL(menulabel), GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap(GTK_LABEL(menulabel), TRUE);

  g_signal_connect((gpointer) menuwindow, "focus_in_event", G_CALLBACK(on_window_focus_in_event), gpointer(this));
  g_signal_connect((gpointer) menuwindow, "delete_event", G_CALLBACK(on_window_delete_event), gpointer(this));

  gtk_window_resize(GTK_WINDOW(menuwindow), settings->genconfig.menu_area_width_get(), settings->genconfig.menu_area_height_get());
  gtk_window_move(GTK_WINDOW(menuwindow), settings->genconfig.menu_area_x_position_get(), settings->genconfig.menu_area_y_position_get());
  gtk_widget_show_all(menuwindow);

  // Create text window and set its position and size.
  textwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(textwindow), "Text area");
  gtk_window_set_default_size(GTK_WINDOW(textwindow), 100, 500);

  textlabel = gtk_label_new("Move and resize\nthe text area\ntill it suits you");
  gtk_widget_show(textlabel);
  gtk_container_add(GTK_CONTAINER(textwindow), textlabel);
  gtk_label_set_justify(GTK_LABEL(textlabel), GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap(GTK_LABEL(textlabel), TRUE);

  g_signal_connect((gpointer) textwindow, "focus_in_event", G_CALLBACK(on_window_focus_in_event), gpointer(this));
  g_signal_connect((gpointer) textwindow, "delete_event", G_CALLBACK(on_window_delete_event), gpointer(this));

  gtk_window_resize(GTK_WINDOW(textwindow), settings->genconfig.text_area_width_get(), settings->genconfig.text_area_height_get());
  gtk_window_move(GTK_WINDOW(textwindow), settings->genconfig.text_area_x_position_get(), settings->genconfig.text_area_y_position_get());
  gtk_widget_show_all(textwindow);

  // Create notes window and set its position and size.
  noteswindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(noteswindow), "Notes area");
  gtk_window_set_default_size(GTK_WINDOW(noteswindow), 100, 500);

  noteslabel = gtk_label_new("Move and resize\nthe notes area\ntill it suits you");
  gtk_widget_show(noteslabel);
  gtk_container_add(GTK_CONTAINER(noteswindow), noteslabel);
  gtk_label_set_justify(GTK_LABEL(noteslabel), GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap(GTK_LABEL(noteslabel), TRUE);

  g_signal_connect((gpointer) noteswindow, "focus_in_event", G_CALLBACK(on_window_focus_in_event), gpointer(this));
  g_signal_connect((gpointer) noteswindow, "delete_event", G_CALLBACK(on_window_delete_event), gpointer(this));

  // It has been found by experience that the window should be moved first, and only then be made visible.
  gtk_window_resize(GTK_WINDOW(noteswindow), settings->genconfig.notes_area_width_get(), settings->genconfig.notes_area_height_get());
  gtk_window_move(GTK_WINDOW(noteswindow), settings->genconfig.notes_area_x_position_get(), settings->genconfig.notes_area_y_position_get());
  gtk_widget_show_all(noteswindow);

  // Create tools window and set its position and size.
  toolswindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(toolswindow), "Tools area");
  gtk_window_set_default_size(GTK_WINDOW(toolswindow), 100, 500);

  toolslabel = gtk_label_new("Move and resize\nthe Tools area\ntill it suits you");
  gtk_widget_show(toolslabel);
  gtk_container_add(GTK_CONTAINER(toolswindow), toolslabel);
  gtk_label_set_justify(GTK_LABEL(toolslabel), GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap(GTK_LABEL(toolslabel), TRUE);

  g_signal_connect((gpointer) toolswindow, "focus_in_event", G_CALLBACK(on_window_focus_in_event), gpointer(this));
  g_signal_connect((gpointer) toolswindow, "delete_event", G_CALLBACK(on_window_delete_event), gpointer(this));

  gtk_window_resize(GTK_WINDOW(toolswindow), settings->genconfig.tools_area_width_get(), settings->genconfig.tools_area_height_get());
  gtk_window_move(GTK_WINDOW(toolswindow), settings->genconfig.tools_area_x_position_get(), settings->genconfig.tools_area_y_position_get());
  gtk_widget_show_all(toolswindow);

  block_focus_signal = false;
}

WindowLayout::~WindowLayout()
{
  gw_destroy_source(event_id);
  extern Settings *settings;
  gint width, height, x, y;

  gtk_window_get_size(GTK_WINDOW(menuwindow), &width, &height);
  gtk_window_get_position(GTK_WINDOW(menuwindow), &x, &y);
  settings->genconfig.menu_area_width_set(width);
  settings->genconfig.menu_area_height_set(height);
  settings->genconfig.menu_area_x_position_set(x);
  settings->genconfig.menu_area_y_position_set(y);
  gtk_widget_destroy(menuwindow);

  gtk_window_get_size(GTK_WINDOW(textwindow), &width, &height);
  gtk_window_get_position(GTK_WINDOW(textwindow), &x, &y);
  settings->genconfig.text_area_width_set(width);
  settings->genconfig.text_area_height_set(height);
  settings->genconfig.text_area_x_position_set(x);
  settings->genconfig.text_area_y_position_set(y);
  gtk_widget_destroy(textwindow);

  gtk_window_get_size(GTK_WINDOW(noteswindow), &width, &height);
  gtk_window_get_position(GTK_WINDOW(noteswindow), &x, &y);
  settings->genconfig.notes_area_width_set(width);
  settings->genconfig.notes_area_height_set(height);
  settings->genconfig.notes_area_x_position_set(x);
  settings->genconfig.notes_area_y_position_set(y);
  gtk_widget_destroy(noteswindow);

  gtk_window_get_size(GTK_WINDOW(toolswindow), &width, &height);
  gtk_window_get_position(GTK_WINDOW(toolswindow), &x, &y);
  settings->genconfig.tools_area_width_set(width);
  settings->genconfig.tools_area_height_set(height);
  settings->genconfig.tools_area_x_position_set(x);
  settings->genconfig.tools_area_y_position_set(y);
  gtk_widget_destroy(toolswindow);

  gtk_widget_destroy(signal_button);
}

gboolean WindowLayout::on_window_focus_in_event(GtkWidget * widget, GdkEventFocus * event, gpointer user_data)
{
  ((WindowLayout *) user_data)->on_window_focus_in(widget);
  return FALSE;
}

void WindowLayout::on_window_focus_in(GtkWidget * widget)
{
  if (block_focus_signal)
    return;
  focus_widget = widget;
  gw_destroy_source(event_id);
  event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 500, GSourceFunc(on_timeout), gpointer(this), NULL);
}

bool WindowLayout::on_timeout(gpointer data)
{
  ((WindowLayout *) data)->timeout();
  return false;
}

void WindowLayout::timeout()
{
  block_focus_signal = true;
  gtk_window_present(GTK_WINDOW(menuwindow));
  gtk_window_present(GTK_WINDOW(textwindow));
  gtk_window_present(GTK_WINDOW(noteswindow));
  gtk_window_present(GTK_WINDOW(toolswindow));
  gtk_window_present(GTK_WINDOW(focus_widget));
  block_focus_signal = false;
}

bool WindowLayout::on_window_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  return ((WindowLayout *) user_data)->on_window_delete();
}

bool WindowLayout::on_window_delete()
{
  gtk_button_clicked(GTK_BUTTON(signal_button));
  return false;
}
