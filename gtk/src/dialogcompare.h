/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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

#ifndef INCLUDED_DIALOGCOMPARE_H
#define INCLUDED_DIALOGCOMPARE_H

#include <gtk/gtk.h>
#include "ustring.h"
#include "guiselectproject.h"
#include "windowreferences.h"


class CompareDialog
{
public:
  CompareDialog(WindowReferences * references_window);
  ~CompareDialog();
  int run();
protected:
  GtkWidget *comparedialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  SelectProjectGui * selectprojectgui;
  GtkWidget *checkbutton1;
  GtkWidget *hbox1;
  GtkWidget *label2;
  GtkWidget *label_portion;
  GtkWidget *vseparator1;
  GtkWidget *button_change;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static bool on_combobox1_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
  bool on_combobox1_key_press(GdkEventKey *event);
  static void static_on_okbutton_clicked(GtkButton * button, gpointer user_data);
  void on_okbutton_clicked();
  static void on_button_portion_clicked(GtkButton *button, gpointer user_data);
  void on_button_portion();
  void gui_reorder_include();
  WindowReferences * my_references_window;
};

#endif
