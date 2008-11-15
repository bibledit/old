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

#include "libraries.h"
#include <glib.h>
#include "windoweditor.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"

WindowEditor::WindowEditor(const ustring& project_name, GtkAccelGroup *accelerator_group, bool startup) :
  WindowBase(widEditor, project_name, accelerator_group, startup)
// Text editor.
{
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER (window), vbox);

  // Create the new editor.
  editor = new Editor (vbox, project_name);
  editor->focus();
}

WindowEditor::~WindowEditor() {
  delete editor;
}

void WindowEditor::go_to(const Reference& reference)
// Let the editor go to a reference.
{
  // Find out what needs to be changed: book, chapter and/or verse.
  bool new_book = (reference.book != editor->current_reference.book);
  editor->current_reference.book = reference.book;
  bool new_chapter = (reference.chapter != editor->current_reference.chapter);
  editor->current_reference.chapter = reference.chapter;
  bool new_verse = (reference.verse != editor->current_reference.verse);
  editor->current_reference.verse = reference.verse;

  // Save the editor if need be.
  if (new_book || new_chapter) {
    editor->chapter_save();
  }

  // With a new book, also load a new chapter.
  if (new_book) {
    new_chapter = true;
    editor->book_set(reference.book);
  }

  // Deal with a new chapter.
  if (new_chapter) {
    // Load chapter in Editor, if need be.
    editor->chapter_load(reference.chapter);
    // When loading a new chapter, there is also a new verse.
    new_verse = true;
  }

  // New reference handling.  
  if (new_book || new_chapter || new_verse) {
    // Position the cursor properly.
    // The positioning will be done whenever Gtk is idle.
    // This is because sometimes Gtk is slow in loading a new chapter.
    // So if the cursor positioning is done straight after loading,
    // it will not work, as there is no text loaded yet.
    // But here we deal with that so that a delay is no longer needed. The trick:
    // Handle all pending events in GTK.
    while (gtk_events_pending())
      gtk_main_iteration();
    editor->position_cursor_at_verse(reference.verse, false);
  }

  // Highlighting of searchwords.
  if (editor->go_to_new_reference_highlight) {
    editor->highlight_searchwords();
    editor->go_to_new_reference_highlight = false;
  }
}

