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
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;
  GtkWidget *radiobutton_current_verse;
  GtkWidget *radiobutton_current_chapter;
  GtkWidget *radiobutton_current_book;
  GtkWidget *radiobutton_any_verse;
  GtkWidget *radiobutton_today;
  GtkWidget *radiobutton_between;
  GtkWidget *button_start;
  GtkWidget *button_end;
  GtkWidget *radiobutton_at_any_time;
  GtkWidget *label_category;
  GtkWidget *hbox_category;
  GtkWidget *combobox_category;
  GtkWidget *radiobutton_current_project;
  GtkWidget *radiobutton_any_project;
  GtkWidget *checkbutton_title;
  GtkWidget *checkbutton_project;
  GtkWidget *checkbutton_category;
  GtkWidget *checkbutton_date_created;
  GtkWidget *checkbutton_created_by;
  GtkWidget *radiobutton_full;
  GtkWidget *radiobutton_summary;
  GtkWidget *checkbutton_add_ref_text;
  GtkWidget *label_result;
 
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_fromdatebutton_clicked (GtkButton *button, gpointer user_data);
  static void on_todatebutton_clicked (GtkButton *button, gpointer user_data);
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_from_date ();
  void on_to_date ();
  void on_ok ();
  void set_gui();
  guint32 from_day;
  guint32 to_day;
  ustring all_categories ();
  GtkToggleButton * reference_get_button (int selector);
  GtkToggleButton * edited_get_button (int selector);
  static void on_checkbutton_show_title_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  NotesSelectionReferenceType get_reference_selection ();
  NotesSelectionEditedType get_edited_selection ();

  // Gui update if a setting was changed.
  static void on_button_clicked (GtkButton *button, gpointer user_data);
  static void on_combobox_changed (GtkComboBox *combobox, gpointer user_data);
  void on_checkbutton_show_title();
  void restart_timeout ();
  guint event_id;
  static bool on_timeout(gpointer user_data);
  void timeout();
};


#endif
