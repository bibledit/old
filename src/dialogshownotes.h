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


#ifndef INCLUDED_DIALOGSHOWNOTES
#define INCLUDED_DIALOGSHOWNOTES


#include <gtk/gtk.h>
#include "libraries.h"
#include "notes_utils.h"


class ShowNotesDialog
{
public:
  ShowNotesDialog (int dummy);
  ~ShowNotesDialog ();
  int run();
protected:
  GtkWidget *shownotesdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label3;
  GtkWidget *hbox6;
  GtkWidget *label5;
  GtkWidget *radiobutton_reference_verse;
  GtkWidget *radiobutton_reference_chapter;
  GtkWidget *radiobutton_reference_book;
  GtkWidget *radiobutton_reference_any;
  GtkWidget *hbox7;
  GtkWidget *label6;
  GtkWidget *radiobutton_date_today;
  GtkWidget *hbox8;
  GtkWidget *radiobutton_date_range;
  GtkWidget *button_date_from;
  GtkWidget *label4;
  GtkWidget *button_date_to;
  GtkWidget *radiobutton_date_any;
  GtkWidget *hbox9;
  GtkWidget *label7;
  GtkWidget *combobox_category;
  GtkWidget *hbox10;
  GtkWidget *label8;
  GtkWidget *radiobutton_project_current;
  GtkWidget *radiobutton_project_any;
  GtkWidget *hseparator1;
  GtkWidget *label9;
  GtkWidget *hbox11;
  GtkWidget *label10;
  GtkWidget *radiobutton_order_sort;
  GtkWidget *radiobutton_center;
  GtkWidget *hbox12;
  GtkWidget *label11;
  GtkWidget *checkbutton_showproject;
  GtkWidget *checkbutton_show_category;
  GtkWidget *checkbutton_date_created;
  GtkWidget *checkbutton_show_created_by;
  GtkWidget *hbox13;
  GtkWidget *label12;
  GtkWidget *radiobutton_text_full;
  GtkWidget *radiobutton_text_summary;
  GtkWidget *checkbutton_show_versetext;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_fromdatebutton_clicked (GtkButton *button, gpointer user_data);
  static void on_todatebutton_clicked (GtkButton *button, gpointer user_data);
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_from_date ();
  void on_to_date ();
  void on_ok ();
  void set_gui();
  guint32 from_day;
  guint32 to_day;
  ustring all_categories ();
  GtkToggleButton * reference_get_button (int selector);
  GtkToggleButton * edited_get_button (int selector);
};


#endif
