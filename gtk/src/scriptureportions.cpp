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

#include "scriptureportions.h"
#include "projectutils.h"
#include "settings.h"
#include "books.h"

ScripturePortions::ScripturePortions(const ustring & project)
// This object encapsulates a Scripture and its portions.
{
  myproject = project;
  reordering_read();
  vector < unsigned int >scripture_books = project_get_books(project);
  for (unsigned int i = 0; i < reordered_books.size(); i++) {
    for (unsigned int i2 = 0; i2 < scripture_books.size(); i2++) {
      if (reordered_books[i] == books_id_to_english(scripture_books[i2])) {
        if (reordered_includes[i]) {
          books.push_back(reordered_books[i]);
          portions.push_back(reordered_portions[i]);
        }
      }
    }
  }
}

void ScripturePortions::reordering_read()
/*
Reads reordering information from disk.

If the amount of settings on disk changes, it includes all bits.
*/
{
  // Load scripture.
  vector < ustring > scripture_books;
  {
    vector < unsigned int >s = project_get_books(myproject);
    for (unsigned int i = 0; i < s.size(); i++)
      scripture_books.push_back(books_id_to_english(s[i]));
  }

  // Read the reordering information.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
  reordered_books = projectconfig->reordered_books_get();
  reordered_includes = projectconfig->reordered_includes_get();
  reordered_portions = projectconfig->reordered_portions_get();

  // See if the amount of books is still the same. If not, reset the whole 
  // thing.
  bool reset = false;
  if (scripture_books.size() != reordered_books.size())
    reset = true;
  if (scripture_books.size() != reordered_includes.size())
    reset = true;
  if (scripture_books.size() != reordered_portions.size())
    reset = true;

  // No reset: we're through.
  if (!reset)
    return;

  // Reset: include everything.
  reordered_books.clear();
  reordered_includes.clear();
  reordered_portions.clear();
  for (unsigned int i = 0; i < scripture_books.size(); i++) {
    reordered_books.push_back(scripture_books[i]);
    reordered_includes.push_back(true);
    reordered_portions.push_back(CHAPTER_VERSE_SELECTION_ALL);
  }
}

bool ScripturePortions::reordering_portions_all()
// Indicates whether all books have their portions set to "all"  and are included.
{
  for (unsigned int i = 0; i < reordered_includes.size(); i++)
    if (!reordered_includes[i])
      return false;
  for (unsigned int i = 0; i < reordered_portions.size(); i++)
    if (reordered_portions[i] != CHAPTER_VERSE_SELECTION_ALL)
      return false;
  return true;
}

bool ScripturePortions::included(const ustring & book)
// Returns true if the book is included in the portion.
{
  bool incl = false;
  for (unsigned int i = 0; i < reordered_books.size(); i++) {
    if (reordered_books[i] == book) {
      incl = reordered_includes[i];
    }
  }
  return incl;
}
