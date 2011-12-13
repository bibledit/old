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


#ifndef INCLUDED_DIALOGAREA_H
#define INCLUDED_DIALOGAREA_H


#include <gtk/gtk.h>
#include "types.h"


class AreaDialog
{
public:
  AreaDialog (int dummy);
  ~AreaDialog ();
  int run ();
protected:
  GtkWidget *areadialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label4;
  GtkWidget *radiobutton_raw;
  GtkWidget *radiobutton_all;
  GtkWidget *radiobutton_categories;
  GtkWidget *checkbutton_id;
  GtkWidget *checkbutton_intro;
  GtkWidget *checkbutton_heading;
  GtkWidget *checkbutton_chapter;
  GtkWidget *checkbutton_study;
  GtkWidget *checkbutton_notes;
  GtkWidget *checkbutton_xref;
  GtkWidget *checkbutton_verse;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void companiondialog_on_okbutton_clicked (GtkButton *button, gpointer user_data);
  static void on_radiobutton_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_ok ();
  void set_gui ();
  AreaType get_area_type ();
  void set_selectors_sensitive (bool sensitive);
};


ustring area_information ();


#endif
