/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_SCREEN_H
#define INCLUDED_SCREEN_H


#include "libraries.h"
#include <gtk/gtk.h>


void screen_scroll_to_iterator(GtkTextView *text_view, GtkTextIter *iter);
void textview_scroll_to_mark (GtkTextView * textview, GtkTextMark * mark, bool exact);


enum DialogPositionType {dptInsertNote};
void dialog_position_save(DialogPositionType type, GtkWidget * dialog);
void dialog_position_restore(DialogPositionType type, GtkWidget * dialog);
void dialog_position_reset_all();


void window_position_get_left_space(GtkWidget * widget, gint& width, gint& height, gint& x, gint& y);
void window_position_make_left_space(GtkWidget * widget, gint space);


class DialogPresenter
{
public:
  DialogPresenter (GtkWidget * widget);
  ~DialogPresenter();
private:
  GtkWidget * mywidget;
  guint event_id;
  static bool on_timeout(gpointer data);
  void timeout();
};


class DialogAutoScaler
{
public:
  DialogAutoScaler (GtkWidget * widget, int height);
  ~DialogAutoScaler();
private:
  GtkWidget * mywidget;
  int desired_height;
  guint event_id;
  static bool on_timeout(gpointer data);
  void timeout();
};


#endif
