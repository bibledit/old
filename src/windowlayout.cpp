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

WindowLayout::WindowLayout(int dummy)
// Screen Layout Windows.
  {

    toolswindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (toolswindow), "Tools area");
    gtk_window_set_default_size(GTK_WINDOW (toolswindow), 100, 500);

    label8
        = gtk_label_new("Move and resize\nthe Tools area\ntill it suits you");
    gtk_widget_show(label8);
    gtk_container_add(GTK_CONTAINER (toolswindow), label8);
    gtk_label_set_justify(GTK_LABEL (label8), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL (label8), TRUE);

    signal_button = gtk_button_new();

    g_signal_connect ((gpointer) toolswindow, "delete_event",
        G_CALLBACK (on_window_delete_event),
        gpointer (this));

    gtk_widget_show_all(toolswindow);

    // Set window position and size.
    extern Settings * settings;
    gtk_window_resize(GTK_WINDOW (toolswindow), settings->genconfig.tools_area_width_get (), settings->genconfig.tools_area_height_get ());
    gtk_window_move(GTK_WINDOW (toolswindow), settings->genconfig.tools_area_x_position_get (), settings->genconfig.tools_area_y_position_get ());
  }

WindowLayout::~WindowLayout()
  {
    // Settings.
    extern Settings * settings;
    
    // Tools window.
    gint width, height, x, y;
    gtk_window_get_size(GTK_WINDOW(toolswindow), &width, &height);
    gtk_window_get_position(GTK_WINDOW(toolswindow), &x, &y);
    settings->genconfig.tools_area_width_set(width);
    settings->genconfig.tools_area_height_set(height);
    settings->genconfig.tools_area_x_position_set(x);
    settings->genconfig.tools_area_y_position_set(y);
    gtk_widget_destroy(toolswindow);

    // Signalling button.
    gtk_widget_destroy(signal_button);
  }

bool WindowLayout::on_window_delete_event(GtkWidget *widget, GdkEvent *event,
    gpointer user_data)
  {
    return ((WindowLayout *) user_data)->on_window_delete();
  }

bool WindowLayout::on_window_delete()
  {
    gtk_button_clicked(GTK_BUTTON (signal_button));
    return false;
  }

