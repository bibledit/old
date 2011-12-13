/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_DIALOG_FIX_MARKERS_H
#define INCLUDED_DIALOG_FIX_MARKERS_H


#include <gtk/gtk.h>
#include "ustring.h"


class FixMarkersDialog
{
public:
  FixMarkersDialog (int dummy);
  ~FixMarkersDialog ();
  int run ();
protected:
  GtkWidget *fixmarkersdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *label1;
  GtkWidget *hseparator1;
  GtkWidget *checkbutton_space_after_opener;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
};


#endif
