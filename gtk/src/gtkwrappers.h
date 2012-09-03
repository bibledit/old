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


#ifndef INCLUDED_GTKWRAPPERS_H
#define INCLUDED_GTKWRAPPERS_H


#include "libraries.h"
#include <gtk/gtk.h>


void gtkw_dialog_info (GtkWidget * parent, const ustring& info);
int gtkw_dialog_question (GtkWidget * parent, const ustring& question, int default_response = GTK_RESPONSE_NO);
void gtkw_dialog_error (GtkWidget * parent, const ustring& error);
void gtkw_dialog_warning (GtkWidget * parent, const ustring& warning);
ustring gtkw_file_chooser_select_folder (GtkWidget * parent, const ustring& title, const ustring& directory);
ustring gtkw_file_chooser_open (GtkWidget * parent, const ustring& title, const ustring& file);
vector <ustring> gtkw_file_chooser_open_multiple (GtkWidget * parent, const ustring& title, ustring directory);
ustring gtkw_file_chooser_save (GtkWidget * parent, const ustring& title, const ustring& file);
void gtkw_show_uri (ustring uri, bool internet = false);


#endif
