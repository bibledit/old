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


#ifndef INCLUDED_PROJECT_MEMORY_H
#define INCLUDED_PROJECT_MEMORY_H


#include "libraries.h"
#include "categorize.h"
#include "sqlite_reader.h"


class ProjectVerse
{
public:
  ProjectVerse (const ustring& number_in);
  ustring number;
  ustring data;
private:
};


class ProjectChapter
{
public:
  ProjectChapter (unsigned int number_in);
  unsigned int number;
  vector <ProjectVerse> data;
  vector <ustring> get_data ();
  void set_data (const vector <ustring>& lines);
  vector <ustring> get_verses ();
  ProjectVerse * get_verse_pointer (const ustring& number);
private:
};


class ProjectBook
{
public:
  ProjectBook (unsigned int number_in);
  unsigned int number;
  vector <ProjectChapter> data;
  vector <ustring> get_data ();
  vector <unsigned int> get_chapters ();
  ProjectChapter * get_chapter_pointer (unsigned int number);
private:
};


class ProjectMemory
{
public:
  ProjectMemory (const ustring& name_in, bool gui);
  ustring name;
  vector <ProjectBook> data;
  vector <unsigned int> get_books ();
  ProjectBook * get_book_pointer (unsigned int number);
  bool cancelled;
private:
};


#endif
