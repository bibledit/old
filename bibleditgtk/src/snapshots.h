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


#ifndef INCLUDED_SNAPSHOTS_H
#define INCLUDED_SNAPSHOTS_H


#include "libraries.h"


ustring old_snapshots_content_database (const ustring& project);
ustring snapshots_directory (const ustring& project);
ustring snapshots_database (const ustring& project, unsigned int book, unsigned int chapter);
void snapshots_initialize_all ();
void snapshots_initialize_project (const ustring& project);
void snapshots_shoot_project (const ustring& project);
void snapshots_shoot_chapter (const ustring& project, unsigned int book, unsigned int chapter, unsigned int seconds, bool persistent);
void snapshots_shoot_chapter (const ustring& project, unsigned int book, unsigned int chapter, const ustring& content, unsigned int seconds, bool persistent);
vector <unsigned int> snapshots_get_seconds (const ustring& project, unsigned int book, unsigned int chapter);
ustring snapshots_get_chapter (const ustring& project, unsigned int book, unsigned int chapter, unsigned int seconds);
unsigned int snapshots_oldest_second (const ustring& project);
void snapshots_get_chapters_changed_since(const ustring & project, unsigned int second, vector <unsigned int>& books, vector <unsigned int>& chapters);


#endif
