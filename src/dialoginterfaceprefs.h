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


#ifndef INCLUDED_DIALOGINTERFACE_PREFS_H
#define INCLUDED_DIALOGINTERFACE_PREFS_H


#include <gtk/gtk.h>
#include "libraries.h"


class InterfacePreferencesDialog
{
public:
  InterfacePreferencesDialog (int dummy);
  ~InterfacePreferencesDialog ();
  int run();
protected:
  GtkWidget *interfaceprefsdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *checkbutton_xref_ref;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
private:
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_ok ();
};


#endif
