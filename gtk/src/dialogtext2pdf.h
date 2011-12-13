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

#ifndef INCLUDED_DIALOGTEXT2PDF_H
#define INCLUDED_DIALOGTEXT2PDF_H

#include <gtk/gtk.h>
#include "ustring.h"

class Text2PdfDialog
{
public:
  Text2PdfDialog(vector <ustring> * commands);
  ~Text2PdfDialog();
  int run();
  ustring entered_value;
  bool always_ok;
protected:
  GtkWidget *text2pdfdialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  vector <ustring> * mycommands;
  static void on_okbutton_clicked(GtkButton *button, gpointer user_data);
  void on_okbutton();
  static gboolean on_text2pdfdialog_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data);
  void on_text2pdfdialog_delete();
};

#endif
