/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_KEYBOARD_H
#define INCLUDED_KEYBOARD_H


#include "libraries.h"
#include <gtk/gtk.h>


bool keyboard_enter_pressed (GdkEventKey *event);
bool keyboard_delete_pressed (GdkEventKey *event);
bool keyboard_insert_pressed (GdkEventKey *event);
bool keyboard_up_arrow_pressed (GdkEventKey *event);
bool keyboard_left_arrow_pressed (GdkEventKey *event);
bool keyboard_right_arrow_pressed (GdkEventKey *event);
bool keyboard_down_arrow_pressed (GdkEventKey *event);
bool keyboard_page_up_pressed (GdkEventKey *event);
bool keyboard_control_state (GdkEventButton *event);
bool keyboard_control_state (GdkEventKey *event);
bool keyboard_shift_state (GdkEventKey *event);
bool keyboard_backspace_pressed (GdkEventKey *event);


#endif
