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


#ifndef INCLUDED_EDITOR_H
#define INCLUDED_EDITOR_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "style.h"
#include "types.h"
#include "editor_aids.h"
#include "highlight.h"
#include "spelling.h"


class Editor
{
public:
  Editor(GtkWidget * vbox, const ustring& project_in);
  ~Editor();

  // Focus handling.
public:
  static void on_textview_grab_focus(GtkWidget * widget, gpointer user_data);
  void textview_grab_focus(GtkWidget * widget);
  GtkWidget * last_focused_widget;
  guint grab_focus_event_id;
  void programmatically_grab_focus(GtkWidget * widget);
  bool focus_programmatically_being_grabbed;
  static bool on_grab_focus_delayer_timeout(gpointer data);
  void on_grab_focus_delayed_handler();
  GtkWidget * last_focused_textview();
  GtkTextBuffer * last_focused_textbuffer();
  EditorTextViewType last_focused_type();
  unsigned int last_focused_column();
  GtkTextChildAnchor * child_anchor_clicked;

  // Cursor movement handling.
  static void on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data);
  void on_textview_cursor_moved_delayer(GtkTextView * textview, GtkMovementStep step, gint count);
  guint textview_cursor_moved_delayer_event_id;
  static bool on_textview_cursor_moved_delayer_handler(gpointer user_data);
  void on_textview_cursor_moved();

  ustring character_style_on_start_typing;

  void book_set(unsigned int book_in);
  void chapter_load(unsigned int chapter_in);
  void text_load (ustring text);
  void chapter_save();
  ustring text_get_selection();
  void text_erase_selection();
  void text_insert(ustring text);

  void show_quick_references();
  guint event_id_show_quick_references;
  static bool show_quick_references_timeout(gpointer user_data);
  void show_quick_references_execute();
  vector <Reference> quick_references;
  GtkWidget * quick_references_button;

  GtkWidget * new_verse_signal;
  GtkWidget * new_styles_signal;
  GtkWidget * reload_signal;
  unsigned int reload_chapter_number;

  Reference current_reference;
  bool go_to_new_reference_highlight;
  ustring word_double_clicked_text;
  GtkWidget * word_double_clicked_signal;

  void set_font();

  ustring project;
  unsigned int book;
  unsigned int chapter;

  set <ustring> get_styles_at_cursor();

  // Automatic save.
  static bool on_save_timeout(gpointer data);
  bool save_timeout();

  static gboolean on_text_motion_notify_event(GtkWidget *textview, GdkEventMotion *event, gpointer user_data);
  gboolean text_motion_notify_event(GtkWidget *textview, GdkEventMotion *event);
  static void on_text_event_after(GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void text_event_after(GtkWidget *textview, GdkEvent *ev);
  bool previous_hand_cursor;
  void text_edit_if_link(GtkWidget *textview, GtkTextIter *iter);
  static gboolean text_key_press_event_before(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  gboolean on_text_key_press_event_before(GtkWidget *widget, GdkEventKey *event);
  static gboolean text_key_press_event_after(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  gboolean on_text_key_press_event_after(GtkWidget *widget, GdkEventKey *event);
  static void on_textbuffer_footnotes_changed(GtkEditable * editable, gpointer user_data);
  void on_textbuffer_footnotes();
  static gboolean on_textview_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  void on_texteditor_click(GtkWidget * widget, GdkEventButton *event);

  // Some event ids.
  guint undo_redo_event_id;
  guint save_timeout_event_id;

  // The formatted view.
  GtkWidget *scrolledwindow;
  GtkWidget *textview;
  GtkTextTagTable * texttagtable;
  void create_or_update_formatting_data();
  GtkTextBuffer * textbuffer;
  void create_or_update_text_style(Style * style, bool paragraph, bool plaintext, double font_multiplier);
  bool verse_restarts_paragraph;

  vector <EditorNote> editornotes;
  vector <EditorTable> editortables;
  bool load_text_not_starting_with_marker(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, size_t marker_pos, size_t marker_length, bool marker_found);
  bool load_text_starting_new_paragraph(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_starting_character_style(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_ending_character_style(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_verse_number(ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_note_raw(ustring& line, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_starting_footnote_content(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_ending_footnote_content(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_table_raw(ustring& line, const ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_table_starting_row(ustring& line, EditorTable& editortable, GtkTextBuffer *& textbuffer, bool& row_zero_initialized, gint& row, gint& column, ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_table_starting_cell(ustring& line, EditorTable& editortable, GtkTextBuffer *& textbuffer, bool& row_zero_initialized, gint& row, gint& column, ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  void load_text_with_unknown_markup(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark);
  void load_text_ensure_normal_paragraph(ustring& line, ustring& paragraph_mark, ustring& character_mark);

  void erase_related_note_bits();
  void display_notes_remainder(bool focus_rendered_textview);
  void renumber_and_clean_notes_callers();
  void insert_note(const ustring& marker, const ustring& rawtext, bool render);

  void display_table(ustring line, GtkTextIter iter);
  void erase_tables();

  static void on_related_widget_size_allocated(GtkWidget *widget, GtkAllocation *allocation, gpointer user_data);
  void related_widget_size_allocated(GtkWidget *widget, GtkAllocation *allocation);
  gint textview_allocated_width;

  void set_embedded_note_textview_width(unsigned int notenumber);
  void set_embedded_note_caller_height(unsigned int notenumber);
  void set_embedded_table_textviews_width(unsigned int tablenumber);

  bool do_not_process_child_anchors_being_deleted;
  static void on_buffer_insert_text_before(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data);
  void buffer_insert_text_before(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length);
  static void on_buffer_insert_text_after(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data);
  void buffer_insert_text_after(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length);
  static void on_buffer_delete_range_before(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data);
  void buffer_delete_range_before(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end);
  static void on_buffer_delete_range_after(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data);
  void buffer_delete_range_after(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end);
  static void on_buffer_apply_tag(GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer, gpointer user_data);
  void buffer_apply_tag(GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer);
  static void on_buffer_remove_tag(GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer, gpointer user_data);
  void buffer_remove_tag(GtkTextBuffer *textbuffer, GtkTextTag *tag, GtkTextIter *startiter, GtkTextIter *enditer);
  static void on_buffer_insert_child_anchor(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GtkTextChildAnchor *childanchor, gpointer user_data);
  void buffer_insert_child_anchor(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GtkTextChildAnchor *childanchor);
  static void on_buffer_insert_pixbuf(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GdkPixbuf *pixbuf, gpointer user_data);
  void buffer_insert_pixbuf(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, GdkPixbuf *pixbuf);
  void collect_text_child_anchors_being_deleted(GtkTextIter *startiter, GtkTextIter *enditer);
  void process_text_child_anchors_deleted();
  set <GtkTextChildAnchor *> text_child_anchors_being_deleted;
  void text_get_all(vector <ustring>& texts, vector <VectorUstring>& styles);
  void text_append(GtkTextBuffer * textbuffer, const ustring& text, const ustring& paragraph_style, const ustring& character_style);

  bool editable;

  void signal_if_styles_changed();
  set <ustring> styles_at_cursor;
  set <ustring> styles_at_iterator(GtkTextIter iter);
  void apply_style(const ustring& marker);

  static bool on_apply_style_at_cursor(gpointer user_data);
  void apply_style_at_cursor_handler();
  ustring style_to_be_applied_at_cursor;

  void insert_table(const ustring& rawtext, GtkTextIter * iter);

  // Undo/redo
  int record_undo_level;
  bool recording_undo_actions();
  deque <EditorSnapshot> snapshots;
  unsigned int redo_counter;
  void undo();
  void redo();
  bool can_undo();
  bool can_redo();
  void restore_snapshot(int pointer);
  static void on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data);
  void textbuffer_changed(GtkTextBuffer * textbuffer);
  void trigger_undo_redo_recording();
  guint textbuffer_changed_event_id;
  static bool on_textbuffer_changed_timeout (gpointer user_data);
  void textbuffer_changed_timeout();

  // Highlighting.
  void highlight_searchwords();
  guint highlight_timeout_event_id;
  static bool on_highlight_timeout(gpointer data);
  bool highlight_timeout();
  static void highlight_thread_start(gpointer data);
  void highlight_thread_main();
  Highlight * highlight;
  GtkTextTag * reference_tag;

  GtkWidget * changed_signal;
  void signal_editor_changed();
  ustring get_chapter();

private:

  // Spelling check.
public:
  void load_dictionaries();
  bool move_cursor_to_spelling_error (bool next, bool extremity);
  GtkWidget * spelling_checked_signal;
private:
  void spelling_trigger();
  guint spelling_timeout_event_id;
  static bool on_spelling_timeout(gpointer data);
  void spelling_timeout();
  SpellingChecker * spellingchecker;
  static void on_button_spelling_recheck_clicked(GtkButton *button, gpointer user_data);

  // Moving from one textview to the other.
public:
private:
  GtkTextView * texview_to_textview_old;
  GtkTextView * texview_to_textview_new;
  gint textview_to_textview_offset;
  GtkMovementStep textview_to_textview_steptype;
  gint textview_to_textview_stepcount;
  void check_move_textview_to_textview();

  // Verse tracking and positioning.
  // The mechanism only starts shortly after loading a chapter.
  // Before it starts, all tracking and positioning is simulated in memory.
public:
  void position_cursor_at_verse(const ustring& cursorposition, bool focus);
  ustring current_verse_number;
private:
  void restart_verse_tracker();
  guint start_verse_tracker_event_id;
  static bool on_restart_verse_tracker_timeout(gpointer data);
  bool on_restart_verse_tracker();
  guint verse_tracker_event_id;
  static bool on_verse_tracker_timeout(gpointer user_data);
  bool verse_tracker_timeout();
  bool verse_tracker_on;
  ustring verse_number_get();
  GtkTextIter previous_insert_iter;
  
  // Scrolling control.
public:
private:
  void scroll_cursor_on_screen ();
  static bool on_scroll_cursor_on_screen_timeout(gpointer data);
  void scroll_cursor_on_screen_timeout();
  
};


#endif
