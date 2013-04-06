/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


#include "note_editor.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "html.h"


NoteEditor::NoteEditor(int dummy)
{
}


NoteEditor::~NoteEditor()
{
}


void NoteEditor::store_original_data(const ustring & data)
// Stores the data before editing.
{
  original_data = data;
}


void NoteEditor::receive_data_from_html_editor(const char *data)
// Receive data from the html note editor.
{
  edited_data =  data;
}


ustring NoteEditor::clean_edited_data ()
// Clean the data from the html note editor.
{
  edited_data = trim (edited_data);
  return edited_data;
}


bool NoteEditor::data_was_edited()
// Returns whether the note data was edited.
{
  return (edited_data != original_data);
}
