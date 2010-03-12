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


#ifndef INCLUDED_EDITOR_AIDS_H
#define INCLUDED_EDITOR_AIDS_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "style.h"
#include "editoractions.h"


enum EditorNoteType {entFootnote, entEndnote, entCrossreference};
enum EditorTextViewType {etvtBody, etvtNote, etvtTable};
enum EditorMovementType {emtForward, emtBack, emtUp, emtDown};


void marker_get_type_and_subtype(const ustring& project, const ustring& marker, StyleType& type, int& subtype);

bool style_get_plaintext(StyleType type, int subtype);
bool style_get_paragraph(StyleType type, int subtype);
bool style_get_starts_new_line_in_editor(StyleType type, int subtype);
bool style_get_starts_new_line_in_usfm(StyleType type, int subtype);
bool style_get_displays_marker(StyleType type, int subtype);
bool style_get_starts_character_style(StyleType type, int subtype);
bool style_get_starts_verse_number(StyleType type, int subtype);
ustring style_get_verse_marker(const ustring& project);
bool style_get_starts_footnote(StyleType type, int subtype);
bool style_get_starts_endnote(StyleType type, int subtype);
bool style_get_starts_crossreference(StyleType type, int subtype);
bool style_get_starts_note_content(StyleType type, int subtype);
ustring style_get_default_note_style(const ustring& project, EditorNoteType type);
ustring style_get_paragraph_note_style(const ustring& project);
bool style_get_starts_table_row(StyleType type, int subtype);
ustring style_get_table_row_marker(const ustring& project);
bool style_get_starts_table_cell(StyleType type, int subtype);
ustring style_get_table_cell_marker(const ustring& project, int column);

EditorNoteType note_type_get(const ustring& project, const ustring& marker);
NoteNumberingType note_numbering_type_get(const ustring& project, const ustring& marker);
ustring note_numbering_user_sequence_get(const ustring& project, const ustring& marker);

gint table_get_n_rows(GtkTable * table);
gint table_get_n_columns(GtkTable * table);

void usfm_internal_add_text(ustring& text, const ustring& addition);
void usfm_internal_get_text_close_character_style(ustring& text, const ustring& project, const ustring& style);
ustring usfm_get_note_text(GtkTextIter startiter, GtkTextIter enditer, const ustring& project);

void get_styles_at_iterator(GtkTextIter iter, ustring& paragraph_style, ustring& character_style);
ustring get_verse_number_at_iterator(GtkTextIter iter, const ustring& verse_marker, const ustring& project, GtkWidget * parent_box);
bool get_iterator_at_verse_number (const ustring& verse_number, const ustring& verse_marker, GtkWidget * parent_box, GtkTextIter & iter, GtkWidget *& textview, bool deep_search = false);
vector <ustring> get_character_styles_between_iterators (GtkTextIter startiter, GtkTextIter enditer);

const gchar * unknown_style();
const gchar * note_starting_style ();
const gchar * usfm_clipboard_code ();
void get_next_note_caller_and_style (EditorNoteType type, ustring& caller, ustring& style, bool restart);
void textbuffer_apply_named_tag(GtkTextBuffer *buffer, const ustring& name, const GtkTextIter *start, const GtkTextIter *end);
void textbuffer_insert_with_named_tags(GtkTextBuffer *buffer, GtkTextIter *iter, const ustring& text, ustring first_tag_name, ustring second_tag_name);

void clear_and_destroy_editor_actions (deque <EditorAction *>& actions);
void on_container_tree_callback_destroy (GtkWidget *widget, gpointer user_data);
void editor_text_append(GtkTextBuffer * textbuffer, const ustring & text, const ustring & paragraph_style, const ustring & character_style);

gint editor_paragraph_insertion_point_get_offset (EditorActionCreateParagraph * paragraph_action);
void editor_paragraph_insertion_point_set_offset (EditorActionCreateParagraph * paragraph_action, gint offset);

bool text_starts_paragraph (const ustring& project, ustring& line, const ustring& marker, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);
bool text_starts_verse (const ustring& project, ustring& line, const ustring& marker_text, size_t marker_pos, size_t marker_length, bool is_opener, bool marker_found);

EditorActionDeleteText * paragraph_delete_last_character_if_space(EditorActionCreateParagraph * paragraph_action);
EditorActionDeleteText * paragraph_get_text_and_styles_after_insertion_point(EditorActionCreateParagraph * paragraph, vector <ustring>& text, vector <ustring>& styles);
void get_text_and_styles_between_iterators(GtkTextIter * startiter, GtkTextIter * enditer, vector <ustring>& text, vector <ustring>& styles);

vector <GtkWidget *> editor_get_widgets (GtkWidget * vbox);
GtkWidget * editor_get_next_textview (GtkWidget * vbox, GtkWidget * textview);
GtkWidget * editor_get_previous_textview (GtkWidget * vbox, GtkWidget * textview);
void editor_park_widget (GtkWidget * vbox, GtkWidget * widget, gint& offset, GtkWidget * parking);

bool move_end_iterator_before_note_caller_and_validate (GtkTextIter startiter, GtkTextIter enditer, GtkTextIter & moved_enditer);


#endif
