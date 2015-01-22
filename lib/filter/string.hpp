/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef INCLUDED_FILTER_STRING_HPP
#define INCLUDED_FILTER_STRING_HPP


#include <config/libraries.h>


template <typename T>
vector <T> array_intersect (vector <T> a, vector <T> b)
{
  vector <T> result;
  set <T> aset (a.begin(), a.end());
  for (auto & item : b) {
    if (aset.find (item) != aset.end()) {
      result.push_back (item);
    }
  }
  return result;
}


// A C++ equivalent for PHP's in_array function.
template <typename T>
bool filter_string_in_array (const T & needle, const vector <T> & haystack)
{
  return (find (haystack.begin(), haystack.end(), needle) != haystack.end());
}


#endif
