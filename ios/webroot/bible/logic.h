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


#ifndef INCLUDED_BIBLE_LOGIC_H
#define INCLUDED_BIBLE_LOGIC_H


#include <config/libraries.h>


class Bible_Logic
{
public:
  static void storeChapter (const string& bible, int book, int chapter, const string& usfm);
  static void deleteChapter (const string& bible, int book, int chapter);
  static void deleteBook (const string& bible, int book);
  static void deleteBible (const string& bible);
};


void bible_logic_import_resource (string bible, string resource);


#endif
