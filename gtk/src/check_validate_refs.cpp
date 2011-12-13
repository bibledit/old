/*
** Copyright (©) 2003-2012 Teus Benschop.
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

#include "check_validate_refs.h"
#include "projectutils.h"
#include "settings.h"
#include "utilities.h"
#include "books.h"
#include "reference.h"
#include "referenceutils.h"
#include "versification.h"
#include "tiny_utilities.h"

CheckValidateReferences::CheckValidateReferences(const ustring & project, const vector < unsigned int >&books, bool gui)
/*
It checks on the correctness and existence of the references found in the
text and in the notes.
project: project to check.
books: books to check; if empty it checks them all.
gui: whether to show graphical progressbar.
*/
{
  // The variables and settings.
  cancelled = false;
  myproject = project;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  versification = projectconfig->versification_get();
  language = projectconfig->language_get();

  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);

  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Validating references", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Go through each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    book = mybooks[bk];

    // Go through each chapter.
    vector < unsigned int >chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];

      // Go through each verse.
      vector < ustring > verses = project_get_verses(project, book, chapter);
      for (unsigned int vs = 0; vs < verses.size(); vs++) {
        verse = verses[vs];

        // Check the text.
        ustring line = project_retrieve_verse(project, book, chapter, verse);
        check(line);
      }
    }
  }
}

CheckValidateReferences::~CheckValidateReferences()
{
  // Clean up.
  if (progresswindow)
    delete progresswindow;
}

void CheckValidateReferences::check(const ustring & text)
// Do the actual check of one verse.
{
  // Bail out if the verse is empty.
  if (text.empty())
    return;

  // Extract the references and check them all.
  ReferencesScanner refscanner(language, book, text);
  for (unsigned int i = 0; i < references.size(); i++) {

    // Check whether the reference fits within the limits of the versification system.
    bool reference_fits = true;
    unsigned int highest_chapter = 0;
    vector < unsigned int >chapters = versification_get_chapters(versification, refscanner.references[i].book);
    if (!chapters.empty())
      highest_chapter = chapters[chapters.size() - 1];
    if (refscanner.references[i].chapter > highest_chapter)
      reference_fits = false;
    unsigned int last_verse = convert_to_int(versification_get_last_verse(versification, refscanner.references[i].book, refscanner.references[i].chapter));
    unsigned int this_verse = convert_to_int(refscanner.references[i].verse);
    if (this_verse > last_verse)
      reference_fits = false;
    if (!reference_fits) {
      message(refscanner.references[i].human_readable(language) + " does not fit in the versification system");
    }
    // Check whether the reference exists in the project.
    if (reference_fits) {
      ustring versetext = project_retrieve_verse(myproject, refscanner.references[i].book, refscanner.references[i].chapter, refscanner.references[i].verse);
      if (versetext.empty()) {
        message(refscanner.references[i].human_readable(language) + " contains no text");
      }
    }
  }

  // Look for chapter/verse pairs by pattern, and check on these.
  ustring mytext(text);
  while (mytext.length() > 5) {
    if (text_starts_chapter_verse(mytext)) {
      // This text starts a chapter/verse string.
      // Look for the dot or colon.
      size_t position = mytext.find_first_of(".:");
      if (position != string::npos) {
        unsigned int mychapter = convert_to_int(mytext.substr(0, position));
        mytext.erase(0, ++position);
        ustring myverse = number_in_string(mytext);
        mytext.erase(0, myverse.length());
        bool referencefound = false;
        for (unsigned int i = 0; i < references.size(); i++) {
          if (refscanner.references[i].chapter == mychapter)
            if (refscanner.references[i].verse == myverse)
              referencefound = true;
        }
        if (!referencefound) {
          message("Unrecognized chapter " + convert_to_string(mychapter) + " and verse " + myverse);
        }
      }
      mytext.erase(0, 3);
    } else {
      // No chapter/verse string: erase one character and try again.
      mytext.erase(0, 1);
    }
  }
}

void CheckValidateReferences::message(const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}
