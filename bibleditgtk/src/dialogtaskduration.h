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


#ifndef INCLUDED_DIALOGTASKDURATION_H
#define INCLUDED_DIALOGTASKDURATION_H


#include <gtk/gtk.h>
#include "ustring.h"


class TaskDurationDialog
{
public:
  TaskDurationDialog (vector <ustring> * tasks, vector <double> * durations);
  ~TaskDurationDialog ();
  int run ();
protected:
  GtkWidget *taskdurationdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label4;
  GtkWidget *scrolledwindow;
  GtkWidget *viewport;
  GtkWidget *table1;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label_task;
  GtkObject *spinbutton1_adj;
  GtkWidget *spinbutton1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton1 ();
  vector <ustring> * mytasks;
  vector <double> * mydurations;
  vector <GtkWidget *> spinbuttons;
};


#endif
