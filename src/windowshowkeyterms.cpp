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
#include "windowshowkeyterms.h"
#include "help.h"
#include "windows.h"

WindowShowKeyterms::WindowShowKeyterms(bool startup)
// Screen Layout Windows.
  {
    showkeytermswindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (showkeytermswindow), "Keyterms");

    scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolledwindow1);
    gtk_container_add(GTK_CONTAINER (showkeytermswindow), scrolledwindow1);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

    textview1 = gtk_text_view_new();
    gtk_widget_show(textview1);
    gtk_container_add(GTK_CONTAINER (scrolledwindow1), textview1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW (textview1), FALSE);
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW (textview1), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW (textview1), FALSE);

    signal_button = gtk_button_new();

    g_signal_connect ((gpointer) showkeytermswindow, "delete_event",
        G_CALLBACK (on_window_delete_event),
        gpointer (this));

    gtk_widget_show_all(showkeytermswindow);

    window_set_state (showkeytermswindow, widShowKeyterms, startup);
  }

WindowShowKeyterms::~WindowShowKeyterms()
  {
    window_store_state (showkeytermswindow, widShowKeyterms);
    gtk_widget_destroy(showkeytermswindow);
    gtk_widget_destroy(signal_button);
  }

bool WindowShowKeyterms::on_window_delete_event(GtkWidget *widget,
    GdkEvent *event, gpointer user_data)
  {
    return ((WindowShowKeyterms *) user_data)->on_window_delete();
  }

bool WindowShowKeyterms::on_window_delete()
  {
    gtk_button_clicked(GTK_BUTTON (signal_button));
    return false;
  }

/*
 
 Todo 
 
 When translating, and going to a certain verse, the user wishes to see
 all of the keyterms found in that verse, and what word they have chosen for
 it in their own language.
 This is going to be one tool in one window. No controls in that window.
 It's a GtkTextView that shows all in simple order. No fancy things.

 When this tool is chosen, it pops up as a window and tiles itself in the Tools area.
 We need a generic routine for that.
 The routine looks for empty space within constraints. It can make other windows smaller.
 When a window closes, the other windows are left as they are. 
 Thus if the same window is opened again, it will go in the same free space as it was before.

 */
