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


#ifndef INCLUDED_PLANNING_H
#define INCLUDED_PLANNING_H


#include "libraries.h"
#include <glib.h>
#include "htmlwriter.h"


void planning_edit (const ustring& project);
ustring planning_assemble_task (unsigned int book, const ustring& task);
void planning_disassemble_task (ustring assembled_task, unsigned int& book, ustring& task);
void planning_produce_report (const ustring& project, HtmlWriter & htmlwriter);


#endif
