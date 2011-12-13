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


#ifndef INCLUDED_SHORTCUTS_H
#define INCLUDED_SHORTCUTS_H


#include "libraries.h"
#include <gtk/gtk.h>


class Shortcuts
{
public:
  Shortcuts (int dummy);
  ~Shortcuts ();
  void stockbutton (GtkWidget * widget);
  void button (GtkWidget * widget);
  void label (GtkWidget * widget);
  void process ();
  GtkWidget * lastwidget;
  void consider_assistant ();
private:
  vector <GtkWidget *> widgets;
  vector <bool> is_button;
  set <ustring> unavailables;
  vector <GtkWidget *> created_widgets;
  void create_widget (const gchar * stock);
};


#endif
