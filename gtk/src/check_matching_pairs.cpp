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

#include "check_matching_pairs.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "scripturechecks.h"
#include "tiny_utilities.h"
#include <glib/gi18n.h>

MatchingPairOpener::MatchingPairOpener(const ustring & char_in, gunichar unichar_in, unsigned int book_in, int chapter_in, const ustring & verse_in, const ustring & context_in)
{
  character = char_in;
  unichar = unichar_in;
  book = book_in;
  chapter = chapter_in;
  verse = verse_in;
  context = context_in;
}

CheckMatchingPairs::CheckMatchingPairs(const ustring & project, const vector < unsigned int >&books, const ustring & ignore, bool gui)
/*
It checks matching pairs of punctuation, e.g. the ( matches with the ).
project: project to check.
books: books to check; if empty it checks them all.
ignore: punctuation to ignore.
gui: whether to show graphical progressbar.
*/
{
  cancelled = false;
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);

  for (unsigned int i = 0; i < ignore.length(); i++) {
    gunichar unichar = g_utf8_get_char(ignore.substr(i, 1).c_str());
    ignores.insert(unichar);
    gunichar mirror;
    if (g_unichar_get_mirror_char(unichar, &mirror)) {
      ignores.insert(mirror);
    }
  }

  // Get list of openers and closers.
  for (gunichar i = 0; i < 1000000; i++) {
    gunichar mirror;
    if (g_unichar_get_mirror_char(i, &mirror)) {
      if (gclosers.find(i) == gclosers.end()) {
        gopeners.insert(i);
        gclosers.insert(mirror);
      }
    }
  }

  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow(_("Matching pairs"), true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }

  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    book = mybooks[bk];
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        break;
      }
    }

    vector < unsigned int >chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];
      vector < ustring > verses = project_get_verses(project, book, chapter);

      for (unsigned int vs = 0; vs < verses.size(); vs++) {
        verse = verses[vs];
        ustring line = project_retrieve_verse(project, book, chapter, verse);

        CategorizeLine categorize(line);
        check_matched_pairs(categorize.id);
        check_matched_pairs(categorize.intro);
        check_matched_pairs(categorize.head);
        check_matched_pairs(categorize.chap);
        check_matched_pairs(categorize.study);
        check_matched_pairs(categorize.note);
        check_matched_pairs(categorize.ref);
        check_matched_pairs(categorize.verse);
      }
      // At the end of each chapter, check whether all pairs are "clean"
      check_pairs_clean();
    }
  }
}

CheckMatchingPairs::~CheckMatchingPairs()
{
  if (progresswindow)
    delete progresswindow;
}

ustring CheckMatchingPairs::get_context(ustring & line, unsigned int offset)
// Returns the context at offset: A couple of words before and after.
{
  // Result.
  ustring returnvalue;
  // Load text into buffer.
  GtkTextBuffer *textbuffer;
  textbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(textbuffer, line.c_str(), -1);
  // Iterators.  
  GtkTextIter iter1;
  GtkTextIter iter2;
  // Find boundaries of context to return.
  gtk_text_buffer_get_iter_at_offset(textbuffer, &iter1, offset);
  iter2 = iter1;
  gtk_text_iter_backward_word_starts(&iter1, 2);
  gtk_text_iter_forward_word_ends(&iter2, 2);
  return gtk_text_iter_get_text(&iter1, &iter2);
  // Free memory
  g_object_unref(textbuffer);
  // Give us the result.
  return returnvalue;
}

void CheckMatchingPairs::check_matched_pairs(ustring & text)
// Checks on matched pairs. Output any problems found.
{
  for (unsigned int i = 0; i < text.length(); i++) {
    // Get the unicode character;
    gunichar unichar;
    unichar = g_utf8_get_char(text.substr(i, 1).c_str());
    // If we found a mirror character, investigate further.
    gunichar mirror;
    if (g_unichar_get_mirror_char(unichar, &mirror)) {
      // Do we ignore this one?
      if (ignores.find(unichar) != ignores.end())
        continue;
      // See whether this one opens or closes a pair.
      if (gopeners.find(unichar) != gopeners.end()) {
        // It opens: Add data.
        MatchingPairOpener opener(text.substr(i, 1), unichar, book, chapter, verse, get_context(text, i));
        openers.push_back(opener);
        continue;
      } else {
        // It closes: check for previously seen opener.
        bool give_message = false;
        if (openers.empty()) {
          give_message = true;
        }
        if (!give_message) {
          if (openers[openers.size() - 1].unichar == mirror) {
            // Remove last one.
            openers.pop_back();
          } else {
            // Flag message.
            give_message = true;
          }
        }
        if (give_message) {
          // Give message;
          message(book, chapter, verse, _("Pair not opened: ") + get_context(text, i));
        }
      }
    }
  }
}

void CheckMatchingPairs::check_pairs_clean()
// See if there is still any opening punctuation that have not been matched with 
// closing punctuation.
{
  // Check for them and give messages.
  for (unsigned int i = 0; i < openers.size(); i++) {
    message(openers[i].book, openers[i].chapter, openers[i].verse, _("Pair not closed: ") + openers[i].context);
  }
  // Clear them up.
  openers.clear();
}

void CheckMatchingPairs::message(unsigned int book, unsigned int chapter, const ustring & verse, const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}
