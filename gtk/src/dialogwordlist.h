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


#ifndef INCLUDED_DIALOGWORDLIST_H
#define INCLUDED_DIALOGWORDLIST_H


#include <gtk/gtk.h>
#include "ustring.h"


class WordlistDialog
{
public:
  WordlistDialog (int dummy);
  ~WordlistDialog ();
  int run ();
protected:
  GtkWidget *wordlistdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *hseparator1;
  GtkWidget *vbox2;
  GtkWidget *checkbutton_wordlist;
  GtkWidget *label3;
  GtkWidget *hbox1;
  GtkWidget *checkbutton_asterisk_general;
  GtkWidget *checkbutton_general_first_time;
  GtkWidget *hseparator3;
  GtkWidget *vbox3;
  GtkWidget *checkbutton_hebrew_wordlist;
  GtkWidget *label4;
  GtkWidget *hbox2;
  GtkWidget *checkbutton_asterisk_hebrew;
  GtkWidget *checkbutton_hebrew_firsttime;
  GtkWidget *hseparator4;
  GtkWidget *vbox4;
  GtkWidget *checkbutton_greek_wordlist;
  GtkWidget *label5;
  GtkWidget *hbox3;
  GtkWidget *checkbutton_asterisk_greek;
  GtkWidget *checkbutton_greek_first_time;
  GtkWidget *hseparator6;
  GtkWidget *vbox6;
  GtkWidget *checkbutton_index;
  GtkWidget *label6;
  GtkWidget *hbox6;
  GtkWidget *checkbutton_asterisk_index;
  GtkWidget *checkbutton_index_first_time;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_checkbutton_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  void gui();
};


#endif
