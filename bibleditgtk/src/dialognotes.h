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


#ifndef INCLUDED_DIALOGNOTES_H
#define INCLUDED_DIALOGNOTES_H


#include <gtk/gtk.h>


class NotesDialog
{
public:
  NotesDialog (int dummy);
  ~NotesDialog ();
  int run ();
protected:
  GtkWidget *notesdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *vbox1;
  GtkWidget *label3;
  GtkWidget *table1;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *entry1;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *entry4;
  GtkWidget *label8;
  GtkWidget *vbox2;
  GtkWidget *label10;
  GtkWidget *treeview1;
  GtkWidget *hbox1;
  GtkWidget *buttondelete;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label11;
  GtkWidget *buttonadd;
  GtkWidget *alignment2;
  GtkWidget *hbox3;
  GtkWidget *image2;
  GtkWidget *label12;
  GtkWidget *label9;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_buttondelete_clicked (GtkButton *button, gpointer user_data);
  void on_delete_category ();
  static void on_buttonadd_clicked (GtkButton *button, gpointer user_data);
  void on_add_category ();
  void set_gui(const vector<ustring>& categories, const ustring& display_value);
  GtkListStore *model;
};


#endif
