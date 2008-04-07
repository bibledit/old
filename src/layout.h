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


#ifndef INCLUDED_LAYOUT_H
#define INCLUDED_LAYOUT_H


#include "libraries.h"
#include <gtk/gtk.h>


void screen_layout_tools_area_set (bool current_pos,
                                   GtkWidget * parentleft, GtkWidget * parentright, 
                                   GtkWidget * childleft, GtkWidget * childright);
void screen_layout_vertical_pane_mirror (bool current_pos, GtkWidget * pane);
     
     
class ScreenLayoutDimensions
// Manages screen layout dimensions.
{
public:
  ScreenLayoutDimensions (GtkWidget *window, GtkWidget * hpane, GtkWidget * editor, GtkWidget * references);
  void verify ();
  void load ();
  void clip ();
  void save ();
private:
  GtkWindow * mywindow;
  GtkWidget * my_hpane;
  GtkWidget * my_editor;
  GtkWidget * my_references;
};


#endif
