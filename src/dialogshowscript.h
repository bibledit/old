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


#ifndef INCLUDED_DIALOGSHOWSCRIPT
#define INCLUDED_DIALOGSHOWSCRIPT


#include <gtk/gtk.h>
#include "libraries.h"


class ShowScriptDialog
{
public:
  ShowScriptDialog (int dummy);
  ~ShowScriptDialog ();
  int run();
protected:
  GtkWidget *showscriptdialog;
  GtkWidget *dialog_vbox;
  GtkWidget *vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *textview1;
  GtkWidget *checkbutton1;
  GtkWidget *hbox1;
  GtkWidget *radiobutton_bibledit;
  GtkWidget *radiobutton_bibletime;
  GtkWidget *radiobutton_git;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
private:
  guint event_source_id;
  static bool show_script_dialog_load (gpointer data);
  bool load ();
  ustring logfilename ();
  static void on_checkbutton1_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_bibledit_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_bibletime_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  static void on_radiobutton_git_toggled (GtkToggleButton *togglebutton, gpointer user_data);
};


#endif
