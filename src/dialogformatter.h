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


#ifndef INCLUDED_DIALOGFORMATTER_H
#define INCLUDED_DIALOGFORMATTER_H


#include <gtk/gtk.h>


class FormatterDialog
{
public:
  FormatterDialog (int dummy);
  ~FormatterDialog ();
  int run ();
protected:
private:
  GtkWidget *formatterdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *expander1;
  GtkWidget *label_expander;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *image_java;
  GtkWidget *label_java_gui;
  GtkWidget *label24;
  GtkWidget *button_java;
  GtkWidget *alignment3;
  GtkWidget *hbox6;
  GtkWidget *image3;
  GtkWidget *label25;
  GtkWidget *label_install;
  GtkWidget *image_install;
  GtkWidget *label_install_gui;
  GtkWidget *label31;
  GtkWidget *button_install;
  GtkWidget *alignment6;
  GtkWidget *hbox11;
  GtkWidget *image10;
  GtkWidget *label32;
  GtkWidget *label_license;
  GtkWidget *image_license;
  GtkWidget *label_license_gui;
  GtkWidget *label29;
  GtkWidget *button_license;
  GtkWidget *alignment5;
  GtkWidget *hbox10;
  GtkWidget *image8;
  GtkWidget *label30;
  GtkWidget *label_package;
  GtkWidget *image_xep;
  GtkWidget *label_xep_gui;
  GtkWidget *label27;
  GtkWidget *button_xep;
  GtkWidget *alignment4;
  GtkWidget *hbox8;
  GtkWidget *image6;
  GtkWidget *label28;
  GtkWidget *label_java;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_button_java_clicked (GtkButton *button, gpointer user_data);
  void on_button_java ();
  static void on_button_xep_clicked (GtkButton *button, gpointer user_data);
  void on_button_xep ();
  static void on_button_license_clicked (GtkButton *button, gpointer user_data);
  void on_button_license ();
  static void on_button_install_clicked (GtkButton *button, gpointer user_data);
  void on_button_install ();
  void set_gui ();
  ustring java_binary_path;
  ustring xep_package_path;
  ustring license_path;
  ustring xep_home;
};


#endif
