/*
Copyright (©) 2003-2016 Teus Benschop.

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


void bible_logic_store_chapter (const string& bible, int book, int chapter, const string& usfm);
void bible_logic_delete_chapter (const string& bible, int book, int chapter);
void bible_logic_delete_book (const string& bible, int book);
void bible_logic_delete_bible (const string& bible);
void bible_logic_import_resource (string bible, string resource);
void bible_logic_log_change (const string& bible, int book, int chapter, const string& usfm, const string & user, const string & summary);


#endif
