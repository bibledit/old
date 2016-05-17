/*
 ** Copyright (©) 2003 The Free Software Foundation.
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

#ifndef INCLUDED_DIALOGPRINTPREFS_H
#define INCLUDED_DIALOGPRINTPREFS_H

#include <gtk/gtk.h>

class PrintPreferencesDialog
{
public:
  PrintPreferencesDialog(int dummy);
  ~PrintPreferencesDialog();
  int run();
protected:
private:
  GtkWidget *dialogprintpreferences;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;

  // Page setup.
  GtkWidget *hbox1;
  GtkWidget *table1;
  GtkObject *spinbutton_height_adj;
  GtkWidget *spinbutton_height;
  GtkWidget *label4;
  GtkObject *spinbutton_width_adj;
  GtkWidget *spinbutton_width;
  GtkWidget *label3;
  GtkWidget *label2;
  GtkWidget *combo_format;
  GtkWidget *combo_entry_format;
  GtkWidget *label5;
  GtkWidget *vseparator1;
  GtkWidget *table2;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkObject *spinbutton_left_adj;
  GtkWidget *spinbutton_left;
  GtkObject *spinbutton_right_adj;
  GtkWidget *spinbutton_right;
  GtkObject *spinbutton_top_adj;
  GtkWidget *spinbutton_top;
  GtkObject *spinbutton_bottom_adj;
  GtkWidget *spinbutton_bottom;
  GtkObject *spinbutton_columngap_adj;
  GtkWidget *spinbutton_columngap;
  GtkWidget *vseparator2;
  GtkWidget *vbox1;
  GtkWidget *checkbuttondate;
  GtkWidget *draftchkbox;
  GtkWidget *hbox2;
  GtkWidget *label12;
  GtkObject *spinbutton_header_font_size_adj;
  GtkWidget *spinbutton_header_font_size;
  GtkWidget *label13;
  GtkWidget *label1;

  // Formatter setup.
  GtkWidget *vbox2;
  GtkWidget *checkbutton_intermediate_text;
  GtkWidget *label11;

  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  static void on_spinbutton_height_changed(GtkEditable *editable, gpointer user_data);
  void on_spinbutton_height();
  static void on_spinbutton_width_changed(GtkEditable *editable, gpointer user_data);
  void on_spinbutton_width();
  static void on_combo_entry_format_changed(GtkEditable *editable, gpointer user_data);
  void on_combo_entry_format();
  static void on_okbutton1_clicked(GtkButton *button, gpointer user_data);
  void on_okbutton1();
  void adjust_margin_limits();
  void set_paper_format();
};

#endif
