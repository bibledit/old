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


#ifndef INCLUDED_SCREEN_H
#define INCLUDED_SCREEN_H


#include "libraries.h"
#include <gtk/gtk.h>


void screen_scroll_to_iterator (GtkTextView *text_view, GtkTextIter *iter);
void screen_set_cursor_hand_or_regular (GtkTextView *text_view, gint x, gint y);
gboolean screen_visibility_notify_event (GtkWidget *text_view, GdkEventVisibility *event, gpointer user_data);


enum DialogPositionType {dptInsertNote};
void dialog_position_save (DialogPositionType type, GtkWidget * dialog);
void dialog_position_restore (DialogPositionType type, GtkWidget * dialog);
void dialog_position_reset_all ();


void dialog_position_get_left_space (GtkWidget * dialog, gint& width, gint& height, gint& x, gint& y);


#endif
