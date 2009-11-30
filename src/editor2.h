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


#ifndef INCLUDED_EDITOR2_H
#define INCLUDED_EDITOR2_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "style.h"
#include "types.h"
#include "editor_aids.h"
#include "highlight.h"
#include "spelling.h"
#include "editoractions.h"


class Editor2
{
public:
  Editor2(GtkWidget * vbox_in, const ustring& project_in);
  ~Editor2();
  GtkWidget * new_widget_signal;
  GtkWidget * new_widget_pointer;
  void cut ();
  void copy ();
  void paste ();
private:
  GtkWidget *scrolledwindow_v2;
  GtkWidget *viewport_v2;
  GtkWidget *vbox_v2;
  GtkWidget *vbox_parking_lot;
  void text_load (ustring text, ustring character_style);
  deque <EditorAction *> actions_done;
  deque <EditorAction *> actions_undone;
  void apply_editor_action (EditorAction * action, EditorActionApplication application = eaaInitial);
  EditorActionCreateParagraph * focused_paragraph;
  bool usfm_starts_new_paragraph (ustring& line, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  void editor_start_new_paragraph (const ustring& marker_text);
  void editor_start_verse (ustring& line, ustring& marker_text, ustring& character_style);
  bool editor_starts_character_style (ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool editor_ends_character_style   (ustring & line, ustring & character_style, const ustring & marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  void editor_text_fallback (ustring& line, ustring& character_style, size_t marker_pos, bool marker_found);
  EditorActionCreateParagraph * textview2paragraph_action (GtkWidget * textview);
  int disregard_text_buffer_signals;
  vector <ustring> text_to_be_deleted;
  vector <ustring> styles_to_be_deleted;

  // Textview keyboard key pressing.
  static gboolean on_textview_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void textview_key_press_event(GtkWidget *widget, GdkEventKey *event);
  static gboolean on_textview_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  void textview_key_release_event(GtkWidget *widget, GdkEventKey *event);
  bool textbuffer_delete_range_was_fired;
  static void on_textbuffer_changed(GtkTextBuffer * textbuffer, gpointer user_data);
  void textbuffer_changed(GtkTextBuffer * textbuffer);

  // Textview mouse button pressing signal and delay.
  static gboolean on_textview_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
  void textview_button_press_event(GtkWidget * widget, GdkEventButton *event);
  guint textview_button_press_event_id;
  static bool on_textview_button_press_delayed (gpointer user_data);
  void textview_button_press_delayed ();

  // Textview focus grabbing signal and delay.
  static void on_textview_grab_focus(GtkWidget * widget, gpointer user_data);
  void textview_grab_focus(GtkWidget * widget);
  guint textview_grab_focus_event_id;
  static bool on_textview_grab_focus_delayed(gpointer data);
  void textview_grab_focus_delayed();

  // Textview cursor movement signal and delay.
  static void on_textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count, gboolean extend_selection, gpointer user_data);
  void textview_move_cursor(GtkTextView * textview, GtkMovementStep step, gint count);
  guint textview_move_cursor_id;
  static bool on_textview_move_cursor_delayed(gpointer user_data);
  void textview_move_cursor_delayed();








// Old stuff.
  // Focus handling.
public:
  GtkWidget * last_focused_widget;
  void programmatically_grab_focus(GtkWidget * widget);
  bool focus_programmatically_being_grabbed;
  GtkWidget * last_focused_textview();
  GtkTextBuffer * last_focused_textbuffer();
  EditorTextViewType last_focused_type();
  unsigned int last_focused_column();
  GtkTextChildAnchor * child_anchor_clicked;

  ustring character_style_on_start_typing;

  void book_set(unsigned int book_in);
  void chapter_load(unsigned int chapter_in);
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



  // Some event ids.
  guint save_timeout_event_id;

  // The formatted view.
  GtkTextTagTable * texttagtable;
  void create_or_update_formatting_data();
  void create_or_update_text_style(Style * style, bool paragraph, bool plaintext, double font_multiplier);
  bool verse_restarts_paragraph;

  vector <EditorNote> editornotes;
  vector <EditorTable> editortables;

  bool load_text_starting_footnote_content(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_ending_footnote_content(GtkTextBuffer * textbuffer, ustring& line, ustring& paragraph_mark, ustring& character_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_table_starting_row(ustring& line, EditorTable& editortable, GtkTextBuffer *& textbuffer, bool& row_zero_initialized, gint& row, gint& column, ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
  bool load_text_table_starting_cell(ustring& line, EditorTable& editortable, GtkTextBuffer *& textbuffer, bool& row_zero_initialized, gint& row, gint& column, ustring& paragraph_mark, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);

  void erase_related_note_bits();
  void display_notes_remainder(bool focus_rendered_textview);
  void renumber_and_clean_notes_callers();
  void insert_note(const ustring& marker, const ustring& rawtext, bool render);

  void display_table(ustring line, GtkTextIter iter);
  void erase_tables();

  bool do_not_process_child_anchors_being_deleted;
  static void on_buffer_insert_text_before(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data);
  void buffer_insert_text_before(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length);
  static void on_buffer_insert_text_after(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length, gpointer user_data);
  void buffer_insert_text_after(GtkTextBuffer *textbuffer, GtkTextIter *pos_iter, gchar *text, gint length);
  static void on_buffer_delete_range_before(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data);
  void buffer_delete_range_before(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end);
  static void on_buffer_delete_range_after(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end, gpointer user_data);
  void buffer_delete_range_after(GtkTextBuffer *textbuffer, GtkTextIter *start, GtkTextIter *end);
  void collect_text_child_anchors_being_deleted(GtkTextIter *startiter, GtkTextIter *enditer);
  void process_text_child_anchors_deleted();
  set <GtkTextChildAnchor *> text_child_anchors_being_deleted;
  void text_get_all(vector <ustring>& texts, vector <VectorUstring>& styles);

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
  void undo();
  void redo();
  bool can_undo();
  bool can_redo();

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
  void spelling_trigger();
  vector <ustring> spelling_get_misspelled ();
  void spelling_approve (const vector <ustring>& words);
private:
  guint spelling_timeout_event_id;
  static bool on_spelling_timeout(gpointer data);
  void spelling_timeout();
  SpellingChecker * spellingchecker;
  static void on_button_spelling_recheck_clicked(GtkButton *button, gpointer user_data);

  // Verse positioning and tracking.
public:
  void go_to_verse(const ustring& number, bool focus);
  ustring current_verse_number;
  GtkWidget * new_verse_signal;
private:
  void switch_verse_tracking_off ();
  void switch_verse_tracking_on ();
  bool verse_tracking_on;
  void signal_if_verse_changed ();
  guint signal_if_verse_changed_event_id;
  static bool on_signal_if_verse_changed_timeout(gpointer data);
  void signal_if_verse_changed_timeout();
  ustring verse_number_get();
  
  // Scrolling control.
public:
private:
  void scroll_insertion_point_on_screen ();
  static bool on_scroll_insertion_point_on_screen_timeout(gpointer data);
  void scroll_insertion_point_on_screen_timeout();
  GtkTextTag * verse_highlight_tag;

  // Moving from one textview to the other.
public:
private:
  void paragraph_crossing_act(GtkMovementStep step, gint count);
  GtkWidget * paragraph_crossing_textview_at_key_press;
  GtkTextIter paragraph_crossing_insertion_point_iterator_at_key_press;
  
};


#endif

