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


#ifndef INCLUDED_RESOURCE_H
#define INCLUDED_RESOURCE_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "types.h"
#include "webkitbrowser.h"


class Resource
{
public:
  Resource(GtkWidget * window);
  ~Resource();
  void focus();
  bool focused();
  void copy();
  void go_to(const Reference& reference);
  ustring template_get();
  void open(const ustring& filename);
  time_t last_focused_time();
private:
  GtkWidget *vbox;
public:
  WebkitBrowser * browser;
private:
  ustring mytemplatefile;
  ustring url_structure;
  map <unsigned int, ustring> book_renderer;
  map <unsigned int, ustring> book_renderer2;
  Reference myreference;
  static void on_button_home_clicked (GtkButton *button, gpointer user_data);
  void on_button_home ();
  ustring homepage;
};

#endif
