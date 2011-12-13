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


#ifndef INCLUDED_DIALOGREPLACING_H
#define INCLUDED_DIALOGREPLACING_H


#include <gtk/gtk.h>
#include "usfm.h"
#include "reference.h"


class ReplacingDialog
{
public:
  ReplacingDialog (const vector <Reference>& references_in);
  ~ ReplacingDialog ();
  int run ();
protected:
private:
  GtkWidget *replacedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *referencelabel;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *scrolledwindow4;
  GtkWidget *textview1;
  GtkWidget *label2;
  GtkWidget *scrolledwindow5;
  GtkWidget *textview2;
  GtkWidget *hbox3;
  GtkWidget *yesbutton;
  GtkWidget *nobutton;
  GtkWidget *allbutton;
  GtkWidget *alignment1;
  GtkWidget *hbox4;
  GtkWidget *image1;
  GtkWidget *label3;
  GtkWidget *cancelbutton;
  GtkWidget *dialog_action_area1;
  vector <Reference> references;
  unsigned int referencepointer;
  ustring myproject;
  ustring mylanguage;
  bool mycasesensitive;
  GtkTextTag *tag1;
  GtkTextTag *tag2;
  GtkTextBuffer *textbuffer1;
  GtkTextBuffer *textbuffer2;
  void set_gui ();
  void accept_change ();
  ustring searchfor;
  ustring replacewith;
  static void replacedialog_on_cancelbutton_clicked (GtkButton * button, gpointer user_data);
  void on_cancelbutton_clicked ();
  static void replacedialog_on_yesbutton_clicked (GtkButton * button, gpointer user_data);
  void on_yesbutton_clicked ();
  static void replacedialog_on_nobutton_clicked (GtkButton * button, gpointer user_data);
  void on_nobutton_clicked ();
  static void replacedialog_on_allbutton_clicked (GtkButton * button, gpointer user_data);
  void on_allbutton_clicked ();
};


#endif
