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

#include "check_unwanted_patterns.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "scripturechecks.h"
#include "tiny_utilities.h"

CheckUnwantedPatterns::CheckUnwantedPatterns(const ustring & project, const vector < unsigned int >&books, ustring patternsfile, bool gui)
/*
It checks for unwanted patterns in the text.
project: project to check.
books: books to check; if empty it checks them all.
patternsfile: file that contains the patterns to look for. If empty, it takes
              the project's file with the patterns.
gui: whether to show graphical progressbar.
*/
{
  // Variables.
  cancelled = false;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // Read the patterns to look for.
  if (patternsfile.empty())
    patternsfile = checks_unwanted_patterns_get_filename(project);
  vector < ustring > patterns;
  ReadText rt(patternsfile, true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++)
    patterns.push_back(rt.lines[i]);
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Looking for unwanted patterns", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Check each book in the project.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    // Check each chapter in the book.
    vector < unsigned int >chapters = project_get_chapters(project, mybooks[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      vector < ustring > verses = project_get_verses(project, mybooks[bk], chapters[ch]);
      // Check each verse in the chapter.
      for (unsigned int vs = 0; vs < verses.size(); vs++) {
        ustring line = project_retrieve_verse(project, mybooks[bk], chapters[ch], verses[vs]);
        // Check the verse.
        CategorizeLine categorize(line);
        ustring text(categorize.id);
        text.append(categorize.intro);
        text.append(categorize.head);
        text.append(categorize.chap);
        text.append(categorize.study);
        text.append(categorize.note);
        text.append(categorize.ref);
        text.append(categorize.verse);
        for (unsigned int i = 0; i < patterns.size(); i++) {
          if (text.find(patterns[i]) != string::npos) {
            ustring message = "Unwanted pattern ";
            message.append(patterns[i]);
            references.push_back(books_id_to_english(mybooks[bk]) + " " + convert_to_string(chapters[ch]) + ":" + verses[vs]);
            comments.push_back(message);
          }
        }
      }
    }
  }
}

CheckUnwantedPatterns::~CheckUnwantedPatterns()
{
  // Clean up.
  if (progresswindow)
    delete progresswindow;
}
