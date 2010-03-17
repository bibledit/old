/*
** Copyright (©) 2003-2009 Teus Benschop.
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
#include <glib.h>
#include "bibletime.h"
#include "bible.h"
#include "shell.h"
#include "books.h"
#include "directories.h"
#include "sqlite_reader.h"
#include <sqlite3.h>
#include "tiny_utilities.h"
#include "settings.h"


bool bibletime_reference_receive (ustring text, Reference& reference)
{
  if (text.empty()) 
    return false;
  // The raw text should be something like: "[KJV] [BIBLE] Jer.48.13" (without the quotes).
  replace_text (text, ".", " ");
  Parse parse (text);
  if (parse.words.size() == 5) {
    reference.book = books_osis_to_id (parse.words[2]);
    reference.chapter = convert_to_int (parse.words[3]);
    reference.verse = parse.words[4];
    return true;
  }
  return false;
}


/*

To checkout bibletime:
svn co https://bibletime.svn.sourceforge.net/svnroot/bibletime/trunk/bibletime bibletime 

*/
