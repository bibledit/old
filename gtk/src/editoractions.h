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


#ifndef INCLUDED_EDITOR_ACTIONS_H
#define INCLUDED_EDITOR_ACTIONS_H


#include "libraries.h"
#include <gtk/gtk.h>


enum EditorActionType {
  eatCreateParagraph,
  eatChangeParagraphStyle,
  eatInsertText,
  eatDeleteText,
  eatChangeCharacterStyle,
  eatLoadChapterBoundary,
  eatOneActionBoundary,
  eatDeleteParagraph,
  eatCreateNoteParagraph
};


enum EditorActionApplication {
  eaaInitial,
  eaaUndo,
  eaaRedo
};


class EditorAction
{
public:
  EditorAction(EditorActionType type_in);
  virtual ~EditorAction();
  void apply (deque <EditorAction *>& done);
  void undo (deque <EditorAction *>& done, deque <EditorAction *>& undone);
  void redo (deque <EditorAction *>& done, deque <EditorAction *>& undone);
  EditorActionType type;
private:
};


class EditorActionCreateParagraph : public EditorAction
{
public:
  EditorActionCreateParagraph(GtkWidget * vbox);
  virtual ~EditorActionCreateParagraph();
  friend class EditorActionCreateNoteParagraph;
  friend class EditorActionDeleteParagraph;
  void apply (GtkTextTagTable * texttagtable, bool editable, EditorActionCreateParagraph * focused_paragraph, GtkWidget *& to_focus);
  void undo (GtkWidget * parking_vbox, GtkWidget *& to_focus);
  void redo (GtkWidget *& to_focus);
  GtkWidget * textview;
  GtkTextBuffer * textbuffer;
  ustring style;
private:
  gint offset_at_delete;
  GtkWidget * parent_vbox;
};


class EditorActionChangeParagraphStyle : public EditorAction
{
public:
  EditorActionChangeParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action);
  virtual ~EditorActionChangeParagraphStyle();
  void apply (GtkWidget *& to_focus);
  void undo (GtkWidget *& to_focus);
  void redo (GtkWidget *& to_focus);
private:
  EditorActionCreateParagraph * paragraph;
  ustring previous_style;
  ustring current_style;
  void set_style (const ustring& style);
};


class EditorActionInsertText : public EditorAction
{
public:
  EditorActionInsertText(EditorActionCreateParagraph * parent_action, gint offset_in, const ustring& text_in);
  virtual ~EditorActionInsertText();
  void apply (GtkWidget *& to_focus);
  void undo (GtkWidget *& to_focus);
  void redo (GtkWidget *& to_focus);
private:
  EditorActionCreateParagraph * paragraph;
  gint offset;
  ustring text;
};


class EditorActionDeleteText : public EditorAction
{
public:
  EditorActionDeleteText(EditorActionCreateParagraph * parent_action, gint offset_in, gint length_in);
  virtual ~EditorActionDeleteText();
  EditorActionCreateParagraph * paragraph;
  void apply (GtkWidget *& to_focus);
  void undo (GtkWidget *& to_focus);
  void redo (GtkWidget *& to_focus);
private:
  gint offset;
  gint length;
  vector <ustring> deleted_text;
  vector <ustring> deleted_styles;
};


class EditorActionChangeCharacterStyle : public EditorAction
{
public:
  EditorActionChangeCharacterStyle(EditorActionCreateParagraph * parent_action, const ustring& style_in, gint offset_in, gint length_in);
  virtual ~EditorActionChangeCharacterStyle();
  void apply (GtkWidget *& to_focus);
  void undo (GtkWidget *& to_focus);
  void redo (GtkWidget *& to_focus);
private:
  EditorActionCreateParagraph * paragraph;
  ustring style;
  gint offset;
  gint length;
  vector <ustring> previous_styles;
  void change_styles (const vector <ustring>& old_ones, const vector <ustring>& new_ones); 
};


class EditorActionDeleteParagraph : public EditorAction
{
public:
  EditorActionDeleteParagraph(EditorActionCreateParagraph * paragraph_in);
  virtual ~EditorActionDeleteParagraph();
  void apply(GtkWidget * parking_vbox, GtkWidget *& to_focus);
  void undo (GtkWidget *& to_focus);
  void redo (GtkWidget * parking_vbox, GtkWidget *& to_focus);
private:
  EditorActionCreateParagraph * paragraph;
  gint offset;
};


class EditorActionCreateNoteParagraph : public EditorActionCreateParagraph
{
public:
  EditorActionCreateNoteParagraph(GtkWidget * vbox, const ustring& marker_in, const ustring& caller_usfm_in, const ustring& caller_text_in, const ustring& identifier_in);
  virtual ~EditorActionCreateNoteParagraph();
  void apply (GtkTextTagTable * texttagtable, bool editable, EditorActionCreateParagraph * focused_paragraph, GtkWidget *& to_focus);
  void undo  (GtkWidget * parking_vbox, GtkWidget *& to_focus);
  void redo  (GtkWidget *& to_focus);
  GtkWidget * hbox;
  GtkWidget * eventbox;
  ustring identifier;
  ustring opening_closing_marker;
  ustring caller_usfm;
private:
  ustring caller_text;
  GtkWidget * label;
  static gboolean on_caller_enter_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_caller_enter_notify (GdkEventCrossing *event);
  static gboolean on_caller_leave_notify_event (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
  gboolean on_caller_leave_notify (GdkEventCrossing *event);
};


#endif
