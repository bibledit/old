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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/

#ifndef INCLUDED_DIALOGEXPORTNOTES_H
#define INCLUDED_DIALOGEXPORTNOTES_H


#include <gtk/gtk.h>
#include "export_utils.h"


class ExportNotesDialog
{
public:
  ExportNotesDialog (int dummy);
  ~ExportNotesDialog ();
  int run ();
  ustring filename;
  ExportNotesFormat exportnotesformat;
  bool save_all_notes;
protected:
  GtkWidget *exportnotesdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *filechooserwidget1;
  GtkWidget *hseparator1;
  GtkWidget *label1;
  GtkWidget *radiobutton1;
  GtkWidget *radiobutton2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_okbutton1_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
};


#endif
