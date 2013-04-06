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

#include "check_ref_inventory.h"
#include "projectutils.h"
#include "settings.h"
#include "utilities.h"
#include "books.h"
#include "checks.h"
#include "referenceutils.h"

CheckReferenceInventory::CheckReferenceInventory(const ustring & project, const vector < unsigned int >&books, bool includetext, bool gui)
/*
It makes an inventory of the references in the project.
project: project to check.
books: books to check; if empty it checks them all.
includetext: whether to include the text of the references.
gui: show graphical progressbar.
*/
{
  // Variables.
  cancelled = false;
  // Language.
  extern Settings *settings;
  ustring language = settings->projectconfig(project, false)->language_get();
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Producing inventory", true);
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
    // Check each chapter.
    vector < unsigned int >chapters = project_get_chapters(project, mybooks[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      // Check each verse.
      vector < ustring > verses2 = project_get_verses(project, mybooks[bk], chapters[ch]);
      for (unsigned int vs = 0; vs < verses2.size(); vs++) {
        ustring text = project_retrieve_verse(project, mybooks[bk], chapters[ch], verses2[vs]);
        // Check for references the verse has.
        if (text_contains_reference(text)) {
          // Retrieve the references.
          ReferencesScanner refscanner(language, mybooks[bk], text);
          if (!refscanner.references.empty()) {
            // Store the references.
            Reference verse_ref(mybooks[bk], chapters[ch], verses2[vs]);
            verses.push_back(verse_ref.human_readable(language));
            ustring human_readable_references;
            for (unsigned int i = 0; i < refscanner.references.size(); i++) {
              if (i > 0) {
                if (includetext)
                  human_readable_references.append(" | ");
                else
                  human_readable_references.append(", ");
              }
              human_readable_references.append(refscanner.references[i].human_readable(language));
              if (includetext) {
                human_readable_references.append(" ");
                ustring text = project_retrieve_verse(project, refscanner.references[i].book, refscanner.references[i].chapter, refscanner.references[i].verse);
                if (text.empty())
                  text = "<empty>";
                human_readable_references.append(text);
              }
            }
            references.push_back(human_readable_references);
          }
        }
      }
    }
  }
}

CheckReferenceInventory::~CheckReferenceInventory()
{
  if (progresswindow)
    delete progresswindow;
}
