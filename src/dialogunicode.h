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

#ifndef INCLUDED_DIALOGUNICODE_H
#define INCLUDED_DIALOGUNICODE_H


#include <gtk/gtk.h>


class UnicodeDialog
{
public:
  UnicodeDialog (const ustring & filetoconvert);
  ~ UnicodeDialog ();
  int run ();
  ustring encoding;
protected:
  GtkWidget *unicodedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *vbox1;
  GtkWidget *button1;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *okbutton1;
private:
  ustring filename;
  vector <ustring> encodings;
  void set_gui ();
  static void on_button1_clicked (GtkButton *button, gpointer user_data);
  void on_button1 ();
};


#endif
