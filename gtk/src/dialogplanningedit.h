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


#ifndef INCLUDED_DIALOGPLANNING_EDIT_H
#define INCLUDED_DIALOGPLANNING_EDIT_H


#include <gtk/gtk.h>
#include "ustring.h"


class PlanningEditDialog
{
public:
  PlanningEditDialog (unsigned int book, unsigned int chapter);
  ~PlanningEditDialog ();
  int run ();
protected:
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;
  GtkWidget *button_status;
  GtkWidget *button_tasks;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  unsigned int mybook;
  unsigned int mychapter;
  
  static void on_button_status_clicked (GtkButton *button, gpointer user_data);
  void on_button_status ();

  static void on_button_tasks_clicked (GtkButton *button, gpointer user_data);
  void on_button_tasks ();

  static void on_button_time_clicked (GtkButton *button, gpointer user_data);
  void on_button_time ();

  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();

  void gui ();
};


#endif
