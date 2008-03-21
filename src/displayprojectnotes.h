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


#ifndef INCLUDED_DISPLAYPROJECTNOTES_H
#define INCLUDED_DISPLAYPROJECTNOTES_H


#include "libraries.h"
#include <gtk/gtk.h>


class DisplayProjectNotes
{
public:
  DisplayProjectNotes (const ustring& reference, 
                       GtkTextBuffer * textbuffer_in, GtkWidget * textview_in, 
                       vector <unsigned int> * ids);
  ~DisplayProjectNotes ();
  void stop ();
  bool ready;
  GtkTextBuffer * switch_buffer ();
  void position_cursor ();
  void update_progressbar ();
private:
  static void thread_start (gpointer data);
  void thread_main (gpointer data);
  bool mystop;
  ustring editor_reference;
  bool ids_passed;
  vector<unsigned int> ids_to_display;
  unsigned int id_to_scroll_to;
  GtkTextBuffer * textbuffer;
  GtkTextView * textview;
  GtkWidget * progressbar;
  double fraction;
  unsigned int cursor_offset;
};


#endif
