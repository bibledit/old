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

#include "check_count_characters.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "checks.h"

CheckCountCharacters::CheckCountCharacters(const ustring & project, const vector < unsigned int >&books, bool sortcharacter, bool sortcount, bool gui)
/*
It counts the characters in the project.
project: project to check.
books: books to check; if empty it checks them all.
sortcharacter: sort on word.
sortcount: sort on count.
gui: show graphical progressbar.
*/
{
  // Initialize variables.
  cancelled = false;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Counting characters", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Check each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    // Get text of the book and go through each line.
    vector < ustring > lines = project_retrieve_book(project, mybooks[bk]);
    for (unsigned int ln = 0; ln < lines.size(); ln++) {
      ustring line(lines[ln]);
      for (unsigned int c = 0; c < line.size(); c++) {
        ustring character = line.substr(c, 1);
        gchar *g_character = g_strdup((gchar *) character.c_str());
        bool character_found = false;
        for (unsigned int i2 = 0; i2 < characters.size(); i2++) {
          /*
             We would have said: if (character == characters[i2])
             But something funny is going on here. In a text like
             nǀi nǂûî
             the ǀ would be considered equal to the ǂ, so the latter would not show
             up in the inventory under ǂ, but under ǀ.
             Comparing g_chars solves this.
           */
          if (g_strrstr(g_characters[i2], g_character) != NULL) {
            character_found = true;
            counts[i2]++;
            break;
          }
        }
        if (!character_found) {
          characters.push_back(character);
          g_characters.push_back(g_strdup(g_character));
          counts.push_back(1);
        }
        g_free(g_character);
      }
    }
  }
  // Sorting, if requested.
  if (sortcharacter) {
    quick_sort(characters, counts, 0, characters.size());
  }
  if (sortcount) {
    quick_sort(counts, characters, 0, counts.size());
  }
}

CheckCountCharacters::~CheckCountCharacters()
{
  if (progresswindow)
    delete progresswindow;
  for (unsigned int i = 0; i < g_characters.size(); i++) {
    g_free(g_characters[i]);
  }
}

/*
Paratext can find invalid characters, but as we have Unicode, where all 
characters are valid, there is no need for this check.

Diacritics usage in Paratext. It shows all character sequences that have 
diacritics, how many times each sequence occurs, and the reference where it 
occurs. We don't use it as we have Unicode, and that views diacritics as any 
other character.
*/
