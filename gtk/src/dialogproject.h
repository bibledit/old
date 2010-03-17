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


#ifndef INCLUDED_DIALOGPROJECT_H
#define INCLUDED_DIALOGPROJECT_H


#include <gtk/gtk.h>
#include "settings.h"


class ProjectDialog
{
public:
  ProjectDialog (bool newproject);
  ~ProjectDialog ();
  int run ();
  ustring newprojectname;
protected:
  GtkWidget *projectdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *nameentry;
  GtkWidget *messagelabel;
  GtkWidget *checkbutton_editable;
  GtkWidget *table1;
  GtkWidget *label9;
  GtkWidget *label8;
  GtkWidget *vseparator1;
  GtkWidget *addbutton;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label5;
  GtkWidget *deletebutton;
  GtkWidget *alignment2;
  GtkWidget *hbox3;
  GtkWidget *image2;
  GtkWidget *label6;
  GtkWidget *hseparator1;
  GtkWidget *table2;
  GtkWidget *label11;
  GtkWidget *combobox_versification;
  GtkWidget *label12;
  GtkWidget *combobox_language;
  GtkWidget *checkbutton_right_to_left;
  GtkWidget *hbox_depend;
  GtkWidget *checkbutton_dependent;
  GtkWidget *button_depend;
  GtkWidget *label_depend;
  GtkWidget *combobox_depend;
  GtkWidget *hbox_spelling;
  GtkWidget *checkbutton_spelling;
  GtkWidget *button_dictionaries;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  ustring currentprojectname;
  void set_gui ();
  void on_book_add ();
  void on_book_delete ();
  void on_ok ();
  void on_cancel ();
  static void projectdialog_on_nameentry_changed (GtkEditable * editable, gpointer user_data);
  static void projectdialog_on_okbutton1_clicked (GtkButton * button, gpointer user_data);
  static void projectdialog_on_addbutton_clicked (GtkButton * button, gpointer user_data);
  static void projectdialog_on_deletebutton_clicked (GtkButton * button, gpointer user_data);
  static void projectdialog_on_cancelbutton1_clicked (GtkButton * button, gpointer user_data);
  static void on_checkbutton_editable_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_checkbutton_dependent_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_dependent ();
  static void on_button_depend_clicked (GtkButton *button, gpointer user_data);
  void on_button_depend ();
  ustring dependent_project (const ustring& project);
  ustring none_project ();
  ustring dependent_project ();
  static void on_button_dictionaries_clicked (GtkButton *button, gpointer user_data);
  void on_button_dictionaries ();
  static void on_checkbutton_spelling_toggled (GtkToggleButton *togglebutton, gpointer user_data);
};


#endif
