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


#ifndef INCLUDED_DIALOGSETSTATUS_H
#define INCLUDED_DIALOGSETSTATUS_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "libraries.h"
#include "reporting.h"


class SetStatusDialog
{
public:
  SetStatusDialog (ProjectStatusRecord * statusrecord_in);
  ~SetStatusDialog ();
  int run ();
protected:
  GtkWidget *setstatusdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox4;
  GtkWidget *checkbutton_status;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_checkbutton_status_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  vector <ustring> tasks;
  vector <unsigned int> percentages;
  ProjectStatusRecord * statusrecord;
  vector <GtkWidget *> checkbuttons;
};


#endif
