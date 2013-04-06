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


#ifndef INCLUDED_RESTORE_H
#define INCLUDED_RESTORE_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


void restore_project (const ustring& unpack_directory, const ustring& bible, vector <ustring>& feedback);
void restore_notes (const ustring& unpack_directory, vector <ustring>& feedback);
void restore_resource (const ustring& unpack_directory, vector <ustring>& feedback);
void restore_all_stage_one (const ustring& unpack_directory, vector <ustring>& feedback);
void restore_all_stage_two ();
bool restore_file_present (const ustring& directory, const gchar * filename, vector <ustring>& feedback);


#endif
