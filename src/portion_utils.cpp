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


#include "portion_utils.h"
#include "constants.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "tiny_utilities.h"


void select_portion_get_values(const ustring & project, unsigned int book, const ustring & portion, vector < unsigned int >&chapters_from, vector < ustring > &verses_from, vector < unsigned int >&chapters_to, vector < ustring > &verses_to)
// Takes "portion", and extracts the chapters and verses from it.
// Checks whether everything is in the limits as given by "bookmetrics".
{
  chapters_from.clear();
  verses_from.clear();
  chapters_to.clear();
  verses_to.clear();
  vector < unsigned int >chapters = project_get_chapters(project, book);
  unsigned int lowest_chapter = chapters[0];
  unsigned int highest_chapter = chapters[chapters.size() - 1];
  vector < ustring > portions = select_portion_get_portions(portion);
  for (unsigned int i = 0; i < portions.size(); i++) {
    unsigned int chapter_from, chapter_to;
    ustring verse_from, verse_to;
    if (portions[i] == CHAPTER_VERSE_SELECTION_ALL) {
      chapter_from = lowest_chapter;
      vector < ustring > from_verses = project_get_verses(project, book, chapter_from);
      verse_from = from_verses[0];
      chapter_to = highest_chapter;
      vector < ustring > to_verses = project_get_verses(project, book, chapter_to);
      verse_to = to_verses[to_verses.size() - 1];
    } else {
      // Format of portion, e.g.: "10.6-7 to 11.1,2a"
      // Work on a copy.
      ustring myportion(portions[i]);
      // Get starting chapter number.
      ustring s = number_in_string(myportion);
      chapter_from = convert_to_int(number_in_string(s));
      chapter_from = CLAMP(chapter_from, lowest_chapter, highest_chapter);
      // Remove the part from the portion, including the dot.
      myportion.erase(0, s.length() + 1);
      // Get starting verse number.
      size_t position = myportion.find(" ");
      verse_from = myportion.substr(0, position);
      // Remove it from the portion.
      myportion.erase(0, verse_from.length() + strlen(CHAPTER_VERSE_SELECTION_TO));
      // Limit starting verse.
      vector < ustring > from_verses = project_get_verses(project, book, chapter_from);
      set < ustring > from_set(from_verses.begin(), from_verses.end());
      if (from_set.find(verse_from) == from_set.end())
        verse_from = from_verses[0];
      // Get ending chapter number.
      s = number_in_string(myportion);
      chapter_to = convert_to_int(number_in_string(s));
      chapter_to = CLAMP(chapter_to, lowest_chapter, highest_chapter);
      // Remove the part from the portion, including the dot.
      myportion.erase(0, s.length() + 1);
      // Get verse_to and limit it.
      verse_to = myportion;
      vector < ustring > to_verses = project_get_verses(project, book, chapter_to);
      set < ustring > to_set(to_verses.begin(), to_verses.end());
      if (to_set.find(verse_to) == to_set.end())
        verse_to = to_verses[to_verses.size() - 1];
    }
    // Store what we got.
    chapters_from.push_back(chapter_from);
    chapters_to.push_back(chapter_to);
    verses_from.push_back(verse_from);
    verses_to.push_back(verse_to);
  }
}


ustring select_portion_get_label(const ustring & project, unsigned int book, unsigned int chapter_from, const ustring & verse_from, unsigned int chapter_to, const ustring & verse_to)
{
  // Assemble the label, e.g.: "10.6-7 to 11.1,2a"
  ustring label;
  label = convert_to_string(chapter_from);
  label.append(".");
  label.append(verse_from);
  label.append(CHAPTER_VERSE_SELECTION_TO);
  label.append(convert_to_string(chapter_to));
  label.append(".");
  label.append(verse_to);
  // If the selection covers everything, return that value instead.  
  vector < unsigned int >chapters = project_get_chapters(project, book);
  unsigned int lowest_chapter = chapters[0];
  if (chapter_from == lowest_chapter) {
    unsigned int highest_chapter = chapters[chapters.size() - 1];
    if (chapter_to == highest_chapter) {
      vector < ustring > from_verses = project_get_verses(project, book, chapter_from);
      if (verse_from == from_verses[0]) {
        vector < ustring > to_verses = project_get_verses(project, book, chapter_to);
        if (verse_to == to_verses[to_verses.size() - 1]) {
          label = CHAPTER_VERSE_SELECTION_ALL;
        }
      }
    }
  }
  // Return value.
  return label;
}


vector < ustring > select_portion_get_portions(const ustring & portion)
// Extract the various possible portions from the one master-portion.
{
  Parse parse(portion, false, "|");
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    parse.words[i] = trim(parse.words[i]);
  }
  return parse.words;
}


ustring select_portion_get_portion(const vector < ustring > &portions)
// Assemble a master-portion from the various possible portions.
{
  ustring portion;
  for (unsigned int i = 0; i < portions.size(); i++) {
    if (i) {
      portion.append(" | ");
    }
    portion.append(portions[i]);
  }
  return portion;
}


WithinReferencesRange::WithinReferencesRange()
{
}


WithinReferencesRange::WithinReferencesRange(int dummy)
{
}


void WithinReferencesRange::add_portion(unsigned int book_in, vector < unsigned int >chapters_from_in, const vector < ustring > &verses_from_in, vector < unsigned int >chapters_to_in, const vector < ustring > &verses_to_in)
{
  // Verify the consistency of the portion.
  if (chapters_from_in.size() != verses_from_in.size())
    return;
  if (chapters_from_in.size() != chapters_to_in.size())
    return;
  if (chapters_from_in.size() != verses_to_in.size())
    return;

  // Add this portion to any existing ones.
  for (unsigned int i = 0; i < chapters_from_in.size(); i++) {
    books.push_back(book_in);
    chapters_from.push_back(chapters_from_in[i]);
    verses_from.push_back(verses_from_in[i]);
    chapters_to.push_back(chapters_to_in[i]);
    verses_to.push_back(verses_to_in[i]);
    in_range_flags.push_back(false);
  }

  // Initialize variables.
  mybook = 0;
  mychapter = 0;
}


void WithinReferencesRange::set_book(unsigned int book)
{
  mybook = book;
  set_chapter(0);
}


void WithinReferencesRange::set_chapter(unsigned int chapter)
{
  mychapter = chapter;
  set_verse("0");
}


void WithinReferencesRange::set_verse(const ustring & verse)
{
  myverse = verse;
  verify_range();
}


void WithinReferencesRange::verify_range()
// Verify whether the current reference is within the various ranges.
// Set internal flags accordingly.
{
  // Go through the available portions.
  for (unsigned int i = 0; i < chapters_from.size(); i++) {
    // Proceed if the book matches.
    if (books[i] == mybook) {
      // Compare on chapter.
      if (mychapter < chapters_from[i]) {
        in_range_flags[i] = false;
      } else if (mychapter > chapters_to[i]) {
        in_range_flags[i] = false;
      } else {
        // At this stage the chapter is within the range of chapters to be printed.
        unsigned int vs = convert_to_int(myverse);
        if (mychapter == chapters_from[i]) {
          unsigned int vs_from = convert_to_int(verses_from[i]);
          in_range_flags[i] = (vs >= vs_from);
        }
        if (mychapter == chapters_to[i]) {
          unsigned int vs_to = convert_to_int(verses_to[i]);
          if (vs > vs_to)
            in_range_flags[i] = false;
        }
      }
    }
  }
}


bool WithinReferencesRange::in_range()
// Return true if the current reference is within the various ranges.
{
  // If no ranges are given, it is considered "in range".
  if (in_range_flags.empty())
    return true;
  // Check from the flags whether we are in range.
  for (unsigned int i = 0; i < in_range_flags.size(); i++) {
    if (in_range_flags[i]) {
      return true;
    }
  }
  // Out of range.
  return false;
}

