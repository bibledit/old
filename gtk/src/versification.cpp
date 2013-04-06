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

#include "versification.h"
#include "utilities.h"
#include "bible.h"
#include "books.h"
#include "sqlite_reader.h"
#include "gwrappers.h"
#include "directories.h"
#include "versifications.h"
#include "tiny_utilities.h"

void versification_get_chapters_verses(const ustring & versification, unsigned int book, vector < unsigned int >&chapters, vector < unsigned int >&verses)
{
  extern Versifications *versifications;
  versifications->book2chaptersverses(versification, book, chapters, verses);
}

void versification_create_book_template(const ustring & versification, unsigned int book, vector < ustring > &booktemplate)
{
  vector < unsigned int >chapters = versification_get_chapters(versification, book);
  chapters.insert(chapters.begin(), 0);
  for (unsigned int i = 0; i < chapters.size(); i++) {
    versification_create_chapter_template(versification, book, chapters[i], booktemplate);
  }
}

void versification_create_chapter_template(const ustring & versification, unsigned int book, unsigned int chapter, vector < ustring > &chaptertemplate)
{
  // In case of chapter 0, add the id, and other relevant stuff.
  if (chapter == 0) {
    chaptertemplate.push_back("\\id " + books_id_to_paratext(book));
    chaptertemplate.push_back("\\h " + books_id_to_english(book));
    chaptertemplate.push_back("\\toc2 " + books_id_to_english(book));
    return;
  }
  // Find the number of verses in a chapter.
  vector < unsigned int >chapters;
  vector < unsigned int >verses;
  versification_get_chapters_verses(versification, book, chapters, verses);
  // See if our chapter is there, if not add it with a default number of verses.
  set < unsigned int >chapter_set(chapters.begin(), chapters.end());
  if (chapter_set.find(chapter) == chapter_set.end()) {
    chapters.push_back(chapter);
    verses.push_back(1);
  }
  // Generate the templete.  
  for (unsigned int i = 0; i < chapters.size(); i++) {
    if (chapters[i] == chapter) {
      chaptertemplate.push_back("\\c " + convert_to_string(chapter));
      chaptertemplate.push_back("\\p ");
      for (unsigned int verse = 1; verse <= verses[i]; verse++) {
        chaptertemplate.push_back("\\v " + convert_to_string(verse));
      }
    }
  }
  // Add full stop.
  // This solves the bug that pasting text in the last verse of an empty
  // template has all of that pasted text formatted as a verse.
  chaptertemplate.push_back(".");
}

vector < unsigned int >versification_get_chapters(const ustring & versification, unsigned int book)
{
  vector < unsigned int >chapters;
  vector < unsigned int >verses;
  versification_get_chapters_verses(versification, book, chapters, verses);
  return chapters;
}

ustring versification_get_last_verse(const ustring & versification, unsigned int book, unsigned int chapter)
{
  ustring lastverse = "1";
  vector < unsigned int >chapters;
  vector < unsigned int >verses;
  versification_get_chapters_verses(versification, book, chapters, verses);
  for (unsigned int i = 0; i < chapters.size(); i++) {
    if (chapters[i] == chapter) {
      lastverse = convert_to_string(verses[i]);
    }
  }
  return lastverse;
}
