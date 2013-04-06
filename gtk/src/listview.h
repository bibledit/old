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


#ifndef INCLUDED_LISTVIEW_H
#define INCLUDED_LISTVIEW_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


ustring listview_get_active_string(GtkWidget * listview);
int listview_get_active_offset(GtkWidget * listview);
vector <int> listview_get_active_offsets(GtkWidget * listview);
vector <ustring> listview_get_active_strings(GtkWidget * listview);
void listview_clear_strings(GtkWidget * listview, GtkListStore * store);
void listview_set_strings(GtkWidget * listview, GtkListStore * store, const vector<ustring>& strings);
void listview_set_strings(GtkWidget * listview, GtkListStore * store, const vector<unsigned int>& strings);
void listview_focus_string(GtkWidget * listview, const ustring& string, bool grabfocus = true);
void listview_focus_string(GtkWidget * listview, unsigned int string, bool grabfocus = true);
vector <ustring> listview_get_strings(GtkWidget * listview);
void list_view_erase_selection(GtkWidget * listview);
void listview_set_row(GtkWidget * listview, GtkListStore * store, unsigned int offset, const ustring row);


#endif
