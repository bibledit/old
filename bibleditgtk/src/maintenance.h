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


#ifndef INCLUDED_MAINTENANCE_H
#define INCLUDED_MAINTENANCE_H


#include "libraries.h"


void maintenance_initialize ();
void maintenance_register_shell_command (const ustring& working_directory, const ustring& shell_command);
void maintenance_register_git_repository (const ustring& directory);
void maintenance_register_database (const ustring& filename);
void maintenance_register_snapshot (const ustring& filename);
void shutdown_actions ();


#endif
