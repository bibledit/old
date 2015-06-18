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

#include "check_chapters_verses.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "checks.h"
#include "versification.h"
#include "tiny_utilities.h"
#include <glib/gi18n.h>

CheckChaptersVerses::CheckChaptersVerses(const ustring & project, const vector < unsigned int >&books, bool gui)
/*
It checks the number of chapters per book and the number of verses per chapter.
project: project to check.
books: books to check; if empty it checks them all.
gui: show graphical progressbar.
*/
{
  cancelled = false;
  myproject = project;
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  myversification = projectconfig->versification_get();
  // If no books given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow(_("Checking chapters and verses"), true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      progresswindow->set_text(books_id_to_english(mybooks[bk]));
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    first_chapter_found = false;
    vector < unsigned int >chapters = project_get_chapters(project, mybooks[bk]);
    highest_chapter_get(mybooks[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      new_chapter_check(mybooks[bk], chapters[ch]);
      vector < ustring > verses;
      verses = project_get_verses(project, mybooks[bk], chapters[ch]);
      highest_verse_get(mybooks[bk], chapters[ch]);
      verses_check(mybooks[bk], chapters[ch], verses);
    }
    last_chapter_check(mybooks[bk], chapters);
  }
}

CheckChaptersVerses::~CheckChaptersVerses()
{
  if (progresswindow)
    delete progresswindow;
}

void CheckChaptersVerses::highest_chapter_get(unsigned int book)
// Get highest chapter of the book, according to the versification.
{
  highest_chapter = 0;
  vector < unsigned int >chapters = versification_get_chapters(myversification, book);
  if (!chapters.empty())
    highest_chapter = chapters[chapters.size() - 1];
}

void CheckChaptersVerses::highest_verse_get(unsigned int book, unsigned int chapter)
// Get highest verse of the chapter in the book, according to the versification.
{
  highest_verse = convert_to_int(versification_get_last_verse(myversification, book, chapter));
  if (chapter == 0)
    highest_verse = 0;
}

void CheckChaptersVerses::new_chapter_check(unsigned int book, unsigned int chapter)
{
  // Deal with the first chapter in a book.
  if (!first_chapter_found) {
    if (chapter > 1)
      message(book, chapter, "0", "First chapter number is too high");
    previous_chapter = chapter - 1;
    first_chapter_found = true;
  }
  // Check whether the chapter number follows the previous one.
  if ((int)chapter != (previous_chapter + 1)) {
    ustring msg = _("Chapter out of sequence following ") + convert_to_string(int (previous_chapter));
    message(book, chapter, "0", msg);
  }
  previous_chapter = chapter;

  // Check whether the number is within limits.
  if (chapter > highest_chapter)
    message(book, chapter, "0", _("Extra chapter or wrong versification"));
}

void CheckChaptersVerses::last_chapter_check(unsigned int book, vector < unsigned int >&chapters)
// Check whether we have enough chapters in this book.
{
  unsigned int lastchapter = 0;
  if (!chapters.empty())
    lastchapter = chapters[chapters.size() - 1];
  if (lastchapter < highest_chapter)
    message(book, lastchapter, "0", _("Less than ") + convert_to_string(highest_chapter) + _(" chapters or wrong versification"));
}

void CheckChaptersVerses::verses_check(unsigned int book, unsigned int chapter, const vector < ustring > &verses)
// This checks all the verses that are in 'book' and 'chapter'.
// It supports sequences in the form of \v 1,2,3, 
// and ranges in the form of \v 1b-3 and \v 2-4a and \v 2b-5a.
{
  // Check whether there are any verses at all. If not, stop further 
  // processing because the following code assumes there are verses.
  if (verses.empty()) {
    message(book, chapter, "1", _("Chapter has no verses"));
    return;
  }
  // Check for verses in chapter 0, which indicates the \c 1 marker wasn't there.
  if ((chapter == 0) && (verses.size() > 1)) {
    message(book, chapter, "1", _("Chapter marker missing"));
  }
  // Transform the verses in the internally used encoding, so as to accomodate
  // for sequences and ranges.
  vector < unsigned int >expanded_verses;
  vector < unsigned int >verses_pointers;
  for (unsigned int i = 0; i < verses.size(); i++) {
    // Do not work in the container, but on a copy.
    ustring vs(verses[i]);
    // If there is a range, take the beginning and the end and fill up in between.
    if (vs.find("-") != string::npos) {
      size_t position;
      position = vs.find("-");
      ustring start_range, end_range;
      start_range = vs.substr(0, position);
      vs.erase(0, ++position);
      end_range = vs;
      int start_expanded_verse = 2 * convert_to_int(number_in_string(start_range));
      if (start_range.find("b") != string::npos)
        start_expanded_verse++;
      // Checking on range start.
      if (start_range.find("a") != string::npos)
        message(book, chapter, verses[i], _("Range starts with \"a\""));
      int end_expanded_verse = 2 * convert_to_int(number_in_string(end_range));
      if (end_range.find("a") == string::npos)
        end_expanded_verse++;
      // Check on range end.
      if (end_range.find("b") != string::npos)
        message(book, chapter, verses[i], _("Range ends with \"b\""));
      for (int i2 = start_expanded_verse; i2 <= end_expanded_verse; i2++) {
        expanded_verses.push_back(i2);
        verses_pointers.push_back(i);
      }
    }
    // If there is a sequence, take each verse in the sequence, and store it.
    else if (vs.find(",") != string::npos) {
      int iterations = 0;
      do {
        // In case of an unusual range formation, do not hang, but give message.
        iterations++;
        if (iterations > 50) {
          message(book, chapter, verses[i], _("Unusual verse sequence"));
          break;
        }
        size_t position = vs.find(",");
        ustring verse;
        if (position == string::npos) {
          verse = vs;
          vs.clear();
        } else {
          verse = vs.substr(0, position);
          vs.erase(0, ++position);
        }
        store_expanded_verse(verse, i, expanded_verses, verses_pointers);
      } while (!vs.empty());
    }
    // No range and no sequence: a "normal" verse.
    else {
      store_expanded_verse(vs, i, expanded_verses, verses_pointers);
    }
  }

  // See whether it starts at verse 0 or 1.
  if (expanded_verses[0] > 2)
    message(book, chapter, verses[0], "Verse 1 missing");

  // See whether the verses are within the limit.
  for (unsigned int i = 0; i < expanded_verses.size(); i += 2) {
    unsigned int verse;
    verse = expanded_verses[i] / 2;
    if (verse > highest_verse) {
      message(book, chapter, convert_to_string(verse), _("Extra verse or wrong versification"));
    }
  }

  // See whether there are verses or bits out of sequence.
  int previous_verse = expanded_verses[0] - 1;
  for (unsigned int i = 0; i < expanded_verses.size(); i++) {
    ustring previous_verse_text;
    unsigned int pointer = verses_pointers[i];
    if (pointer == 0)
      previous_verse_text = _("beginning");
    else
      previous_verse_text = verses[pointer - 1];
    if ((int)expanded_verses[i] != previous_verse + 1) {
      message(book, chapter, verses[pointer], _("Verse out of sequence following ") + previous_verse_text);
    }
    previous_verse = expanded_verses[i];
  }

  // Check whether we have enough verses.
  int highverse = expanded_verses[expanded_verses.size() - 1];
  highverse = highverse / 2;
  if (highverse < (int)highest_verse) {
    message(book, chapter, verses[verses.size() - 1], _("Not enough verses in chapter"));
  }
  // Clear storage.
  expanded_verses.clear();
  verses_pointers.clear();
}

void CheckChaptersVerses::message(unsigned int book, unsigned int chapter, const ustring & verse, const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}

void CheckChaptersVerses::store_expanded_verse(const ustring & verse, unsigned int verses_pointer, vector < unsigned int >&expanded_verses, vector < unsigned int >&verses_pointers)
{
  int expanded_verse;
  expanded_verse = 2 * (convert_to_int(verse));
  if (verse.find("b") == string::npos) {
    expanded_verses.push_back(expanded_verse);
    verses_pointers.push_back(verses_pointer);
  }
  if (verse.find("a") == string::npos) {
    expanded_verses.push_back(++expanded_verse);
    verses_pointers.push_back(verses_pointer);
  }
}
