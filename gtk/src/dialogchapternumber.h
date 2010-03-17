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


#ifndef INCLUDED_DIALOGCHAPTERNUMBER_H
#define INCLUDED_DIALOGCHAPTERNUMBER_H


#include "utilities.h"


class ChapterNumberDialog
{
public:
  ChapterNumberDialog (int dummy);
  ~ChapterNumberDialog ();
  int run ();
protected:
  GtkWidget *dialogchapternumber;
  GtkWidget *dialog_vbox1;
  GtkWidget *label_purpose;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *entry1;
  GtkWidget *label_info;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_ok ();
  static void on_entry1_changed (GtkEditable *editable, gpointer user_data);
  void set_gui ();
  unsigned int get_chapter ();
};


#endif
