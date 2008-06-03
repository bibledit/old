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

  // Browser.
  GtkWidget *notebook;
  vector <Resource *> resource_objects;

  // Reference logic.
  map <unsigned int, ustring> book_renderer;

  // Focus handling.
  static void on_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
  void notebook_switch_page (guint page_num);
  
  // Desktop layout.
  void desktop_save ();
  void desktop_load ();
};


#endif
