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

#include "displayprojectnotes.h"
#include <glib.h>
#include "notes_utils.h"
#include "mainwindow.h"

DisplayProjectNotes::DisplayProjectNotes(const ustring& reference, GtkWidget * htmlview_in, vector <unsigned int> * ids) {
  // Initialize and save variables.
  editor_reference = reference;
  htmlview = htmlview_in;
  mystop = false;
  ready = false;
  cursor_offset = 0;
  // Handle any notes given for display.
  if (ids) {
    ids_passed = true;
    ids_to_display.assign(ids->begin(), ids->end());
  } else {
    ids_passed = false;
  }
  // Start main thread.  
  g_thread_create (GThreadFunc (thread_start), gpointer (this), false, NULL);
}

DisplayProjectNotes::~DisplayProjectNotes() {
}

void DisplayProjectNotes::stop() {
  mystop = true;
}

void DisplayProjectNotes::show_buffer() {
  // Displays the textbuffer.
  GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview));
  gtk_html_write(GTK_HTML(htmlview), stream, note_buffer.c_str(), -1);
  gtk_html_end(GTK_HTML(htmlview), stream, GTK_HTML_STREAM_OK);
}

void DisplayProjectNotes::position_cursor() {
  // Position the cursor at the right offset.
  if (!cursor_offset)
    return;
  /* Todo
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_offset(textbuffer, &iter, cursor_offset);
  gtk_text_buffer_place_cursor(textbuffer, &iter);
  while (gtk_events_pending())
    gtk_main_iteration();
  gtk_text_view_scroll_to_iter(textview, &iter, 0, true, 0.3, 0.3);
  */
}

void DisplayProjectNotes::thread_start(gpointer data) {
  ((DisplayProjectNotes *) data)->thread_main(data);
}

void DisplayProjectNotes::thread_main(gpointer data) {
  // Select notes for display, and the one to scroll to, if none were given.
  if (!ids_passed) {
    notes_select(ids_to_display, id_to_scroll_to, editor_reference);
  }
  // "Display" the notes, that is, load them in the buffer.  
  if (!mystop) {
    notes_display(note_buffer, ids_to_display, id_to_scroll_to, cursor_offset, mystop);
  }
  // Finish off.
  ready = true;
}

/*
 Todo gradually move over to the html view.

 Remove old stuff.

Functionality to transfer:
- Ctrl-click does not open the note for editing, but retrieves its references.
- Enter edits the note.
- Right-click can delete the note.
- Delete deletes the note.

 
 */

