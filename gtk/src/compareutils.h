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


#ifndef INCLUDED_COMPAREUTILS_H
#define INCLUDED_COMPAREUTILS_H


#include "libraries.h"
#include "progresswindow.h"
#include "project_memory.h"
#include "windowreferences.h"


void compare_with (WindowReferences * references_window, const ustring& project, const ustring& secondproject, bool print_changes_only);
bool compare_projects (ProjectMemory& originalproject, ProjectMemory& secondproject, ProjectMemory& outputproject);
void compare_get_changes (ProjectMemory& project, vector <Reference>& changed_references);
void compare_get_additions_deletions (ProjectMemory& originalproject, ProjectMemory& secondproject, vector<ustring>& results);
void compare_usfm_text (vector<ustring>& original, vector<ustring>& edit, vector<ustring>& output, bool mark_new_line);


#endif
