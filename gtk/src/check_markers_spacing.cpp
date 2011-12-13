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

#include "check_markers_spacing.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "tiny_utilities.h"

CheckMarkersSpacing::CheckMarkersSpacing(const ustring & project, const vector < unsigned int >&books, bool gui)
/*
It performs checks related to the USFM standard.
project: project to check.
books: books to check; if empty it checks them all.
gui: whether to show graphical progressbar.
*/
{
  // Init variables.
  cancelled = false;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // Get the markers to check the spacing for.
  extern Settings *settings;
  Parse parse(settings->genconfig.check_markers_spacing_include_get());
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    markers.insert(parse.words[i]);
  }
  // GUI.
  ProgressWindow *progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Checking spacing", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Check each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        break;
      }
    }
    book = mybooks[bk];
    // Check each chapter.
    vector < unsigned int >chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];
      vector < ustring > verses = project_get_verses(project, book, chapter);
      // Check each verse.
      for (unsigned int vs = 0; vs < verses.size(); vs++) {
        verse = verses[vs];
        ustring line = project_retrieve_verse(project, book, chapter, verse);
        check(line);
      }
    }
  }
  // Clean up.
  if (progresswindow)
    delete progresswindow;
}

CheckMarkersSpacing::~CheckMarkersSpacing()
{
}

void CheckMarkersSpacing::check(ustring text)
// Do the actual check of one verse.
{
  // Extract the marker, and deal with it.
  text = trim(text);
  if (text.empty())
    return;
  ustring marker = usfm_extract_marker_within_line(text);
  while (!marker.empty()) {
    if (markers.find(marker) != markers.end()) {
      size_t pos = text.find("\\");
      if (pos != string::npos) {
        if (text.substr(--pos, 1) == " ") {
          if (pos > 1) {
            if (text.substr(--pos, 1) == " ") {
              message("Text of marker " + marker + " ends with more than one space");
            }
          }
        } else {
          message("Text of marker " + marker + " doesn't end with a space");
        }
      }
    }
    // Extract any next marker in this text.
    marker = usfm_extract_marker_within_line(text);
  }
}

void CheckMarkersSpacing::message(const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}
