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


#include "window.h"
#include "settings.h"
#include "gwrappers.h"


WindowBase::WindowBase(WindowID id, ustring data_title, bool startup, unsigned long xembed, GtkWidget * parent_box)
// Base class for each window.
{
  // If there's no title/data, then the configuration file gets inconsistent. Therefore put something there.
  if (data_title.empty()) {
    data_title.append("Untitled");
  }
  // Initialize variables.
  my_shutdown = false;
  window_id = id;
  window_data = data_title;
  window_parent_box = parent_box;
  resize_window_pointer = NULL;
  last_focused_widget = NULL;

  // Signalling buttons.
  focus_in_signal_button = gtk_button_new();
  delete_signal_button = gtk_button_new();

  // Create the window or the vertical box.
  if (window_parent_box) {
    // In attached mode, a vertical box should be created.
    window_vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (window_parent_box), window_vbox, TRUE, TRUE, 0);
  } else {
    // In detached mode, a window is created.
    if (xembed) {
      window_vbox = gtk_plug_new(GdkNativeWindow(xembed));
    } else {
      window_vbox = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    }
  }
  
  // Set extra widgets for attached mode to NULL.  
  hbox_title = NULL;
  button_title = NULL;
  progressbar_title = NULL;
  button_close = NULL;
  if (window_parent_box) {
    
    hbox_title = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox_title);
    gtk_box_pack_start (GTK_BOX (window_vbox), hbox_title, FALSE, TRUE, 0);

    // Attached mode: Create a title bar and set the title.
    button_title = gtk_button_new ();
    gtk_widget_show (button_title);
    gtk_box_pack_start (GTK_BOX (hbox_title), button_title, TRUE, TRUE, 0);
    GTK_WIDGET_UNSET_FLAGS (button_title, GTK_CAN_FOCUS);
    gtk_button_set_relief (GTK_BUTTON (button_title), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click (GTK_BUTTON (button_title), FALSE);

    progressbar_title = gtk_progress_bar_new ();
    gtk_widget_show (progressbar_title);
    gtk_container_add (GTK_CONTAINER (button_title), progressbar_title);
    gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar_title), data_title.c_str());

    // Attached mode: Create close button too.
    button_close = gtk_button_new ();
    gtk_widget_show (button_close);
    gtk_box_pack_start (GTK_BOX (hbox_title), button_close, FALSE, FALSE, 0);
    gtk_button_set_focus_on_click (GTK_BUTTON (button_close), FALSE);

    image_close = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show (image_close);
    gtk_container_add (GTK_CONTAINER (button_close), image_close);

  } else {
    
    // Detached mode: Set the title in the window's titlebar.
    gtk_window_set_title(GTK_WINDOW(window_vbox), data_title.c_str());
  
  }

  // If using detached windows, use the same accelerators in each window to make them look and feel the same.
  if (window_parent_box == NULL) {
    extern GtkAccelGroup *accelerator_group;
    gtk_window_add_accel_group(GTK_WINDOW(window_vbox), accelerator_group);
  }

  // Signal handlers.
  g_signal_connect ((gpointer) window_vbox, "focus_in_event", G_CALLBACK(on_window_focus_in_event), gpointer(this));
  g_signal_connect ((gpointer) window_vbox, "delete_event", G_CALLBACK(on_window_delete_event), gpointer(this));

  if (window_parent_box) {
    g_signal_connect ((gpointer) button_title, "clicked", G_CALLBACK (on_button_title_clicked), gpointer(this));
    g_signal_connect ((gpointer) button_close, "clicked",  G_CALLBACK (on_button_close_clicked), gpointer (this));
  }

  // Do the display handling.
  display(startup);

  // Show it.
  gtk_widget_show_all(window_vbox);
}


WindowBase::~WindowBase()
{
  gw_destroy_source(display_event_id);
  undisplay();
  gtk_widget_destroy(window_vbox);
  gtk_widget_destroy(focus_in_signal_button);
  gtk_widget_destroy(delete_signal_button);
}


void WindowBase::display(bool startup)
// Does the bookkeeping necessary for displaying the detached window.
// startup: whether the window is started at program startup.
{
}


bool WindowBase::on_display_timeout(gpointer data)
{
  return ((WindowBase *) data)->display_timeout();
}


bool WindowBase::display_timeout()
// Repositions the detached window for the second time, as the first time does not always work out.
{
  if (window_parent_box) {
    // Attached mode: focus it.
    gtk_button_clicked(GTK_BUTTON(focus_in_signal_button));
  } else {
    // Detached mode: reposition / resize.
    if (window_gdk_rectangle.width && window_gdk_rectangle.height) {
      gtk_window_resize(GTK_WINDOW(window_vbox), window_gdk_rectangle.width, window_gdk_rectangle.height);
      gtk_window_move(GTK_WINDOW(window_vbox), window_gdk_rectangle.x, window_gdk_rectangle.y);
      if (resize_window_pointer) {
        gtk_window_resize(resize_window_pointer, resize_window_rectangle.width, resize_window_rectangle.height);
      }
    }
  } 
  // Done.
  return false;
}


gboolean WindowBase::on_window_focus_in_event(GtkWidget * widget, GdkEventFocus * event, gpointer user_data)
{
  ((WindowBase *) user_data)->on_window_focus_in(widget);
  return FALSE;
}


void WindowBase::on_window_focus_in(GtkWidget * widget)
{
  gtk_button_clicked(GTK_BUTTON(focus_in_signal_button));
}


void WindowBase::present(bool force)
// Presents the detached window.
{
  if (window_parent_box) {
    // Attached mode.
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar_title), 1);
    // Grab the widget of the object that was focused last.
    if (last_focused_widget) {
      gtk_widget_grab_focus (last_focused_widget);
    }
    gtk_button_clicked(GTK_BUTTON(focus_in_signal_button));
  } else {
    // Detached mode.
    // Only act if the window is not fully visible already, or if forced.
    if ((visibility_state() != GDK_VISIBILITY_UNOBSCURED) || force) {
      gtk_window_present(GTK_WINDOW(window_vbox));
    }
  }
}


bool WindowBase::on_window_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  ((WindowBase *) user_data)->on_window_delete();
  // Prevent the window from being deleted right now.
  // This prevents crashes.
  // The window will be deleted later on.
  return true;
}


void WindowBase::on_window_delete()
{
  gtk_button_clicked(GTK_BUTTON(delete_signal_button));
}


void WindowBase::shutdown()
// Program shutdown.
{
  my_shutdown = true;
}


void WindowBase::undisplay()
// Does the bookkeeping needed for deleting a window.
// When a detached window closes, the sizes of other windows are not affected. 
// Thus if the same window is opened again, it will go in the same free space as it was in before.
{
  // If the menu is getting deleted, skip everything else, as this is the main window.
  if (window_id == widMenu)
    return;

  // Window position if detached.
  gint width = 0;
  gint height = 0;
  gint x = 0;
  gint y = 0;
  if (window_parent_box == NULL) {
    gtk_window_get_size(GTK_WINDOW(window_vbox), &width, &height);
    gtk_window_get_position(GTK_WINDOW(window_vbox), &x, &y);
  }

  // Get the parameters of all the windows.
  WindowData window_params(true);

  // Ensure that the window has its entry in the settings.
  bool window_found = false;
  for (unsigned int i = 0; i < window_params.widths.size(); i++) {
    if ((window_params.ids[i] == window_id) && (window_params.titles[i] == window_data)) {
      window_found = true;
    }
  }
  if (!window_found) {
    window_params.x_positions.push_back(0);
    window_params.y_positions.push_back(0);
    window_params.widths.push_back(0);
    window_params.heights.push_back(0);
    window_params.ids.push_back(window_id);
    window_params.titles.push_back(window_data);
    window_params.shows.push_back(false);
  }
  // Set data for the window.
  for (unsigned int i = 0; i < window_params.ids.size(); i++) {
    if ((window_id == window_params.ids[i]) && (window_data == window_params.titles[i])) {
      // Set the position and size of the window.
      window_params.x_positions[i] = x;
      window_params.y_positions[i] = y;
      window_params.widths[i] = width;
      window_params.heights[i] = height;
      // The "showing" flag is set on program shutdown, else it is cleared.
      window_params.shows[i] = my_shutdown;
    }
  }

}


gboolean WindowBase::on_visibility_notify_event(GtkWidget * widget, GdkEventVisibility * event, gpointer user_data)
{
  ((WindowBase *) user_data)->visibility_notify_event(event);
  return false;
}


void WindowBase::visibility_notify_event(GdkEventVisibility * event)
/*
The signal should be connected to a widget that can be visible, such as a textview, or a listview, and so on.
Like so:
g_signal_connect ((gpointer) widget, "visibility-notify-event", G_CALLBACK (on_visibility_notify_event), gpointer(this));
*/
{
  bool state_registered = false;
  for (unsigned int i = 0; i < visibility_windows.size(); i++) {
    if (event->window == visibility_windows[i]) {
      visibility_states[i] = event->state;
      state_registered = true;
    }
  }
  if (!state_registered) {
    visibility_windows.push_back(event->window);
    visibility_states.push_back(event->state);
  }
}


GdkVisibilityState WindowBase::visibility_state()
/*
Gets the visibility state of the window.
GDK_VISIBILITY_UNOBSCURED the window is completely visible.
GDK_VISIBILITY_PARTIAL the window is partially visible.
GDK_VISIBILITY_FULLY_OBSCURED the window is not visible at all.
*/
{
  GdkVisibilityState visibility = GDK_VISIBILITY_UNOBSCURED;
  for (unsigned int i = 0; i < visibility_states.size(); i++) {
    if (visibility_states[i] > visibility) {
      visibility = visibility_states[i];
    }
  }
  return visibility;
}


void WindowBase::on_button_close_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowBase *) user_data)->on_button_close();
}


void WindowBase::on_button_close()
{
  gtk_button_clicked(GTK_BUTTON(delete_signal_button));
}


void WindowBase::on_button_title_clicked (GtkButton *button, gpointer user_data)
{
  ((WindowBase *) user_data)->on_button_title();
}


void WindowBase::on_button_title()
{
  gtk_button_clicked(GTK_BUTTON(focus_in_signal_button));
}


void WindowBase::defocus()
// Makes the window look like defocused.
{
  if (progressbar_title) {
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar_title), 0);
  }
}


void WindowBase::on_container_tree_callback (GtkWidget *widget, gpointer user_data)
{
  ((WindowBase *) user_data)->container_tree_callback(widget, user_data);
}


void WindowBase::container_tree_callback (GtkWidget *widget, gpointer user_data)
// Recursive callback that fires the focus signal if the widget belongs to the object.
{
  if (widget == focused_widget_to_look_for) {
    last_focused_widget = widget;
    gtk_button_clicked(GTK_BUTTON(focus_in_signal_button));
  }
  if (GTK_IS_CONTAINER(widget)) {
    gtk_container_foreach(GTK_CONTAINER(widget), on_container_tree_callback, user_data);
  }
}


void WindowBase::focus_if_widget_mine (GtkWidget *widget)
// It looks through all widgets it has, to find out whether "widget" belongs to the object.
{
  focused_widget_to_look_for = widget;
  if (GTK_IS_CONTAINER(window_vbox)) {
    gtk_container_foreach(GTK_CONTAINER(window_vbox), on_container_tree_callback, gpointer(this));
  }
}
