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


#ifndef INCLUDED_DIALOGFONTCOLOR_H
#define INCLUDED_DIALOGFONTCOLOR_H


#include <gtk/gtk.h>
#include "libraries.h"


class FontColorDialog
{
public:
  FontColorDialog (bool initial_use_default_font, 
                   const ustring& initial_font,
                   unsigned int initial_line_spacing,
                   bool initial_use_default_color,
                   unsigned int initial_normal_text_color, 
                   unsigned int initial_background_color,
                   unsigned int initial_selected_text_color, 
                   unsigned int initial_selection_color);
  ~FontColorDialog ();
  int run ();
  bool new_use_default_font;
  ustring new_font;
  unsigned int new_line_spacing;
  bool new_use_default_color;
  unsigned int new_normal_text_color;
  unsigned int new_background_color;
  unsigned int new_selected_text_color;
  unsigned int new_selection_color;
protected:
  GtkWidget *fontcolordialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *checkbutton_font;
  GtkWidget *fontselection;
  GtkWidget *hbox_line_spacing;
  GtkWidget *label7;
  GtkObject *spinbutton_line_spacing_adj;
  GtkWidget *spinbutton_line_spacing;
  GtkWidget *label8;
  GtkWidget *hseparator1;
  GtkWidget *checkbutton_colors;
  GtkWidget *table1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *colorbutton_normal_text;
  GtkWidget *colorbutton_background;
  GtkWidget *colorbutton_selected_text;
  GtkWidget *colorbutton_selection;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
private:
  static void on_checkbutton_font_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_font ();
  static void on_checkbutton_colors_toggled (GtkToggleButton *togglebutton, gpointer user_data);
  void on_checkbutton_colors ();
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();
};


#endif
