/*
** Copyright (©) 2003 The Free Software Foundation.
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


#ifndef INCLUDED_DIALOGPRINTPROJECT_H
#define INCLUDED_DIALOGPRINTPROJECT_H


#include <gtk/gtk.h>
#include "ustring.h"


class PrintProjectDialog
{
public:
  PrintProjectDialog (int dummy);
  ~PrintProjectDialog ();
  int run ();
protected:
  GtkWidget *printprojectdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *label_portion;
  GtkWidget *button_portion;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label5;
  GtkWidget *expander1;
  GtkWidget *vbox_expander;
  GtkWidget *checkbutton_full_references;
  GtkWidget *label_expander;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_button_portion_clicked (GtkButton *button, gpointer user_data);
  void on_button_portion ();
  void gui_reorder_include ();
};


#endif
