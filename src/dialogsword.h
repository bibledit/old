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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_DIALOGENTRY_H
#define INCLUDED_DIALOGENTRY_H


#include <gtk/gtk.h>
#include "ustring.h"


class SwordDialog
{
public:
  SwordDialog (int dummy);
  ~SwordDialog ();
  int run ();
protected:
  GtkWidget *sworddialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label11;
  GtkWidget *hseparator1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *hbox7;
  GtkWidget *labelname;
  GtkWidget *entry_name;
  GtkWidget *hbox8;
  GtkWidget *labeldescription;
  GtkWidget *entry_description;
  GtkWidget *hbox9;
  GtkWidget *labelabout;
  GtkWidget *entry_about;
  GtkWidget *entry_lcsh;
  GtkWidget *entry_license;
  GtkWidget *entry_version;
  GtkWidget *hbox5;
  GtkWidget *entry_language;
  GtkWidget *button_language;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label14;
  GtkWidget *hbox1;
  GtkWidget *entry_installpath;
  GtkWidget *button_installpath;
  GtkWidget *alignment1;
  GtkWidget *hbox3;
  GtkWidget *image1;
  GtkWidget *label12;
  GtkWidget *hbox2;
  GtkWidget *entry_modulepath;
  GtkWidget *button_modulepath;
  GtkWidget *alignment2;
  GtkWidget *hbox4;
  GtkWidget *image2;
  GtkWidget *label13;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_button_installpath_clicked (GtkButton *button, gpointer user_data);
  void on_button_installpath ();
  static void on_button_modulepath_clicked (GtkButton *button, gpointer user_data);
  void on_button_modulepath ();
  static void on_button_language_clicked (GtkButton *button, gpointer user_data);
  void on_button_language ();
};


#endif
