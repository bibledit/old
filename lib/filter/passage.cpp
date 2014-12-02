/*
Copyright (©) 2003-2014 Teus Benschop.

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


#include <filter/passage.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/usfm.h>
#include <database/books.h>


Passage::Passage ()
{
  book = 0;
  chapter = 0;
}


Passage::Passage (string bible_in, int book_in, int chapter_in, string verse_in)
{
  bible = bible_in;
  book = book_in;
  chapter = chapter_in;
  verse = verse_in;
}


bool Passage::equal (Passage & passage)
{
  if (bible != passage.bible) return false;
  if (book != passage.book) return false;
  if (chapter != passage.chapter) return false;
  if (verse != passage.verse) return false;
  return true;
}


string filter_passage_display (int book, int chapter, string verse)
{
  string sbook = gettext (Database_Books::getEnglishFromId (book).c_str());
  string display = sbook + " " + convert_to_string (chapter);
  if (!verse.empty ()) display.append (":" + verse);
  return display;
}


