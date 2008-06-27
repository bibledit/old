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

#include "note_editor.h"
#include "gwrappers.h"
#include "tiny_utilities.h"

NoteEditor::NoteEditor(int dummy) {
}

NoteEditor::~NoteEditor() {
}


void NoteEditor::store_original_data (const ustring& data)
// Stores the data before editing.
{
  original_data = data;
}

void NoteEditor::receive_data_from_html_editor(const char *data, unsigned int len)
// Receive data from the html note editor.
// This function will be called several times until all of the data has been saved.
{
  edited_data.append (data);
}


ustring NoteEditor::clean_edited_data()
/* Clean the data from the html note editor.

A simple note with only the word "test" on one line, looks as below when saved:

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 TRANSITIONAL//EN">
<HTML>
<HEAD>
  <META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=UTF-8">
  <META NAME="GENERATOR" CONTENT="GtkHTML/3.12.1">
</HEAD>
<BODY>
test
</BODY>
</HTML>

We have to strip of the head and the tail and remain with the text within the body only.

*/
{
  // Remove html stuff before the relevant data.
  size_t pos = edited_data.find ("<BODY>");
  if (pos == string::npos) return "";
  edited_data.erase (0, pos + 7);
  // Remove html stuff after the relevant data.
  pos = edited_data.find ("</BODY>");
  if (pos == string::npos) return "";
  edited_data.erase (pos, 1000);
  // Trim off extra newlines at the end, and ensure it always has one.
  edited_data = trim (edited_data);
  edited_data.append ("\n");
  // Give result.  
  return edited_data;
}


bool NoteEditor::data_was_edited ()
// Returns whether the note data was edited.
{
  return (edited_data != original_data);
}

