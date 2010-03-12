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


#ifndef INCLUDED_DIALOGFILTERS_H
#define INCLUDED_DIALOGFILTERS_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "ustring.h"


class FiltersDialog
{
public:
  FiltersDialog (int dummy);
  ~FiltersDialog ();
  int run ();
protected:
  GtkWidget *filterdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label_rules;
  GtkWidget *scrolledwindow_rules;
  GtkWidget *textview_rules;
  GtkWidget *label2;
  GtkWidget *hbox2;
  GtkWidget *scrolledwindow_input;
  GtkWidget *textview_input;
  GtkWidget *vbox1;
  GtkWidget *button_try;
  GtkWidget *alignment1;
  GtkWidget *hbox3;
  GtkWidget *image1;
  GtkWidget *label3;
  GtkWidget *scrolledwindow_output;
  GtkWidget *textview_output;
  GtkWidget *hbox1;
  GtkWidget *button_new;
  GtkWidget *combobox_filters;
  GtkWidget *label_type;
  GtkWidget *button_delete;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  void load_filters (const ustring& selection);
  static void on_button_try_clicked (GtkButton *button, gpointer user_data);
  void on_button_try ();
  vector <ustring> compile_errors;
  static void on_button_new_clicked (GtkButton *button, gpointer user_data);
  void on_button_new ();
  static void on_combobox_filters_changed (GtkComboBox *combobox, gpointer user_data);
  void on_combobox_filters ();
  static void on_button_delete_clicked (GtkButton *button, gpointer user_data);
  void on_button_delete ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  GtkTextBuffer * rulesbuffer;
  static void on_rulesbuffer_changed (GtkTextBuffer * textbuffer, gpointer user_data);
  void on_rulesbuffer ();
  guint rulesbuffer_changed_event_id;
  static bool on_rulesbuffer_changed_timeout (gpointer user_data);
  void on_rulesbuffer_changed_execute ();

};


#endif
