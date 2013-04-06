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

#include "reference.h"
#include "books.h"
#include "utilities.h"
#include "bible.h"
#include "mapping.h"
#include "tiny_utilities.h"

Reference::Reference(int dummy)
{
  book = 0;
  chapter = 0;
  verse.clear();
}

Reference::Reference(unsigned int book_in, unsigned int chapter_in, const ustring & verse_in)
{
  book = book_in;
  chapter = chapter_in;
  verse = verse_in;
}

ustring Reference::human_readable(const ustring & language)
// Gives a reference in a human readable format. If no language is given,
// it takes the English names of the books.
{
  ustring s;
  if (language.empty())
    s.append(books_id_to_english(book));
  else
    s.append(books_id_to_name(language, book));
  s.append(" ");
  s.append(convert_to_string(chapter));
  s.append(":");
  s.append(verse);
  return s;
}

bool Reference::equals(const Reference & reference)
{
  // See if the references are exactly the same.
  if (book == reference.book) {
    if (chapter == reference.chapter) {
      if (verse == reference.verse) {
        return true;
      } else {
        // See if they overlap, take range and sequence in account.
        vector < unsigned int >me = verse_range_sequence(verse);
        vector < unsigned int >he = verse_range_sequence(reference.verse);
        for (unsigned int m = 0; m < me.size(); m++) {
          for (unsigned int h = 0; h < he.size(); h++) {
            if (me[m] == he[h])
              return true;
          }
        }
      }
    }
  }
  // No match found: The references differ.
  return false;
}

void Reference::assign(const Reference & reference)
{
  book = reference.book;
  chapter = reference.chapter;
  verse = reference.verse;
}
