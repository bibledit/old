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


#ifndef INCLUDED_DIALOGREFEXCHANGE_H
#define INCLUDED_DIALOGREFEXCHANGE_H


#include <gtk/gtk.h>
#include "ustring.h"


class ReferenceExchangeDialog
{
public:
  ReferenceExchangeDialog (int dummy);
  ~ReferenceExchangeDialog ();
  int run ();
protected:
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;

  GtkWidget *entry_url;
  GtkWidget *button_url;
  GtkWidget *label_url;

  GtkWidget *checkbutton_bibleworks;
  GtkWidget *checkbutton_santafe;
  GtkWidget *checkbutton_onlinebible;

  GtkWidget *radiobutton_off;
  GtkWidget *radiobutton_bibleworks;
  GtkWidget *radiobutton_santafe;
  GtkWidget *radiobutton_onlinebible;

  GtkWidget *vbox_outpost;
  
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_url_test_clicked(GtkButton * button, gpointer user_data);
  void on_url_test();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
  static void on_button_outpost_requirement_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_outpost ();
};


#endif
