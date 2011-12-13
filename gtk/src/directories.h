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


#ifndef INCLUDED_DIRECTORIES_H
#define INCLUDED_DIRECTORIES_H


#include "libraries.h"


void directories_check_structure ();
ustring directories_get_root ();
ustring directories_get_projects ();
ustring directories_get_notes ();
ustring directories_get_stylesheets ();
ustring directories_get_configuration ();
ustring directories_get_pictures ();
ustring directories_get_resources ();
ustring directories_get_scripts ();
ustring directories_get_temp ();
ustring directories_get_templates ();
ustring directories_get_templates_user ();
ustring directories_get_package_data ();
ustring directories_get_restore ();


#endif
