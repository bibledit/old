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


#ifndef INCLUDED_DIALOGNOTESUPDATE_H
#define INCLUDED_DIALOGNOTESUPDATE_H


#include "libraries.h"
#include <gtk/gtk.h>


class NotesUpdateDialog
{
public:
  NotesUpdateDialog (int dummy);
  ~NotesUpdateDialog ();
  int run ();
protected:
  GtkWidget *notesupdatedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *label6;
  GtkWidget *label4;
  GtkWidget *vbox1;
  GtkWidget *label7;
  GtkWidget *combobox_project_from;
  GtkWidget *label8;
  GtkWidget *combobox_project_to;
  GtkWidget *label5;
  GtkWidget *vbox2;
  GtkWidget *label10;
  GtkWidget *combobox_category_from;
  GtkWidget *label11;
  GtkWidget *combobox_category_to;
  GtkWidget *label9;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_ok ();
};


#endif
