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

#ifndef INCLUDED_DIALOG_PROJECT_NOTE_H
#define INCLUDED_DIALOG_PROJECT_NOTE_H

#include <gtk/gtk.h>
#include "ustring.h"

class ProjectNoteDialog
{
public:
  ProjectNoteDialog(GtkWidget * parent, const vector <ustring>& projects_in, const ustring& project_in, const ustring& created_on, const ustring& created_by, const ustring& edited_on, const ustring& logbook);
  ~ProjectNoteDialog();
  int run();
  ustring project;
protected:
  GtkWidget *dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label_note_project;
  GtkWidget *combobox_note_project;
  GtkWidget *label_note_created_on;
  GtkWidget *label_note_created_by;
  GtkWidget *label_note_edited_on;
  GtkWidget *label_note_logbook;
  GtkWidget *textview_note_logbook;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_okbutton1_clicked(GtkButton *button, gpointer user_data);
  void on_okbutton1();
};

#endif
