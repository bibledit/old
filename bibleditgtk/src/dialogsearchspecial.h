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


#ifndef INCLUDED_DIALOGSEARCHSPECIAL_H
#define INCLUDED_DIALOGSEARCHSPECIAL_H


#include <gtk/gtk.h>
#include "screen.h"


class SearchSpecialDialog
{
public:
  SearchSpecialDialog(int dummy);
  ~ SearchSpecialDialog();
  int run();
protected:
  GtkWidget *searchspecialdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox3;
  GtkWidget *notebook_mode;
  GtkWidget *label17;
  GtkWidget *label19;
  GtkWidget *label22;
  GtkWidget *label20;
  GtkWidget *hbox1;
  GtkWidget *label_word_entry;
  GtkWidget *word_entry;
  GtkWidget *notebook_case_current;
  GtkWidget *vbox6;
  GtkWidget *checkbutton_casesensitive;
  GtkWidget *checkbutton_current_book;
  GtkWidget *checkbutton_current_chapter;
  GtkWidget *label27;
  GtkWidget *empty_notebook_page;
  GtkWidget *label28;
  GtkWidget *label29;
  GtkWidget *label30;
  GtkWidget *notebook_advanced;
  GtkWidget *vbox5;
  GtkWidget *checkbutton_glob;
  GtkWidget *checkbutton_start_word;
  GtkWidget *checkbutton_end_word;
  GtkWidget *table2;
  GtkWidget *button_books;
  GtkWidget *alignment1;
  GtkWidget *hbox3;
  GtkWidget *image1;
  GtkWidget *label13;
  GtkWidget *button_area;
  GtkWidget *alignment2;
  GtkWidget *hbox5;
  GtkWidget *image2;
  GtkWidget *label39;
  GtkWidget *label_book;
  GtkWidget *label_area;
  GtkWidget *label31;
  GtkWidget *label32;
  GtkWidget *notebook_results;
  GtkWidget *vbox7;
  GtkWidget *label14;
  GtkWidget *combobox_results;
  GtkWidget *label35;
  GtkWidget *label36;
  GtkWidget *label37;
  GtkWidget *label38;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *findbutton;
  GtkAccelGroup *accel_group;
private:
  static void on_word_entry_activate(GtkEntry *entry, gpointer user_data);
  void on_activate();
  static bool on_timeout(gpointer data);
  void timeout();
  bool allow_activate;
  static void on_word_entry_changed(GtkEditable * editable, gpointer user_data);
  void set_gui();
  static void on_findbutton_clicked(GtkButton * button, gpointer user_data);
  void on_find();
  static void on_button_books_clicked(GtkButton *button, gpointer user_data);
  void on_books();
  static void on_checkbutton_current_book_toggled(GtkToggleButton *togglebutton, gpointer user_data);
  static void on_button_area_clicked(GtkButton *button, gpointer user_data);
  void on_button_area();
  static void on_searchspecialdialog_activate_focus(GtkWindow *window, gpointer user_data);
  void on_control_f();
  static void on_notebook_mode_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
  void on_mode_change(guint page_num);
  DialogPresenter * dialogpresenter;
};


#endif
