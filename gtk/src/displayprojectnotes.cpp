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


#include "displayprojectnotes.h"
#include <glib.h>
#include "notes_utils.h"
#include "mainwindow.h"


DisplayProjectNotes::DisplayProjectNotes(const ustring & reference, GtkWidget * webview_in, vector < unsigned int >*ids, unsigned int& edited_note_id)
{
  // Initialize and save variables.
  editor_reference = reference;
  webview = webview_in;
  mystop = false;
  ready = false;
  cursor_offset = 0;
  extra_note_id = edited_note_id;
  edited_note_id = 0;
  // Handle any notes given for display.
  if (ids) {
    ids_passed = true;
    ids_to_display.assign (ids->begin (), ids->end ());
  } else {
    ids_passed = false;
  }
  // Start main thread.  
  g_thread_new ("displayprojectnotes", GThreadFunc (thread_start), gpointer(this));
}


DisplayProjectNotes::~DisplayProjectNotes()
{
}


void DisplayProjectNotes::stop()
{
  mystop = true;
}


void DisplayProjectNotes::show_buffer ()
{
  // Displays the notes text buffer.
  webkit_web_view_load_string (WEBKIT_WEB_VIEW (webview), note_buffer.c_str(), NULL, NULL, NULL);
}


void DisplayProjectNotes::thread_start(gpointer data)
{
  ((DisplayProjectNotes *) data)->thread_main(data);
}


void DisplayProjectNotes::thread_main(gpointer data)
{
  // Select notes for display, and the one to scroll to, if none were given.
  if (!ids_passed) {
    notes_select(ids_to_display, id_to_scroll_to, editor_reference);
  }
  // "Display" the notes, that is, load them in the buffer.  
  if (!mystop) {
    notes_display(note_buffer, ids_to_display, id_to_scroll_to, cursor_offset, mystop, extra_note_id);
  }
  // Finish off.
  ready = true;
}

