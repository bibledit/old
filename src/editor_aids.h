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


#ifndef INCLUDED_EDITOR_AIDS_H
#define INCLUDED_EDITOR_AIDS_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "style.h"


enum EditorNoteType { entFootnote, entEndnote, entCrossreference };
enum EditorTextViewType { etvtBody, etvtNote, etvtTable };
enum EditorUndoType { eudInsertText, eudDeleteText, eudApplyTag, eudRemoveTag };


class EditorNote
{
public:
  EditorNote (int dummy);
  ~EditorNote ();
  GtkTextChildAnchor * childanchor_caller_text;
  GtkWidget * label_caller_text;
  GtkTextChildAnchor * childanchor_caller_note;
  GtkWidget * label_caller_note;
  gint label_caller_note_allocated_width;
  GtkTextChildAnchor * childanchor_textview;
  GtkWidget * textview;
  gint textview_allocated_height;
  GtkTextBuffer * textbuffer;
  ustring marker;
  ustring rawtext;
  ustring caller;
private:
};


class EditorTable
{
public:
  EditorTable (int dummy);
  ~EditorTable ();
  GtkTextChildAnchor * childanchor;
  GtkWidget * table;
  vector < vector <GtkWidget * > > textviews;
  vector < vector <GtkTextBuffer * > > textbuffers;
private:
};


class EditorUndo
{
public:
  EditorUndo (int dummy);
  ~EditorUndo ();
  EditorUndoType type;
  ustring text;
  unsigned int startoffset;
  unsigned int endoffset;
  bool flag;
  ustring type2text ();
private:
};


class PreventEditorUndo
{
public:
  PreventEditorUndo (int * flag);
  ~PreventEditorUndo ();
private:
  int * flagpointer;
};


void marker_get_type_and_subtype (const ustring& project, const ustring& marker, StyleType& type, int& subtype);


bool style_get_plaintext (StyleType type, int subtype);
bool style_get_paragraph (StyleType type, int subtype);
bool style_get_starts_new_line_in_editor (StyleType type, int subtype);
bool style_get_starts_new_line_in_usfm (StyleType type, int subtype);
bool style_get_displays_marker (StyleType type, int subtype);
bool style_get_starts_character_style (StyleType type, int subtype);
bool style_get_starts_verse_number (StyleType type, int subtype);
ustring style_get_verse_marker (const ustring& project);
bool style_get_starts_footnote (StyleType type, int subtype);
bool style_get_starts_endnote (StyleType type, int subtype);
bool style_get_starts_crossreference (StyleType type, int subtype);
bool style_get_starts_note_content (StyleType type, int subtype);
ustring style_get_default_note_style (const ustring& project, EditorNoteType type);
ustring style_get_paragraph_note_style (const ustring& project);
bool style_get_starts_table_row (StyleType type, int subtype);
ustring style_get_table_row_marker (const ustring& project);
bool style_get_starts_table_cell (StyleType type, int subtype);
ustring style_get_table_cell_marker (const ustring& project, int column);


void textbuffer_erase_character_before_text_insertion_point_if_space (GtkTextBuffer * textbuffer);
void textbuffer_erase_white_space_at_end (GtkTextBuffer * textbuffer);
void textbuffer_erase_child_anchor (GtkTextBuffer * textbuffer, GtkTextChildAnchor * anchor);
bool textbuffer_empty (GtkTextBuffer * textbuffer);
void textbuffers_set_unmodified (GtkTextBuffer * mainbuffer, vector <EditorNote>& editornotes, vector <EditorTable>& editortables);
bool textbuffers_get_modified (GtkTextBuffer * mainbuffer, vector <EditorNote>& editornotes, vector <EditorTable>& editortables);


EditorNoteType note_type_get (const ustring& project, const ustring& marker);
NoteNumberingType note_numbering_type_get (const ustring& project, const ustring& marker);
ustring note_numbering_user_sequence_get (const ustring& project, const ustring& marker);


gint table_get_n_rows (GtkTable * table);
gint table_get_n_columns (GtkTable * table);
GtkTextBuffer * table_cell_get_buffer (const EditorTable& editortable, gint row, gint column);
GtkWidget * table_cell_get_view (const EditorTable& editortable, gint row, gint column);
void table_resize (EditorTable& editortable, GtkTextTagTable * texttagtable, gint n_rows, gint n_columns, bool editable);
void table_create_cell (GtkTable * table, GtkTextTagTable * texttagtable, GtkWidget * & textview, GtkTextBuffer * & textbuffer, gint row, gint column, bool editable);


void usfm_get_text (GtkTextBuffer * textbuffer, GtkTextIter startiter, GtkTextIter enditer, vector <EditorNote> * editornotes, vector <EditorTable> * editortables, const ustring& project, ustring& text);
void usfm_internal_add_text (ustring& text, const ustring& addition);
void usfm_internal_get_text_close_paragraph_style (ustring& text, const ustring& project, const ustring& style);
void usfm_get_note_text (const EditorNote& editornote, GtkTextIter startiter, GtkTextIter enditer, const ustring& project, ustring& text);
void usfm_get_table_text (const EditorTable& editortable, const ustring& project, ustring& text);
GtkTextIter editor_get_iter_for_note (GtkTextBuffer * textbuffer, const vector <EditorNote>& editornotes, unsigned int offset, unsigned int function);


void get_styles_at_iterator (GtkTextIter iter, ustring& paragraph_style, ustring& character_style);
ustring get_verse_number_at_iterator (GtkTextIter iter, const ustring& verse_marker);


gchar * unknown_style ();
void textbuffer_apply_named_tag (GtkTextBuffer *buffer, const ustring& name, const GtkTextIter *start, const GtkTextIter *end);
void textbuffer_insert_with_named_tags (GtkTextBuffer *buffer, GtkTextIter *iter, const ustring& text, ustring first_tag_name, ustring second_tag_name);


#endif
