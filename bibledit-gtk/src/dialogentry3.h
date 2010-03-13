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

#ifndef INCLUDED_DIALOGENTRY3_H
#define INCLUDED_DIALOGENTRY3_H


#include <gtk/gtk.h>
#include "ustring.h"


class Entry3Dialog
{
public:
  Entry3Dialog (const string& title, bool allow_empty_values, 
                const string& info1, const ustring& value1,
                const string& info2, const ustring& value2,
                const string& info3, const ustring& value3);
  virtual ~Entry3Dialog ();
  virtual int run ();
  ustring entered_value1;
  ustring entered_value2;
  ustring entered_value3;
protected:
  GtkWidget *entry3dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *entry1;
  GtkWidget *label2;
  GtkWidget *entry2;
  GtkWidget *label3;
  GtkWidget *entry3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  virtual void on_okbutton ();
  static void on_entry_changed (GtkEditable *editable, gpointer user_data);
  virtual void on_entry ();
  bool my_allow_empty_values;
};


#endif
