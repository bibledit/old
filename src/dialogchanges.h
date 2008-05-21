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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_DIALOGCHANGES_H
#define INCLUDED_DIALOGCHANGES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "references.h"


class ChangesDialog
{
public:
  ChangesDialog (References * references);
  ~ChangesDialog ();
  int run ();
protected:
  GtkWidget *changesdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label2;
  GtkWidget *hbox3;
  GtkWidget *radiobutton_since_last;
  GtkWidget *label_last_review;
  GtkWidget *hbox5;
  GtkWidget *radiobutton_since_date;
  GtkWidget *label_since;
  GtkWidget *button_since;
  GtkWidget *alignment2;
  GtkWidget *hbox6;
  GtkWidget *image2;
  GtkWidget *label6;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_radiobutton_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_button_since_clicked (GtkButton *button, gpointer user_data);
  void on_button_since ();
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void gui ();
  References * myreferences;
  ustring temporal_project;
  void view_local_changes (bool changes_since_last_review);
  unsigned int oldest_commit ();
};


#endif
