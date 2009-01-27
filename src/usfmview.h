/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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

#ifndef INCLUDED_USFM_VIEW_H
#define INCLUDED_USFM_VIEW_H

#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "style.h"
#include "types.h"
#include "editor_aids.h"
#include <gtksourceview/gtksourceview.h>

class USFMView
{
public:
  USFMView(GtkWidget * vbox, const ustring& project_in);
  ~USFMView();
  GtkWidget * sourceview;
  Reference current_reference;
  ustring project;
  void book_set(unsigned int book_in);
  unsigned int book;
  void chapter_load(unsigned int chapter_in);
  void chapter_save();
  unsigned int reload_chapter_number;
  bool can_undo ();
  void undo();
  bool can_redo();
  void redo();
  bool editable;
  GtkWidget * reload_signal;
  GtkWidget * changed_signal;
  void set_font();
  void position_cursor_at_verse(const ustring& verse);
  ustring current_verse_number;
  GtkWidget * new_verse_signal;
  ustring text_get_selection();
  void text_erase_selection();
  GtkTextBuffer * last_focused_textbuffer();
  void text_insert(ustring text);
  GtkWidget * word_double_clicked_signal;
  ustring word_double_clicked_text;
  void insert_note(const ustring& marker, const ustring& rawtext);
  ustring get_chapter();
  unsigned int chapter;
private:
  GtkWidget *scrolledwindow;
  GtkSourceBuffer * sourcebuffer;
  guint save_timeout_event_id;
  static bool on_save_timeout(gpointer data);
  bool save_timeout();
  static void on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data);
  void textbuffer_changed();
  static void on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data);
  void on_textview_cursor_moved();
  static void on_textview_grab_focus(GtkWidget * widget, gpointer user_data);
  void textview_grab_focus();
  void restart_verse_tracker();
  guint verse_tracker_event_id;
  static bool on_verse_tracker_timeout(gpointer data);
  bool on_verse_tracker();
  static gboolean on_textview_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  void on_texteditor_click(GtkWidget * widget, GdkEventButton *event);
  GtkTextTag * markup_tag;

};


#endif
