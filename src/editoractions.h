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
  eatInsertText
};


unsigned int next_action_object_identifier ();


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
  unsigned int identifier;
  ustring style;
private:
};


class EditorActionSetParagraphStyle : public EditorAction
{
public:
  EditorActionSetParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action);
  virtual ~EditorActionSetParagraphStyle();
  unsigned int parent_identifier;
  ustring previous_style;
  ustring current_style;
private:
};


class EditorActionInsertText : public EditorAction
{
public:
  EditorActionInsertText(const ustring& text_in, EditorActionCreateParagraph * parent_action);
  virtual ~EditorActionInsertText();
  unsigned int parent_identifier;
  ustring text;
private:
};


#endif
