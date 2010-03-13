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


#ifndef INCLUDED_DIALOGORIGINREFERENCES_H
#define INCLUDED_DIALOGORIGINREFERENCES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "types.h"


class OriginReferencesDialog
{
public:
  OriginReferencesDialog (int dummy);
  ~OriginReferencesDialog ();
  int run ();
protected:
  GtkWidget *originreferencesdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label3;
  GtkWidget *hseparator1;
  GtkWidget *label4;
  GtkWidget *checkbutton_footnotes;
  GtkWidget *checkbutton_endnotes;
  GtkWidget *checkbutton_xrefs;
  GtkWidget *hseparator2;
  GtkWidget *label5;
  GtkWidget *radiobutton_nothing;
  GtkWidget *radiobutton_remove;
  GtkWidget *hbox1;
  GtkWidget *radiobutton_add;
  GtkWidget *checkbutton_book;
  GtkWidget *checkbutton_chapter;
  GtkWidget *entry_dot;
  GtkWidget *checkbutton_verse;
  GtkWidget *entry2;
  GtkWidget *vseparator1;
  GtkWidget *label_example;
  GtkWidget *radiobutton_text_label;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_radiobutton_action_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_radiobutton_action ();
  static void on_checkbutton_reference_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_entry_reference_changed (GtkEditable *editable, gpointer user_data);
  void on_reference ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void get_relevant_markers ();
  OriginReferencesActionType action;
};


#endif
