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


#ifndef INCLUDED_DIALOGFINDNOTE_H
#define INCLUDED_DIALOGFINDNOTE_H


#include <gtk/gtk.h>


class FindNoteDialog
{
public:
  FindNoteDialog (int dummy);
  ~FindNoteDialog ();
  int run ();
  vector<unsigned int> ids;
protected:
private:
  GtkWidget *findnotedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox2;
  GtkWidget *label5;
  GtkWidget *entry1;
  GtkWidget *checkbutton_case;
  GtkWidget *dialog_action_area1;
  GtkWidget *buttonfind;
  GtkWidget *buttoncancel;
  static void findnotedialog_on_buttonfind_clicked (GtkButton * button, gpointer user_data);
  void on_buttonfind_clicked ();
  vector<ustring> references;
  vector<ustring> allreferences;
  vector<int> dates;
  static void on_word_entry_changed (GtkEditable * editable, gpointer user_data);
  void set_gui ();  
};


#endif
