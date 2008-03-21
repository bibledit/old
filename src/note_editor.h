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


#ifndef INCLUDED_NOTE_EDITOR_H
#define INCLUDED_NOTE_EDITOR_H


#include "libraries.h"
#include <gtk/gtk.h>


class NoteEditor
// Used while a note is being edited.
{
public:
  NoteEditor (int dummy);
  ~NoteEditor ();
  int id;
  bool newnote;
  unsigned int date_created;
  unsigned int date_modified;
  ustring created_by;
  GtkTextBuffer * textbuffer_references;
  GtkTextBuffer * textbuffer_note;
  GtkTextBuffer * textbuffer_logbook;
  GtkWidget * previous_focus;
  int previous_tools_page;
  ustring previous_category;
  ustring previous_project;
private:
};


#endif
