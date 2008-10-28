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


#ifndef INCLUDED_DIALOGREFEXCHANGE_H
#define INCLUDED_DIALOGREFEXCHANGE_H


#include <gtk/gtk.h>
#include "ustring.h"


class ReferenceExchangeDialog
{
public:
  ReferenceExchangeDialog (int dummy);
  ~ReferenceExchangeDialog ();
  int run ();
protected:
  GtkWidget *dialogsynchronize;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label6;
  GtkWidget *entry_bw_exe;
  GtkWidget *hseparator3;
  GtkWidget *label2;
  GtkWidget *vbox2;
  GtkWidget *checkbutton_send_bw;
  GtkWidget *checkbutton_send_bt;
  GtkWidget *checkbutton_send_paratext;
  GtkWidget *hseparator1;
  GtkWidget *label4;
  GtkWidget *vbox3;
  GtkWidget *radiobutton_receive_none;
  GtkWidget *radiobutton_receive_bw;
  GtkWidget *radiobutton_receive_bt;
  GtkWidget *radiobutton_receive_paratext;
  GtkWidget *hseparator_warning_outpost;
  GtkWidget *hbox1;
  GtkWidget *image_warning_outpost;
  GtkWidget *label_warning_outpost;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_checkbutton_send_bw_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_receive_bw_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_bibleworks ();
  static void on_checkbutton_send_paratext_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_receive_paratext_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_outpost ();
};


#endif
