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


#ifndef INCLUDED_DIALOG_XETEX_H
#define INCLUDED_DIALOG_XETEX_H


#include <gtk/gtk.h>
#include "ustring.h"


class XeTeXDialog
{
public:
  XeTeXDialog (int dummy);
  ~XeTeXDialog ();
  int run ();
protected:
  GtkBuilder *gtkbuilder;
  GtkWidget *dialog;
  GtkWidget *label_portion;
  
  GtkWidget *button_portion;
  static void on_button_portion_clicked (GtkButton *button, gpointer user_data);
  void on_button_portion ();

  GtkWidget *expander;
  GtkWidget *label_expander;

  GtkWidget *notebook;
  static void on_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
  void on_notebook (guint page_num);

  GtkWidget *label_tab_notes;
  GtkWidget *checkbutton_full_references;

  GtkWidget *label_tab_page;
  GtkWidget *checkbutton_cropmarks;

  GtkWidget *label_tab_mapping;
  GtkWidget *button_font_mapping_clear;
  GtkWidget *filechooserbutton_font_mapping_file;
  static void on_button_font_mapping_clear_clicked (GtkButton *button, gpointer user_data);
  
  GtkWidget *label_tab_engine;
  GtkWidget *radiobutton_shaping_engine_generic;
  GtkWidget *radiobutton_shaping_engine_arab;
  void shaping_engine_set (int type);
  int shaping_engine_get ();
  
  GtkWidget *cancelbutton;
  GtkWidget *okbutton;
  static void on_okbutton_clicked (GtkButton *button, gpointer user_data);
  void on_okbutton ();

  void set_gui ();
};


#endif
