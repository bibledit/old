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


#ifndef INCLUDED_STATISTICS_H
#define INCLUDED_STATISTICS_H


#include "libraries.h"
#include <glib.h>
#include "categorize.h"


ustring statistics_database (const ustring& project, const gchar * suffix = NULL);
void statistics_initial_check_all ();
void statistics_initial_check_project (const ustring& project, bool gui);
void statistics_record_store_chapter (const ustring& project, unsigned int book, unsigned int chapter, 
                                      CategorizeChapterVerse& ccv, unsigned int first, unsigned int last);
void statistics_record_store_verse (const ustring& project, unsigned int book, unsigned int chapter, 
                                    const ustring& verse, const ustring& data);
void statistics_record_remove_chapter (const ustring& project, unsigned int book, unsigned int chapter);
void statistics_record_remove_book (const ustring& project, unsigned int book);


#endif
