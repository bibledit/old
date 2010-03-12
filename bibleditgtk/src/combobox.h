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


#ifndef INCLUDED_COMBOBOX_H
#define INCLUDED_COMBOBOX_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


ustring combobox_get_active_string (GtkWidget * combobox);
unsigned int combobox_get_active_index (GtkWidget * combobox);
unsigned int combobox_get_string_count (GtkWidget * combobox);
void combobox_clear_strings (GtkWidget * combobox);
vector<ustring> combobox_get_strings (GtkWidget * combobox);
void combobox_set_strings (GtkWidget * combobox, const vector<ustring>& strings);
void combobox_set_strings (GtkWidget * combobox, const vector<unsigned int>& strings);
void combobox_set_string (GtkWidget * combobox, const ustring& string);
void combobox_set_string (GtkWidget * combobox, unsigned int string);
void combobox_set_index (GtkWidget * combobox, unsigned int index);


#endif
