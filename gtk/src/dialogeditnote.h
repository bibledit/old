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


#ifndef INCLUDED_DIALOG_EDIT_NOTE_H
#define INCLUDED_DIALOG_EDIT_NOTE_H


#include <gtk/gtk.h>
#include "types.h"
#include "editor.h"


class EditNoteDialog
{
public:
  EditNoteDialog (Editor2 * editor);
  ~EditNoteDialog ();
  int run ();
protected:
  GtkWidget *editnotedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *radiobutton_numbering_automatic;
  GtkWidget *radiobutton_numbering_none;
  GtkWidget *radiobutton_numbering_character;
  GtkWidget *entry_numbering;
  GtkWidget *hseparator5;
  GtkWidget *hbox7;
  GtkWidget *label4;
  GtkWidget *combobox1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  Editor2 * myeditor;
  EditorActionCreateNoteParagraph * note_paragraph;
  static void on_radiobutton_numbering_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_numbering (GtkToggleButton *togglebutton);
  int numbering_get ();
  void numbering_set (int numbering);
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  ustring get_marker (const ustring& text);
  ustring switch_marker (const ustring& marker, bool switch_to_xref);
};


#endif
