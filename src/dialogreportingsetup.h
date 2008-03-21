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


#ifndef INCLUDED_DIALOGREPORTINGSETUP_H
#define INCLUDED_DIALOGREPORTINGSETUP_H


#include <gtk/gtk.h>
#include "ustring.h"


class ReportingSetupDialog
{
public:
  ReportingSetupDialog (int dummy);
  ~ReportingSetupDialog ();
  int run ();
protected:
  GtkWidget *reportingsetupdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *button_tasks;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label2;
  GtkWidget *hbox3;
  GtkWidget *button_time;
  GtkWidget *alignment2;
  GtkWidget *hbox4;
  GtkWidget *image2;
  GtkWidget *label5;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_button_tasks_clicked (GtkButton *button, gpointer user_data);
  void on_button_tasks ();
  static void on_button_time_clicked (GtkButton *button, gpointer user_data);
  void on_button_time ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  vector <ustring> tasks;
  vector <double> durations;
  vector <ustring> tasks_ever;
  vector <double> durations_ever;
};


#endif
