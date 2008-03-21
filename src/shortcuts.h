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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
  void stockbutton (GtkWidget * widget);
  void button (GtkWidget * widget);
  void label (GtkWidget * widget);
  void process ();
  GtkWidget * lastwidget;
private:
  vector <GtkWidget *> widgets;
  vector <bool> is_button;
  set <ustring> unavailables;
};


#endif
