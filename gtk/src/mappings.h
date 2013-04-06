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


#ifndef INCLUDED_MAPPINGS_H
#define INCLUDED_MAPPINGS_H


#include "libraries.h"


class MappingStore
{
public:
  MappingStore (const ustring& system_in, const ustring& filename);
  ustring system;
  vector <unsigned int> id;
  vector <unsigned int> chapter;
  vector <unsigned int> verse;
  vector <unsigned int> original_id;
  vector <unsigned int> original_chapter;
  vector <unsigned int> original_verse;
};


class Mappings
{
public:
  Mappings (int dummy);
  vector <ustring> systems_get ();
  void me2original (const ustring& system, int mybook, int mychapter, int myverse, 
                    vector<int>& originalchapter, vector<int>& originalverse);
  void original2me (const ustring& system, int originalbook, int originalchapter, int originalverse, 
                    vector<int>& mychapter, vector<int>& myverse);
private:
  vector <ustring> available_systems;
  vector <ustring> available_filenames;
  ustring filename_get_system (ustring filename);
  vector <MappingStore> loaded_systems;
  unsigned int system_pointer_get (ustring system);
};


#endif
