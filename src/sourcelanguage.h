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


#ifndef INCLUDED_SOURCE_LANGUAGE_H
#define INCLUDED_SOURCE_LANGUAGE_H


#include "libraries.h"
#include <glib.h>
#include <sqlite3.h>
#include "reference.h"


ustring source_language_database_file_name (const ustring& name);
void source_language_database_create (const ustring& name);
vector <ustring> source_language_get_names ();
void source_language_add_to_display ();
void source_language_remove_from_display ();
void source_language_get_lemmata_and_morphology (const ustring& name, const Reference& reference, vector <ustring>& words,
                                                 vector <unsigned int>& lemmata_positions, vector <ustring>& lemmata_values, 
                                                 vector <unsigned int>& morphology_positions, vector <ustring>& morphology_values);


#endif
