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
  // The default style of the paragraph will be "unknown".
  style = unknown_style();
  // Offset of this widget at time of deletion.
  offset_at_delete = -1;
}


EditorActionCreateParagraph::~EditorActionCreateParagraph ()
{
}


EditorActionChangeParagraphStyle::EditorActionChangeParagraphStyle(const ustring& style, EditorActionCreateParagraph * parent_action) :
EditorAction (eatChangeParagraphStyle)
{
  // The EditorAction object that created the paragraph whose style it going to be set.
  paragraph = parent_action;
  // The style of the paragraph before the new style was applied.
  previous_style = parent_action->style;
  // The new style for the paragraph.
  current_style = style;
}


EditorActionChangeParagraphStyle::~EditorActionChangeParagraphStyle ()
{
}


EditorActionInsertText::EditorActionInsertText(EditorActionCreateParagraph * parent_action, gint offset_in, const ustring& text_in) :
EditorAction (eatInsertText)
{
  // The paragraph to operate on.
  paragraph = parent_action;
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
  // The paragraph to operate on.
  paragraph = parent_action;
  // Where to start deleting the text, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The length of the text to be deleted.
  length = length_in;
  // The text which was deleted will be set when this action is executed.
}


EditorActionDeleteText::~EditorActionDeleteText ()
{
}


EditorActionChangeCharacterStyle::EditorActionChangeCharacterStyle(EditorActionCreateParagraph * parent_action, const ustring& style_in, gint offset_in, gint length_in) :
EditorAction (eatChangeCharacterStyle)
{
  // The identifier of the paragraph to operate on.
  paragraph = parent_action;
  // The name of the style.
  style = style_in;
  // Where to start applying the style, that is, at which offset within the GtkTextBuffer.
  offset = offset_in;
  // The length of the text where the style is to be applied.
  length = length_in;
  // The previous styles are stored per character when this action is executed.
}


EditorActionChangeCharacterStyle::~EditorActionChangeCharacterStyle ()
{
}


EditorActionDeleteParagraph::EditorActionDeleteParagraph(EditorActionCreateParagraph * paragraph_in) :
EditorAction (eatDeleteParagraph)
{
  // The identifier of the paragraph to operate on.
  paragraph = paragraph_in;
  // Initialize the offset within the parent GtkBox.
  offset = -1;
}

EditorActionDeleteParagraph::~EditorActionDeleteParagraph ()
{
}


