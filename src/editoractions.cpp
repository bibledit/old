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


unsigned int new_action_object_identifier ()
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
  my_identifier = new_action_object_identifier ();
  parent_identifier = 0;
  my_pointer = NULL;
  my_integer = 0;
}


EditorAction::~EditorAction()
{
}


void EditorAction::describe ()
{
  ustring description;
  switch (type) {
    case eatCreateParagraphWidget: description = "CreateParagraphWidget"; break;
    case eatSetParagraphWidgetStyle: description = "SetParagraphWidgetStyle"; break;
    case eatInsertText: description = "InsertText"; break;
  }
  gw_message (description);
}

