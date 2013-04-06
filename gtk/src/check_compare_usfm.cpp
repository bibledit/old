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

#include "check_compare_usfm.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "checks.h"
#include "versification.h"
#include "tiny_utilities.h"

CheckCompareUsfms::CheckCompareUsfms(const ustring & project, const ustring & project2, const vector < unsigned int >&books, bool gui, bool allmarkers, const ustring & includeonly, const ustring & ignore, bool ignoreverse0)
/*
It compares the USFMs of one project with the USFMs of the other project.
project: project to check.
books: books to check; if empty it checks them all.
gui: show graphical progressbar.
*/
{
  // Initialize variables.
  cancelled = false;
  myproject = project;
  myproject2 = project2;
  myallmarkers = allmarkers;
  if (!includeonly.empty()) {
    Parse parse(includeonly);
    for (unsigned int i = 0; i < parse.words.size(); i++)
      myincludeonly.insert(parse.words[i]);
  }
  if (!ignore.empty()) {
    Parse parse(ignore);
    for (unsigned int i = 0; i < parse.words.size(); i++)
      myignore.insert(parse.words[i]);
  }
  myignoreverse0 = ignoreverse0;
  // Get the books of the two projects.
  vector < unsigned int >scripture_books = project_get_books(project);
  vector < unsigned int >scripture2_books = project_get_books(project2);
  // Container with flags to verify all books in scripture2 have been processed.
  vector < bool > scripture2_checked;
  for (unsigned int i = 0; i < scripture2_books.size(); i++)
    scripture2_checked.push_back(false);
  // Progress information.  
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Comparing USFMs", true);
    progresswindow->set_iterate(0, 1, scripture_books.size());
  }
  // Book selecton: If no books given, take them all.
  set < unsigned int >selected_books;
  if (books.empty()) {
    for (unsigned int i = 0; i < scripture_books.size(); i++)
      selected_books.insert(scripture_books[i]);
  } else {
    for (unsigned int i = 0; i < books.size(); i++)
      selected_books.insert(books[i]);
  }
  // Go through each book we have in this scripture.
  for (unsigned int i = 0; i < scripture_books.size(); i++) {
    // Progress.
    if (gui) {
      progresswindow->iterate();
      progresswindow->set_text(books_id_to_english(scripture_books[i]));
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    // See whether this book is included in the ones to check.
    if (selected_books.find(scripture_books[i]) != selected_books.end()) {
      // Get pointer to same book in second scripture, if it exists.
      int pointer2 = -1;
      for (unsigned int i2 = 0; i2 < scripture2_books.size(); i2++) {
        if (scripture_books[i] == scripture2_books[i2]) {
          pointer2 = i2;
          break;
        }
      }
      // Does the book exist in the second project?
      if (project_book_exists(project2, scripture_books[i])) {
        // Ok, same book found in second scripture: check the book.
        comparebook(scripture_books[i]);
        // Mark this book as done.
        scripture2_checked[pointer2] = true;
      } else {
        // Same book not found: give message about that.
        message(scripture_books[i], 0, "0", "Extra book");
      }
    }
  }
  // See whether there are still some unchecked books in the second scripture.
  // Give message if there are unchecked ones, but only in case ALL books
  // were selected for checking.
  if (scripture_books.size() == selected_books.size()) {
    for (unsigned int i = 0; i < scripture2_checked.size(); i++) {
      if (!scripture2_checked[i]) {
        message(scripture2_books[i], 0, "0", "Missing book");
      }
    }
  }
}

CheckCompareUsfms::~CheckCompareUsfms()
{
  if (progresswindow)
    delete progresswindow;
}

void CheckCompareUsfms::message(unsigned int book, unsigned int chapter, const ustring & verse, const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}

void CheckCompareUsfms::comparebook(unsigned int book)
{
  vector < unsigned int >chapters = project_get_chapters(myproject, book);
  set < unsigned int >chapterset(chapters.begin(), chapters.end());
  vector < unsigned int >chapters2 = project_get_chapters(myproject2, book);
  set < unsigned int >chapter2set(chapters2.begin(), chapters2.end());
  // Check all chapters in this book, give message if chapter isn't there in 
  // the second project.
  for (unsigned int i = 0; i < chapters.size(); i++) {
    if (chapter2set.find(chapters[i]) != chapter2set.end()) {
      comparechapter(book, chapters[i]);
    } else {
      message(book, chapters[i], "0", "Extra chapter");
    }
  }
  // See if there are any chapters in the second project, which are not 
  // there in the first.
  for (unsigned int i = 0; i < chapters2.size(); i++) {
    if (chapterset.find(chapters2[i]) == chapterset.end())
      message(book, chapters2[i], "0", "Missing chapter");
  }
}

void CheckCompareUsfms::comparechapter(unsigned int book, unsigned int chapter)
{
  vector < ustring > verses = project_get_verses(myproject, book, chapter);
  set < ustring > verseset(verses.begin(), verses.end());
  vector < ustring > verses2 = project_get_verses(myproject2, book, chapter);
  set < ustring > verse2set(verses2.begin(), verses2.end());
  // Check all verses in this chapter, give message if a verse isn't there in 
  // the second project.
  for (unsigned int i = 0; i < verses.size(); i++) {
    if (verse2set.find(verses[i]) != verse2set.end()) {
      compareverse(book, chapter, verses[i]);
    } else {
      message(book, chapter, verses[i], "Extra verse");
    }
  }
  // See if there are any verses in the second project, which are not 
  // there in the first.
  for (unsigned int i = 0; i < verses.size(); i++) {
    if (verseset.find(verses2[i]) == verseset.end())
      message(book, chapter, verses2[i], "Missing verse");
  }
}

void CheckCompareUsfms::compareverse(unsigned int book, unsigned int chapter, const ustring & verse)
{
  // Get markers for the (first) project.
  ustring markers;
  {
    ustring text = project_retrieve_verse(myproject, book, chapter, verse);
    ustring marker = usfm_extract_marker_within_line(text);
    while (!marker.empty()) {
      storemarker(marker, markers, verse);
      marker = usfm_extract_marker_within_line(text);
    }
  }
  // Get markers for the second project.
  ustring markers2;
  {
    ustring text = project_retrieve_verse(myproject2, book, chapter, verse);
    ustring marker = usfm_extract_marker_within_line(text);
    while (!marker.empty()) {
      storemarker(marker, markers2, verse);
      marker = usfm_extract_marker_within_line(text);
    }
  }
  // Compare the two lots of markers.
  if (markers != markers2) {
    message(book, chapter, verse, "Markers differ: " + markers + " (" + markers2 + ")");
  }
}

void CheckCompareUsfms::storemarker(const ustring & marker, ustring & markers, const ustring & verse)
{
  // Ignore verse zero?
  if (myignoreverse0) {
    if (verse == "0")
      return;
  }
  // Check whether to include this marker.
  bool include = false;
  if (myincludeonly.empty())
    include = true;
  else
    include = myincludeonly.find(marker) != myincludeonly.end();
  if (myallmarkers)
    include = true;
  if (!myignore.empty()) {
    if (myignore.find(marker) != myignore.end())
      include = false;
  }
  if (!include)
    return;
  // Store the marker.
  if (!markers.empty())
    markers.append(" ");
  markers.append(marker);
}
