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


#ifndef INCLUDED_TRACK_H
#define INCLUDED_TRACK_H


#include "libraries.h"
#include "referenceutils.h"


class Track
{
public:
  Track (unsigned int dummy);
  ~Track ();
  void store (const Reference& reference);
  void get_previous_references (vector <Reference>& references);
  bool previous_reference_available ();
  void get_previous_reference (Reference& reference);
  void get_next_reference (Reference& reference);
  bool next_reference_available ();
  void get_next_references (vector <Reference>& references);
  void clear ();
private:
  vector <Reference> recorded_references;
  int reference_pointer;
  void state ();
};


#endif
