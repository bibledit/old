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


#ifndef INCLUDED_VERSIFICATIONS_H
#define INCLUDED_VERSIFICATIONS_H


#include "libraries.h"


class Versification
{
public:
  Versification (const ustring& system_in, const ustring& filename);
  ustring system;
  vector <unsigned int> id;
  vector <unsigned int> chapter;
  vector <unsigned int> verse;
};


class Versifications
{
public:
  Versifications (int dummy);
  vector <ustring> systems_get ();
  void book2chaptersverses (const ustring& system, unsigned int id, vector<unsigned int>& chapters, vector<unsigned int>& verses);
private:
  vector <ustring> available_systems;
  vector <ustring> available_filenames;
  ustring filename_get_system (ustring filename);
  vector <Versification> loaded_localizations;
  unsigned int system_pointer_get (ustring system);
};


#endif
