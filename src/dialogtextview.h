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


#ifndef INCLUDED_DIALOGTEXTVIEW_H
#define INCLUDED_DIALOGTEXTVIEW_H


#include <gtk/gtk.h>
#include "ustring.h"


class TextViewDialog
{
public:
  TextViewDialog (const ustring& title, const ustring& info, bool editable, gchar * contents);
  ~TextViewDialog ();
  int run ();
  ustring entered_value;
protected:
  GtkWidget *textviewdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  GtkTextBuffer * buffer;
};


#endif
