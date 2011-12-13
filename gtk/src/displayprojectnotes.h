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


#ifndef INCLUDED_DISPLAYPROJECTNOTES_H
#define INCLUDED_DISPLAYPROJECTNOTES_H


#include "libraries.h"
#include <gtk/gtk.h>


class DisplayProjectNotes
{
public:
  DisplayProjectNotes(const ustring& reference, GtkWidget * htmlview_in, vector <unsigned int> * ids, unsigned int& edited_note_id);
  ~DisplayProjectNotes();
  void stop();
  bool ready;
  void show_buffer();
  void position_cursor();
private:
  static void thread_start(gpointer data);
  void thread_main(gpointer data);
  bool mystop;
  ustring editor_reference;
  bool ids_passed;
  vector<unsigned int> ids_to_display;
  unsigned int id_to_scroll_to;
  GtkWidget * htmlview;
  unsigned int cursor_offset;
  ustring note_buffer;
  unsigned int extra_note_id;
};


#endif
