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


#ifndef INCLUDED_RESOURCEVIEWER_GUI_H
#define INCLUDED_RESOURCEVIEWER_GUI_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include <gtkmozembed.h>
#include "resource.h"


class ResourcesGUI
{
public:
  ResourcesGUI (GtkWidget * notebook_vbox);
  ~ResourcesGUI ();
  void open (const ustring& filename, int method);
  void close ();
  Resource * focused_resource ();
  void go_to (const Reference& reference);
  void reload (const ustring& oldfilename, const ustring& newfilename);
private:
  // Main window.
  GtkWidget *my_notebook_vbox;

  // Tool bar.
  GtkWidget *toolbar;
  GtkWidget *toolitem4;
  GtkWidget *button_back;
  GtkWidget *image4;
  GtkWidget *toolitem5;
  GtkWidget *button_forward;
  GtkWidget *image5;
  GtkWidget *toolitem1;
  GtkWidget *button_refresh;
  GtkWidget *image1;
  GtkWidget *toolitem2;
  GtkWidget *button_stop;
  GtkWidget *image2;
  GtkWidget *toolitem3;
  GtkWidget *button_home;
  GtkWidget *image3;

  // Browser.
  GtkWidget *notebook;
  vector <Resource *> resource_objects;

  // Tool callbacks.
  static void on_button_back_clicked (GtkButton *button, gpointer user_data);
  void on_button_back ();
  static void on_button_forward_clicked (GtkButton *button, gpointer user_data);
  void on_button_forward ();
  static void on_button_refresh_clicked (GtkButton *button, gpointer user_data);
  void on_button_refresh ();
  static void on_button_stop_clicked (GtkButton *button, gpointer user_data);
  void on_button_stop ();
  static void on_button_home_clicked (GtkButton *button, gpointer user_data);
  void on_button_home ();
  
  // Reference logic.
  map <unsigned int, ustring> book_renderer;

  // Gui updater.
  static bool on_update_gui_timeout (gpointer user_data);
  bool on_update_gui ();
  
  // Focus handling.
  static void on_focus_signal_clicked (GtkButton *button, gpointer user_data);
  void on_focus_signal (GtkButton *button);
  static void on_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
  void notebook_switch_page (guint page_num);
  
  // Desktop layout.
  void desktop_save ();
  void desktop_load ();
};


#endif
