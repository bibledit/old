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


#include "libraries.h"
#include "utilities.h"
#include "track.h"
#include "bible.h"


Track::Track(unsigned int dummy)
{
  clear();
}


Track::~Track()
{
}


void Track::store(const Reference & reference)
// Stores the reference for tracking them.
{
  recorded_references.push_back(reference);
  reference_pointer = recorded_references.size();
}


void Track::get_previous_references (vector <Reference>& references)
// Gets the previous references, the ones we can go back to.
{
  for (int i = reference_pointer - 2; i >= 0; i--) {
    references.push_back (recorded_references[i]);
  }  
}


bool Track::previous_reference_available()
{
  return (reference_pointer > 0);
}


void Track::get_previous_reference(Reference & reference)
{
  reference_pointer--;
  bool equal = reference.equals(recorded_references[reference_pointer]);
  reference = recorded_references[reference_pointer];
  if (equal && previous_reference_available()) {
    reference_pointer--;
    reference = recorded_references[reference_pointer];
  }
}


void Track::get_next_reference(Reference & reference)
{
  bool equal = reference.equals(recorded_references[reference_pointer]);
  reference = recorded_references[reference_pointer];
  reference_pointer++;
  if (equal && next_reference_available()) {
    reference = recorded_references[reference_pointer];
    reference_pointer++;
  }
}


bool Track::next_reference_available()
{
  return (reference_pointer < int (recorded_references.size()));
}


void Track::get_next_references (vector <Reference>& references)
{
  for (unsigned int i = reference_pointer + 1; i < recorded_references.size(); i++) {
    references.push_back (recorded_references[i]);
  }  
}


void Track::clear()
{
  reference_pointer = 0;
  recorded_references.clear();
}


void Track::state()
// For testing only.
{
  /*
  cout << "--begin state--" << endl;
  for (unsigned int i = 0; i < recorded_references.size(); i++) {
    cout << recorded_references[i].human_readable ("") << endl;
  }
  cout << "pointer: " << reference_pointer << endl;
  cout << "--end state--" << endl;
  */
}
