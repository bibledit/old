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


#ifndef INCLUDED_DIALOGPLANNINGSETUP_H
#define INCLUDED_DIALOGPLANNINGSETUP_H


#include <gtk/gtk.h>
#include "ustring.h"


class PlanningSetupDialog
{
public:
  PlanningSetupDialog (int dummy);
  ~PlanningSetupDialog ();
  int run ();
protected:
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;
  GtkWidget *button_start;
  GtkWidget *button_tasks;
  GtkWidget *button_time;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_button_start_clicked (GtkButton *button, gpointer user_data);
  void on_button_start ();
  int startdate;

  static void on_button_tasks_clicked (GtkButton *button, gpointer user_data);
  void on_button_tasks ();
  vector <ustring> tasks;
  vector <ustring> tasks_ever;

  static void on_button_time_clicked (GtkButton *button, gpointer user_data);
  void on_button_time ();
  vector <double> durations;
  vector <double> durations_ever;

  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();

  void gui ();
};


#endif
