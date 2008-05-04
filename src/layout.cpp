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


#include "layout.h"
#include "gtkwrappers.h"
#include "settings.h"
#include "gui_features.h"
#include "screen.h"
#include "gwrappers.h"


void screen_layout_tools_area_set (bool current_pos, 
                                   GtkWidget * parentleft, GtkWidget * parentright, 
                                   GtkWidget * childleft, GtkWidget * childright)
// Sets Tools Area left or right.
{
  extern Settings * settings;
  // Current and desired position are the same: bail out.
  if (current_pos == settings->genconfig.tools_area_left_get ())
    return;
  // Swap the left and right side.
  if (settings->genconfig.tools_area_left_get ()) {
    // Swapped
    gtk_widget_reparent (childleft, parentright);
    gtk_widget_reparent (childright, parentleft);
  } else {
    // Normal
    gtk_widget_reparent (childleft, parentleft);
    gtk_widget_reparent (childright, parentright);
  }
}


void screen_layout_vertical_pane_mirror (bool current_pos, GtkWidget * pane)
// Mirrors the divisions of the main vertical pane, if need be.
{
  extern Settings * settings;
  // Current and desired position are the same: bail out.
  if (current_pos == settings->genconfig.tools_area_left_get ())
    return;
  int position = gtk_paned_get_position (GTK_PANED (pane));
  position = settings->genconfig.window_width_get () - position;
  gtk_paned_set_position (GTK_PANED (pane), position);
}


ScreenLayoutDimensions::ScreenLayoutDimensions (GtkWidget *window, GtkWidget * hpane, GtkWidget * editor, GtkWidget * references)
{
  mywindow = GTK_WINDOW (window);
  my_hpane = hpane;
  my_editor = editor;
  my_references = references;
  counter = 0;
}


void ScreenLayoutDimensions::verify ()
// Set the dimensions in the screen.
{
  // Open configuration and get values.
  extern Settings * settings;
  int width = settings->genconfig.window_width_get ();
  int height = settings->genconfig.window_height_get ();
  int x = settings->genconfig.window_x_position_get ();
  int y = settings->genconfig.window_y_position_get ();
  int hpane_position = settings->genconfig.hpane_position_get ();
  int vpane_editor_position = settings->genconfig.vpane_editor_position_get ();
  int vpane_quick_references_position = settings->genconfig.vpane_quick_references_position_get ();
  bool tools_area_left = settings->genconfig.tools_area_left_get ();
  // If the screen resolution got changed, or if the window is too big, 
  // recalculate the values.
  bool recalculate = false;
  GdkScreen * screen = gtk_window_get_screen (mywindow);
  int real_screen_width = gdk_screen_get_width (screen);
  int stored_screen_width = settings->genconfig.screen_width_get ();
  int real_screen_height = gdk_screen_get_height (screen);
  int stored_screen_height = settings->genconfig.screen_height_get ();
  if (real_screen_width != stored_screen_width) recalculate = true;
  if (real_screen_height != stored_screen_height) recalculate = true;
  if (x + width > real_screen_width) recalculate = true;
  if (y + height > real_screen_height) recalculate = true;
  if (recalculate) {
    gw_message ("Recalculating window size");    
    settings->genconfig.screen_width_set (real_screen_width);
    settings->genconfig.screen_height_set (real_screen_height);
    width = real_screen_width * 95 / 100;
    settings->genconfig.window_width_set (width);
    height = real_screen_height * 90 / 100;
    settings->genconfig.window_height_set (height);
    x = 0;
    settings->genconfig.window_x_position_set (x);
    y = 0;
    settings->genconfig.window_y_position_set (y);
    hpane_position = width * 80 / 100;
    settings->genconfig.hpane_position_set (hpane_position);
    vpane_editor_position = height * 60 / 100;
    settings->genconfig.vpane_editor_position_set (vpane_editor_position);
    vpane_quick_references_position = vpane_editor_position;
    settings->genconfig.vpane_quick_references_position_set (vpane_quick_references_position);
    tools_area_left = false;
    settings->genconfig.tools_area_left_set (tools_area_left);
    dialog_position_reset_all ();
  }
  // Prevents areas from becoming invisible altogether,
  // prompting users to ask for support.
  if (hpane_position < 300) hpane_position = 300;
  if (vpane_editor_position < 200) vpane_editor_position = 200;
  if (vpane_quick_references_position < 100) vpane_quick_references_position = 100;
}


void ScreenLayoutDimensions::clip ()
/*
Clips the dimensions of the screen.
A weird bugs has been seen on some computers.
Suddenly window of bibledit becomes very wide, up to 4 times the size of the screen.
This function clips the sizes.
*/
{
  counter++;
  if (counter < 10) return;
  counter = 0;
  gint windowwidth, windowheight;
  gtk_window_get_size (mywindow, &windowwidth, &windowheight);
  extern Settings * settings;
  int screenwidth = settings->genconfig.screen_width_get ();
  int screenheight = settings->genconfig.screen_height_get ();
  int maxwith = (int) (screenwidth * 1.1);
  int maxheight = (int) (screenheight * 1.1);
  bool oversize = false;
  if (windowwidth > maxwith) oversize = true;
  if (windowheight > maxheight) oversize = true;
  if (oversize) {
    gw_message ("Resetting oversized window");
    load ();  
  }
}


void ScreenLayoutDimensions::save ()
{
  extern Settings * settings;
  GuiFeatures guifeatures (0);
  if (!settings->genconfig.window_maximized_get ()) {
    gint width, height, x, y;
    gtk_window_get_size (mywindow, &width, &height);
    gtk_window_get_position (mywindow, &x, &y);
    settings->genconfig.window_width_set (width);
    settings->genconfig.window_height_set (height);
    settings->genconfig.window_x_position_set (x);
    settings->genconfig.window_y_position_set (y);
  }     
  if (guifeatures.project_notes ())
    settings->genconfig.hpane_position_set (gtk_paned_get_position (GTK_PANED (my_hpane)));
  settings->genconfig.vpane_editor_position_set (gtk_paned_get_position (GTK_PANED (my_editor)));
  settings->genconfig.vpane_quick_references_position_set (gtk_paned_get_position (GTK_PANED (my_references)));
}
  

void ScreenLayoutDimensions::load ()
{
  extern Settings * settings;
  gtk_window_resize (mywindow, settings->genconfig.window_width_get (), settings->genconfig.window_height_get ());
  gtk_window_move (mywindow, settings->genconfig.window_x_position_get (), settings->genconfig.window_y_position_get ());
  gtk_paned_set_position (GTK_PANED (my_hpane), settings->genconfig.hpane_position_get ());
  gtk_paned_set_position (GTK_PANED (my_editor), settings->genconfig.vpane_editor_position_get ());
  gtk_paned_set_position (GTK_PANED (my_references), settings->genconfig.vpane_quick_references_position_get ());
}
