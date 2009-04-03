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


#ifndef INCLUDED_DIALOGBACKUP_H
#define INCLUDED_DIALOGBACKUP_H


#include <gtk/gtk.h>
#include "ustring.h"


class BackupDialog
{
public:
  BackupDialog (int dummy);
  ~BackupDialog ();
  int run ();
protected:
  GtkWidget *backupdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label_info;
  GtkWidget *hbox1;
  GtkWidget *radiobutton_incremental;
  GtkWidget *button_since;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label2;
  GtkWidget *radiobutton_full;
  GtkWidget *label_comment;
  GtkWidget *entry_comment;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_radiobutton_incremental_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void gui ();
  static void on_sincebutton_clicked (GtkButton *button, gpointer user_data);
  void on_sincebutton ();
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
};


#endif
