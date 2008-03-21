/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "portion_utils.h"
#include "constants.h"
#include "utilities.h"
#include "projectutils.h"
#include "books.h"
#include "tiny_utilities.h"


void select_portion_get_values (const ustring& project, unsigned int book, const ustring& portion,
                                unsigned int& chapter_from, ustring& verse_from,
                                unsigned int& chapter_to, ustring& verse_to)
// Takes "portion", and extracts the chapters and verses from it.
// Checks whether everything is in the limits as given by "bookmetrics".
{
  vector<unsigned int> chapters = project_get_chapters(project, book);
  unsigned int lowest_chapter = chapters[0];
  unsigned int highest_chapter = chapters[chapters.size() - 1];
  if (portion == CHAPTER_VERSE_SELECTION_ALL) {
    chapter_from = lowest_chapter;
    vector<ustring> from_verses = project_get_verses (project, book, chapter_from);
    verse_from = from_verses[0];
    chapter_to = highest_chapter;
    vector<ustring> to_verses = project_get_verses (project, book, chapter_to);
    verse_to = to_verses[to_verses.size() - 1];   
  } else {
    // Format of portion, e.g.: "10.6-7 to 11.1,2a"
    // Work on a copy.
    ustring myportion (portion);
    // Get starting chapter number.
    ustring s = number_in_string (myportion);
    chapter_from = convert_to_int (number_in_string (s));
    chapter_from = CLAMP (chapter_from, lowest_chapter, highest_chapter);
    // Remove the part from the portion, including the dot.
    myportion.erase (0, s.length () + 1);
    // Get starting verse number.
    size_t position = myportion.find (" ");
    verse_from = myportion.substr (0, position);
    // Remove it from the portion.
    myportion.erase (0, verse_from.length() + strlen (CHAPTER_VERSE_SELECTION_TO));
    // Limit starting verse.
    vector<ustring> from_verses = project_get_verses (project, book, chapter_from);
    set<ustring> from_set (from_verses.begin(), from_verses.end());
    if (from_set.find (verse_from) == from_set.end())
      verse_from = from_verses[0];
    // Get ending chapter number.
    s = number_in_string (myportion);
    chapter_to = convert_to_int (number_in_string (s));
    chapter_to = CLAMP (chapter_to, lowest_chapter, highest_chapter);
    // Remove the part from the portion, including the dot.
    myportion.erase (0, s.length () + 1);
    // Get verse_to and limit it.
    verse_to = myportion;
    vector<ustring> to_verses = project_get_verses (project, book, chapter_to);
    set<ustring> to_set (to_verses.begin(), to_verses.end());
    if (to_set.find (verse_to) == to_set.end())
      verse_to = to_verses[to_verses.size() - 1];
  }
}


ustring select_portion_get_label (const ustring& project, unsigned int book,
                                  unsigned int chapter_from, const ustring& verse_from,
                                  unsigned int chapter_to, const ustring& verse_to)
{
  // Assemble the label, e.g.: "10.6-7 to 11.1,2a"
  ustring label;
  label = convert_to_string (chapter_from);
  label.append (".");
  label.append (verse_from);
  label.append (CHAPTER_VERSE_SELECTION_TO);
  label.append (convert_to_string (chapter_to));
  label.append (".");
  label.append (verse_to);
  // If the selection covers everything, return that value instead.  
  vector<unsigned int> chapters = project_get_chapters(project, book);
  unsigned int lowest_chapter = chapters[0];
  if (chapter_from == lowest_chapter) {
    unsigned int highest_chapter = chapters[chapters.size() - 1];
    if (chapter_to == highest_chapter) {
      vector<ustring> from_verses = project_get_verses (project, book, chapter_from);
      if (verse_from == from_verses[0]) {
        vector<ustring> to_verses = project_get_verses (project, book, chapter_to);
        if (verse_to == to_verses[to_verses.size() - 1]) {
          label = CHAPTER_VERSE_SELECTION_ALL;          
        }        
      }
    }    
  }
  // Return value.
  return label;
}
