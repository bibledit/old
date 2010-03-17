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


#ifndef INCLUDED_UNIXWRAPPERS_H
#define INCLUDED_UNIXWRAPPERS_H


#include "libraries.h"
#include <glib.h>


bool unix_fnmatch (const char * pattern, const ustring& text);
void unix_cp (const ustring& from, const ustring& to);
void unix_cp_r (const ustring& from, const ustring& to);
void unix_mv (const ustring& from, const ustring& to);
void unix_rmdir (const ustring& dir);
void unix_kill (GPid pid);


#endif
