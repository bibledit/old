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

#include "mapping.h"
#include "utilities.h"
#include "bible.h"
#include "shell.h"
#include "gwrappers.h"
#include "books.h"
#include "directories.h"
#include "mappings.h"
#include "tiny_utilities.h"

Mapping::Mapping(const ustring & name, unsigned int book)
{
  // Save variables.
  mybook = book;
  myname = name;
}

Mapping::~Mapping()
{
}

void Mapping::me_to_original(int mychapter, const ustring & myverse, vector < int >&original_chapter, vector < int >&original_verse)
/*
Input:
- a chapter (int).
- a verse (ustring) : can be a sequence or a range too.
Output:
- containers of mapped chapters/verses.
*/
{
  // Storage for the containers with chapters and verses to map.
  vector < int >chapters;
  vector < int >verses;
  // Convert the verse to a container with half verses.
  vector < int >half_verses = verses_encode(myverse);
  set < int >encoded;
  // Divide the half-verses by 2 and add them to the containers with values to map.
  for (unsigned int i = 0; i < half_verses.size(); i++) {
    int verse = half_verses[i] / 2;
    if (encoded.find(verse) == encoded.end()) {
      chapters.push_back(mychapter);
      verses.push_back(verse);
      encoded.insert(verse);
    }
  }
  // Do the mapping.
  me_to_original(chapters, verses, original_chapter, original_verse);
}

void Mapping::me_to_original(vector < int >mychapter, vector < int >myverse, vector < int >&original_chapter, vector < int >&original_verse)
/*
Input:
- containers of chapters/verses.
Output:
- containers of mapped chapters/verses.
Containers as input and as output are needed as there may be a verse that 
maps to one verse in a chapter, and one verse in the next chapter.
*/
{
  for (unsigned int i = 0; i < mychapter.size(); i++) {
    vector < int >origchapter;
    vector < int >origverse;
    get_original(mychapter[i], myverse[i], origchapter, origverse);
    for (unsigned int i2 = 0; i2 < origchapter.size(); i2++) {
      original_chapter.push_back(origchapter[i2]);
      original_verse.push_back(origverse[i2]);
    }
  }
}

void Mapping::me_to_original(Reference & reference)
{
  vector < int >remapped_chapters;
  vector < int >remapped_verses;
  me_to_original(reference.chapter_get(), reference.verse_get(), remapped_chapters, remapped_verses);
  reference.chapter_set(remapped_chapters[0]);
  reference.verse_set(convert_to_string(remapped_verses[0]));
  if (remapped_verses.size() > 1) {
    reference.verse_append("-");
    reference.verse_append(convert_to_string(remapped_verses[remapped_verses.size() - 1]));
  }
}

void Mapping::original_to_me(int originalchapter, const ustring & originalverse, vector < int >&mychapter, vector < int >&myverse)
/*
Input:
- a chapter (int).
- a verse (ustring) : can be a sequence or a range too.
Output:
- containers of mapped chapters/verses.
*/
{
  // Storage for the containers with chapters and verses to map.
  vector < int >chapters;
  vector < int >verses;
  // Convert the verse to a container with half verses.
  vector < int >half_verses = verses_encode(originalverse);
  set < int >encoded;
  // Divide the half-verses by 2 and add them to the containers with values to map.
  for (unsigned int i = 0; i < half_verses.size(); i++) {
    int verse = half_verses[i] / 2;
    if (encoded.find(verse) == encoded.end()) {
      chapters.push_back(originalchapter);
      verses.push_back(verse);
      encoded.insert(verse);
    }
  }
  // Do the mapping.
  original_to_me(chapters, verses, mychapter, myverse);
}

void Mapping::original_to_me(vector < int >original_chapter, vector < int >original_verse, vector < int >&mychapter, vector < int >&myverse)
{
  for (unsigned int i = 0; i < original_chapter.size(); i++) {
    vector <int> chapter;
    vector <int> verse;
    get_me(original_chapter[i], original_verse[i], chapter, verse);
    for (unsigned int i2 = 0; i2 < chapter.size(); i2++) {
      mychapter.push_back(chapter[i2]);
      myverse.push_back(verse[i2]);
    }
  }
}

void Mapping::original_to_me(Reference & reference)
{
  vector < int >remapped_chapters;
  vector < int >remapped_verses;
  original_to_me(reference.chapter_get(), reference.verse_get(), remapped_chapters, remapped_verses);
  reference.chapter_set(remapped_chapters[0]);
  reference.verse_set(convert_to_string(remapped_verses[0]));
  if (remapped_verses.size() > 1) {
    reference.verse_append("-");
    reference.verse_append(convert_to_string(remapped_verses[remapped_verses.size() - 1]));
  }
}

void Mapping::get_original(int mychapter, int myverse, vector < int >&originalchapter, vector < int >&originalverse)
{
  extern Mappings *mappings;
  mappings->me2original(myname, mybook, mychapter, myverse, originalchapter, originalverse);
  if (originalchapter.empty()) {
    originalchapter.push_back(mychapter);
    originalverse.push_back(myverse);
  }
}

void Mapping::get_me(int originalchapter, int originalverse, vector < int >&mychapter, vector < int >&myverse)
{
  extern Mappings *mappings;
  mappings->original2me(myname, mybook, originalchapter, originalverse, mychapter, myverse);
  if (mychapter.empty()) {
    mychapter.push_back(originalchapter);
    myverse.push_back(originalverse);
  }
}

void Mapping::book_change(int book)
// Changes the book.
{
  mybook = book;
}
