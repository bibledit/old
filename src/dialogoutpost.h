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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_DIALOGOUTPOST_H
#define INCLUDED_DIALOGOUTPOST_H


#include <gtk/gtk.h>
#include "types.h"


class OutpostDialog
{
public:
  OutpostDialog (int dummy);
  ~OutpostDialog ();
  int run ();
  bool changed;
protected:
private:
  GtkWidget *outpostdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *vbox2;
  GtkWidget *checkbutton_use;
  GtkWidget *hbox17;
  GtkWidget *checkbutton_host;
  GtkWidget *entry_host;
  GtkWidget *hseparator1;
  GtkWidget *image_outpost_ok;
  GtkWidget *label_outpost_ok;
  GtkWidget *label_outpost_question;
  GtkWidget *button_outpost_select;
  GtkWidget *alignment8;
  GtkWidget *hbox15;
  GtkWidget *image19;
  GtkWidget *label46;
  GtkWidget *button_outpost_open;
  GtkWidget *alignment5;
  GtkWidget *hbox12;
  GtkWidget *image15;
  GtkWidget *label39;
  GtkWidget *button_outpost_find;
  GtkWidget *alignment6;
  GtkWidget *hbox13;
  GtkWidget *image16;
  GtkWidget *label40;
  GtkWidget *label_outpost_info;
  GtkWidget *hseparator2;
  GtkWidget *image_wine_ok;
  GtkWidget *label_wine_ok;
  GtkWidget *label_wine_question;
  GtkWidget *button_wine_select;
  GtkWidget *alignment9;
  GtkWidget *hbox16;
  GtkWidget *image20;
  GtkWidget *label47;
  GtkWidget *button_wine_open;
  GtkWidget *alignment3;
  GtkWidget *hbox10;
  GtkWidget *image11;
  GtkWidget *label34;
  GtkWidget *button_wine_search;
  GtkWidget *alignment4;
  GtkWidget *hbox11;
  GtkWidget *image12;
  GtkWidget *label35;
  GtkWidget *label_wine_info;
  GtkWidget *hseparator3;
  GtkWidget *image_access_ok;
  GtkWidget *label_access_ok;
  GtkWidget *label_access_question;
  GtkWidget *label_access_info;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  ustring outpost_path;
  set <ustring> outpost_paths;
  ustring wine_path;
  set <ustring> wine_paths;
  ustring outpost_command;
  bool old_use_outpost;
  bool old_outpost_networked;
  ustring old_outpost_host;
  ustring old_outpost_path;
  ustring old_wine_path;
  ustring old_outpost_command;
  guint host_event_id;
  static void on_checkbutton_use_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_use ();
  static void on_checkbutton_host_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_host ();
  static void on_entry_host_changed (GtkEditable *editable, gpointer user_data);
  void on_entry_host ();
  static bool host_delayer (gpointer user_data);
  void host_delayed ();
  static void on_button_outpost_select_clicked (GtkButton *button, gpointer user_data);
  void on_outpost_select ();
  static void on_button_outpost_open_clicked (GtkButton *button, gpointer user_data);
  void on_outpost_open ();
  static void on_button_outpost_find_clicked (GtkButton *button, gpointer user_data);
  void on_outpost_find ();
  static void on_button_wine_select_clicked (GtkButton *button, gpointer user_data);
  void on_wine_select ();  
  static void on_button_wine_open_clicked (GtkButton *button, gpointer user_data);
  void on_wine_open ();
  static void on_button_wine_search_clicked (GtkButton *button, gpointer user_data);
  void on_wine_search ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_ok ();
  void set_gui ();
  bool outpost_path_okay (const ustring& path);
  void find_outpost ();
  bool wine_path_okay (const ustring& path);
  void find_wine ();
  WineType wine_get_type (const ustring& path);
};


#endif
