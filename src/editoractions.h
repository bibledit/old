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


#ifndef INCLUDED_EDITOR_ACTIONS_H
#define INCLUDED_EDITOR_ACTIONS_H


#include "libraries.h"
#include <gtk/gtk.h>


enum EditorActionType {
  eatCreateParagraph,
  eatSetParagraphStyle,
  eatInsertText,
  eatDeleteText,
  eatApplyStyle
};


class EditorAction
{
public:
  EditorAction(EditorActionType type_in);
  virtual ~EditorAction();
  EditorActionType type;
private:
};


class EditorActionCreateParagraph : public EditorAction
{
public:
  EditorActionCreateParagraph(int dummy);
  virtual ~EditorActionCreateParagraph();
  GtkWidget * widget;
  ustring style;
private:
};


class EditorActionSetParagraphStyle : public EditorAction
{
public:
  EditorActionSetParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action);
  virtual ~EditorActionSetParagraphStyle();
  EditorActionCreateParagraph * paragraph;
  ustring previous_style;
  ustring current_style;
private:
};


class EditorActionInsertText : public EditorAction
{
public:
  EditorActionInsertText(EditorActionCreateParagraph * parent_action, gint offset_in, const ustring& text_in);
  virtual ~EditorActionInsertText();
  EditorActionCreateParagraph * paragraph;
  gint offset;
  ustring text;
private:
};


class EditorActionDeleteText : public EditorAction
{
public:
  EditorActionDeleteText(EditorActionCreateParagraph * parent_action, gint offset_in, gint length_in);
  virtual ~EditorActionDeleteText();
  EditorActionCreateParagraph * paragraph;
  gint offset;
  gint length;
  ustring deleted_text;
private:
};


class EditorActionApplyTextStyle : public EditorAction
{
public:
  EditorActionApplyTextStyle(EditorActionCreateParagraph * parent_action, const ustring& style_in, gint offset_in, gint length_in);
  virtual ~EditorActionApplyTextStyle();
  EditorActionCreateParagraph * paragraph;
  ustring style;
  gint offset;
  gint length;
  vector <ustring> previous_styles;
private:
};


#endif
