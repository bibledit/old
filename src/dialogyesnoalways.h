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


#ifndef INCLUDED_DIALOG_YES_NO_ALWAYS_H
#define INCLUDED_DIALOG_YES_NO_ALWAYS_H


#include "libraries.h"
#include <gtk/gtk.h>


enum YesNoAlwaysDialogType {ynadtLoadReferences, ynadtDeleteNote};


bool yes_no_always_dialog (const ustring& message, YesNoAlwaysDialogType type, bool default_yes, bool default_always);


class YesNoAlwaysDialog
{
public:
  YesNoAlwaysDialog (const ustring& message, bool default_yes);
  ~YesNoAlwaysDialog ();
  int run ();
protected:
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;
  GtkWidget *label;
public:
  GtkWidget *checkbutton;
protected:
  static gboolean static_on_timeout (gpointer data);
  bool on_timeout ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_checkbutton_compare_all_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_compare_all ();
};


#endif
