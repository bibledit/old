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
  // The type of this EditorAction.
  type = type_in;
}


EditorAction::~EditorAction ()
{
}


EditorActionCreateParagraph::EditorActionCreateParagraph(int dummy) :
EditorAction (eatCreateParagraph)
{
  // Pointer to the GtkTextView. To be set when the GtkTextView has been created.
  widget = NULL;
  // New unique identifier for this paragraph.
  // It works with an identifier rather than with a pointer to the widget,
  // since the identifier remains the same throughout Undo and Redo actions, 
  // but the address of the widget changes each time when old ones are destroyed and new ones created.
  identifier = next_action_object_identifier ();
  // The default style of the paragraph will be "unknown".
  style = unknown_style();
}


EditorActionCreateParagraph::~EditorActionCreateParagraph ()
{
}


EditorActionSetParagraphStyle::EditorActionSetParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action) :
EditorAction (eatSetParagraphStyle)
{
  // Identifier of the EditorAction object that created the paragraph whose style it going to be set.
  parent_identifier = parent_action->identifier;
  // The style of the paragraph before the new style was applied.
  previous_style = parent_action->style;
  // The new style for the paragraph.
  current_style = style;
}


EditorActionSetParagraphStyle::~EditorActionSetParagraphStyle ()
{
}


EditorActionInsertText::EditorActionInsertText(EditorActionCreateParagraph * parent_action, gint offset_in, const ustring& text_in) :
EditorAction (eatInsertText)
{
  // The identifier of the paragraph to operate on.
  parent_identifier = parent_action->identifier;
  // Where to insert the text, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The text to insert.
  text = text_in;
}


EditorActionInsertText::~EditorActionInsertText ()
{
}


EditorActionDeleteText::EditorActionDeleteText(EditorActionCreateParagraph * parent_action, gint offset_in, gint length_in) :
EditorAction (eatDeleteText)
{
  // The identifier of the paragraph to operate on.
  parent_identifier = parent_action->identifier;
  // Where to start deleting the text, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The length of the text to be deleted.
  length = length_in;
  // The text which was deleted will be set when this action is executed.
}


EditorActionDeleteText::~EditorActionDeleteText ()
{
}


