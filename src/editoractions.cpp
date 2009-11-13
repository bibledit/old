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

#include "editoractions.h"
#include "gwrappers.h"
#include "editor_aids.h"


unsigned int next_action_object_identifier ()
{
  static unsigned int action_object_identifier = 0;
  action_object_identifier++;
  if (action_object_identifier == 0)
    action_object_identifier++;
  return action_object_identifier;
}


EditorAction::EditorAction(EditorActionType type_in)
{
  type = type_in;
}


EditorAction::~EditorAction ()
{
}


EditorActionCreateParagraph::EditorActionCreateParagraph(int dummy) :
EditorAction (eatCreateParagraph)
{
  widget = NULL;
  identifier = next_action_object_identifier ();
  style = unknown_style();
}


EditorActionCreateParagraph::~EditorActionCreateParagraph ()
{
}


EditorActionSetParagraphStyle::EditorActionSetParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action) :
EditorAction (eatSetParagraphStyle)
{
  parent_identifier = parent_action->identifier;
  previous_style = parent_action->style;
  current_style = style;
}


EditorActionSetParagraphStyle::~EditorActionSetParagraphStyle ()
{
}


EditorActionInsertText::EditorActionInsertText(const ustring& text_in, EditorActionCreateParagraph * parent_action) :
EditorAction (eatInsertText)
{
  parent_identifier = parent_action->identifier;
  text = text_in;
}


EditorActionInsertText::~EditorActionInsertText ()
{
}


