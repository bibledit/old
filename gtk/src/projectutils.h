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


#ifndef INCLUDED_PROJECTUTILS_H
#define INCLUDED_PROJECTUTILS_H


#include "libraries.h"
#include "categorize.h"
#include "sqlite_reader.h"


void projects_initial_check ();
vector<ustring> projects_get_all ();
ustring project_data_directory_project (const ustring& project);
ustring project_data_directory_book (const ustring& project, unsigned int book);
ustring project_data_directory_chapter (const ustring& project, unsigned int book, unsigned int chapter);
ustring project_data_filename_chapter (const ustring& project, unsigned int book, unsigned int chapter, bool checkexists);
bool project_exists (const ustring& project);
void project_create_restore (const ustring& project, const ustring& restore_from);
void project_delete (const ustring& project);
void project_copy (const ustring& project, const ustring& newproject);
void project_move (const ustring& project, const ustring& newproject);
void project_store_book (const ustring& project, unsigned int book, CategorizeChapterVerse& ccv);
void project_remove_book (const ustring& project, unsigned int book);
void project_store_chapter (const ustring& project, unsigned int book, CategorizeChapterVerse& ccv);
void project_remove_chapter (const ustring& project, unsigned int book, unsigned int chapter);
void project_store_verse (const ustring& project, unsigned int book, unsigned int chapter, const ustring& verse, const ustring& data);
ustring project_retrieve_verse (const ustring& project, unsigned int book, unsigned int chapter, const ustring& verse);
vector<ustring> project_retrieve_chapter (const ustring& project, unsigned int book, unsigned int chapter);
vector<ustring> project_retrieve_book (const ustring& project, unsigned int book);
vector<unsigned int> project_get_books (const ustring& project);
vector<unsigned int> project_get_chapters (const ustring& project, unsigned int book);
vector<ustring> project_get_verses (const ustring& project, unsigned int book, unsigned int chapter);
bool project_book_exists (const ustring& project, unsigned int book);

bool project_select (ustring& newproject);


#endif
