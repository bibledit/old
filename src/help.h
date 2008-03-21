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


#ifndef INCLUDED_HELP_H
#define INCLUDED_HELP_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "shortcuts.h"


void help_open (GtkButton * button, gpointer user_data);


class InDialogHelp
{
public:
  InDialogHelp (GtkWidget * dialog, Shortcuts * shortcuts, gchar * topic);
  ~InDialogHelp ();
private:
  GtkWidget *helpbutton;
  GtkWidget *alignment;
  GtkWidget *hbox;
  GtkWidget *image;
  GtkWidget *label;
  GtkWidget *scrolledwindow;
  GtkWidget *htmlview;
  gchar * mytopic;
  GtkWidget * mydialog;
  GPid process_id;
  static void on_helpbutton_activated (GtkButton * button, gpointer user_data);
  void on_helpbutton ();
  static gboolean on_dialog_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data);
};


#endif
