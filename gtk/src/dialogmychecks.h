/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_DIALOMYCHECKS_H
#define INCLUDED_DIALOMYCHECKS_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "windowreferences.h"


class MyChecksDialog
{
public:
  MyChecksDialog (WindowReferences * references_window);
  ~MyChecksDialog ();
  int run ();
protected:
  GtkWidget *mychecksdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *checkbutton_chapters_verses;
  GtkWidget *hseparator1;
  GtkWidget *label2;
  GtkWidget *checkbutton_markers_validate;
  GtkWidget *checkbutton_markers_count;
  GtkWidget *checkbutton_markers_compare;
  GtkWidget *checkbutton_markers_spacing;
  GtkWidget *hseparator2;
  GtkWidget *label3;
  GtkWidget *checkbutton_characters_inventory;
  GtkWidget *checkbutton_characters_unwanted_patterns;
  GtkWidget *hseparator3;
  GtkWidget *label4;
  GtkWidget *checkbutton_words_capitalization;
  GtkWidget *checkbutton_words_repetition;
  GtkWidget *checkbutton_words_unwanted;
  GtkWidget *checkbutton_words_inventory;
  GtkWidget *hseparator4;
  GtkWidget *label5;
  GtkWidget *checkbutton_punctuation_matching_pairs;
  GtkWidget *hseparator5;
  GtkWidget *label6;
  GtkWidget *checkbutton_references_inventory;
  GtkWidget *checkbutton_references_validate;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  WindowReferences * my_references_window;
};


#endif
