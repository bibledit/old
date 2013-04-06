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

#include "check_capitalization.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "scripturechecks.h"
#include "referenceutils.h"
#include "tiny_utilities.h"

CheckCapitalization::CheckCapitalization(const ustring & project, const vector < unsigned int >&books, const ustring & punctuation_followed_by_capitals, const ustring & ignore_lowercase_following, const ustring & abbreviations_filename, bool any_prefixes, const ustring & uncapitalized_prefixes_filename, const ustring & capitalized_suffixes_filename, bool gui)
/*
It checks the capitalization in the text.
project: project to check.
books: books to check; if empty it checks them all.
punctuation_followed_by_capitals: give punctuation that may be followed by capitals.
ignore_lowercase_following: ignore the error if a lower case character follows these characters.
abbreviations_filename: filename with the abbreviations.
any_prefixes: allow any prefixes before the capitals.
uncapitalized_prefixes_filename: filename with the list of uncapitalized prefixes.
capitalized_suffixes_filename: filename with the list of the capitalized suffixes.
gui: whether to show graphical progressbar.
*/
{
  cancelled = false;
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);

  for (unsigned int i = 0; i < punctuation_followed_by_capitals.length(); i++) {
    punctuation_followed_by_capitals_set.insert(g_utf8_get_char(punctuation_followed_by_capitals.substr(i, 1).c_str()));
  }

  for (unsigned int i = 0; i < ignore_lowercase_following.length(); i++) {
    ignore_lower_case_following_set.insert(g_utf8_get_char(ignore_lowercase_following.substr(i, 1).c_str()));
  }

  {
    ReadText rt(abbreviations_filename, true);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      abbreviations.insert(rt.lines[i]);
    }
  }

  allow_any_uncapitalized_prefixes = any_prefixes;
  {
    ReadText rt(uncapitalized_prefixes_filename, true);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      uncapitalized_prefixes.insert(rt.lines[i]);
    }
  }

  {
    ReadText rt(capitalized_suffixes_filename, true);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      capitalized_suffixes.insert(rt.lines[i]);
    }
  }

  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Capitalization", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }

  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    book = mybooks[bk];

    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }

    vector < unsigned int >chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];
      vector < ustring > verses = project_get_verses(project, book, chapter);

      for (unsigned int vs = 0; vs < verses.size(); vs++) {
        verse = verses[vs];

        vector < int >mychapters;
        vector < ustring > myverses;
        vector < size_t > mypointers;
        mychapters.push_back(chapters[ch]);
        myverses.push_back(verses[vs]);
        mypointers.push_back(0);

        ustring line = project_retrieve_verse(project, book, chapter, verse);
        CategorizeLine categorize(line);
        // No checks done on id.
        // Introduction text.
        check_capitalization(mychapters, myverses, categorize.intro, mypointers, false);
        check_suspicious_capitalization(categorize.intro);
        // Heading text.
        check_capitalization(mychapters, myverses, categorize.head, mypointers, false);
        check_suspicious_capitalization(categorize.head);
        // No checks done on chapter.
        // Study notes.
        check_capitalization(mychapters, myverses, categorize.study, mypointers, true);
        check_suspicious_capitalization(categorize.study);
        // Foot- and endnotes.
        check_capitalization(mychapters, myverses, categorize.note, mypointers, true);
        check_suspicious_capitalization(categorize.note);
        // Crossreferences.
        check_capitalization(mychapters, myverses, categorize.ref, mypointers, true);
        check_suspicious_capitalization(categorize.ref);
        // Store verse text for checking at the end of the book. We cannot check 
        // per verse or chapter, because sentences could span them.
        verse_chapter.push_back(chapter);
        verse_verse.push_back(verse);
        verse_pointer.push_back(verse_text.length());
        if (!verse_text.empty())
          verse_text.append(" ");
        verse_text.append(categorize.verse);
        // Check suspicious capitalization in the versetext.
        check_suspicious_capitalization(categorize.verse);
      }
    }

    // We've reached the end of the book. Check all verse text.
    check_capitalization(verse_chapter, verse_verse, verse_text, verse_pointer, true);
    // Clear containers for verse text.
    verse_chapter.clear();
    verse_verse.clear();
    verse_text.clear();
    verse_pointer.clear();

  }
}

CheckCapitalization::~CheckCapitalization()
{
  if (progresswindow)
    delete progresswindow;
}

void CheckCapitalization::check_capitalization(vector < int >&chapters, vector < ustring > &verses, ustring & text, vector < size_t > &pointers, bool end_check)
/*
Check capitalization in text.
If "end_check" is true, it also check for final sentence closing.
*/
{
  /*
     Note that this at first used gtk_text_iter_starts_sentence (&iter) and
     gtk_text_iter_ends_sentence (&iter), but these functions are not good enough,
     because do not work in several cases, like e.g. in the following line, it does
     not indicate the end of the sentence:
     As soon as the leaders of the tribes of Israel took their places, the 
     Israelites said, “How could such a horrible thing happen?"
     Therefore we use other means to check sentences.
   */

  // No check if there's no text.
  if (trim(text).empty())
    return;
  // Some variables needed.
  bool expect_capital_now = false;
  bool expect_capital_caused_by_reference = false;
  gunichar previous_char = 0;
  int localchapter = 0;
  ustring localverse = "0";
  GtkTextBuffer *textbuffer;
  textbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(textbuffer, text.c_str(), -1);
  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(textbuffer, &iter);
  bool going = true;
  while (going) {
    // Get the unicode character.
    gunichar unichar = gtk_text_iter_get_char(&iter);
    // See whether to expect a capital now.
    if (punctuation_followed_by_capitals_set.find(unichar) != punctuation_followed_by_capitals_set.end()) {
      // Ok, expect capital.
      expect_capital_now = true;
      expect_capital_caused_by_reference = false;
      // Was this expectation caused by a reference?
      if (is_reference(iter))
        expect_capital_caused_by_reference = true;
    }
    // If we expect a capital, and we find one, no longer look for one.
    if (expect_capital_now) {
      if (g_unichar_isupper(unichar)) {
        expect_capital_now = false;
      }
    }
    // If we expect a capital, and we get lower case, that might be trouble.
    if (expect_capital_now) {
      if (g_unichar_islower(unichar)) {
        // There is no trouble if it follows a character after which to ignore lower case.
        if (ignore_lower_case_following_set.find(previous_char) != ignore_lower_case_following_set.end()) {
          expect_capital_now = false;
        }
        // If the lowercase character follows an abbreviation, there is no trouble either.
        GtkTextIter iter2 = iter;
        gtk_text_iter_backward_word_start(&iter2);
        GtkTextIter iter3 = iter2;
        gtk_text_iter_forward_word_end(&iter3);
        gtk_text_iter_forward_char(&iter3);
        ustring abbreviation = gtk_text_iter_get_text(&iter2, &iter3);
        if (abbreviations.find(abbreviation) != abbreviations.end()) {
          expect_capital_now = false;
        }
        // If it follows a reference, there is no trouble.
        if (expect_capital_caused_by_reference)
          expect_capital_now = false;
        // Ok, give message.
        if (expect_capital_now) {
          // Determine chapter and verse.
          get_chapter_and_verse(chapters, verses, pointers, iter, localchapter, localverse);
          message(book, localchapter, localverse, "Capital expected: " + get_context(iter));
        }
        // Only give one message about missing capitals in this context.
        expect_capital_now = false;
      }
    }
    // Store this characters as the previous characters for the next round.
    if (g_unichar_isgraph(unichar))
      previous_char = unichar;
    // Next round.
    going = gtk_text_iter_forward_char(&iter);
  }
  // The sentence should be ended with proper punctuation.
  if (end_check) {
    if (expect_capital_now)
      if (g_unichar_isdigit(previous_char))
        expect_capital_now = false;
    if (!expect_capital_now) {
      message(book, chapter, verse, "Unended sentence: " + get_context(iter));
    }
  }
  // Free memory
  g_object_unref(textbuffer);
}

void CheckCapitalization::message(unsigned int book, unsigned int chapter, const ustring & verse, const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}

bool CheckCapitalization::is_reference(GtkTextIter iter)
/*
Looks whether the text at iter looks like a reference.
A reference, e.g. Mt.5.5 or Mt.5:5 or John 10:5 follows a certain pattern,
while going through it. Some references are given without the bookname, e.g.
"10.5". Handle these too.
Patterns:
- digit, dot/colon, digit.
*/
{
  GtkTextIter iter0 = iter;
  gtk_text_iter_backward_chars(&iter0, 4);
  gtk_text_iter_forward_chars(&iter, 4);
  ustring reference = gtk_text_iter_get_text(&iter0, &iter);
  return text_contains_reference(reference);
}

void CheckCapitalization::get_chapter_and_verse(vector < int >&chapters, vector < ustring > &verses, vector < size_t > &pointers, GtkTextIter iter, int &chapter, ustring & verse)
// Based on the inputs (chapters, verses, pointers, iter),
// it gets the chapter and verse we are now at.
{
  size_t offset = gtk_text_iter_get_offset(&iter);
  for (unsigned int i = pointers.size() - 1; i >= 0; i--) {
    if (offset >= pointers[i]) {
      chapter = chapters[i];
      verse = verses[i];
      return;
    }
  }
}

ustring CheckCapitalization::get_context(GtkTextIter iter)
// Returns the context at iter: A couple of words before and after.
{
  GtkTextIter iter1 = iter;
  GtkTextIter iter2 = iter;
  gtk_text_iter_backward_word_starts(&iter1, 2);
  gtk_text_iter_forward_word_ends(&iter2, 2);
  return gtk_text_iter_get_text(&iter1, &iter2);
}

void CheckCapitalization::mixed_capitalization_message(ustring & word)
{
  message(book, chapter, verse, "Mixed capitalization: " + word);
}

void CheckCapitalization::check_suspicious_capitalization(ustring & text)
/*
Checks on suspicious capitalization, like "bOat" or "BOat".
There are exceptions to this check.
*/
{
  // Load text into buffer.
  ustring text2(text);
  text2.append(" ");
  GtkTextBuffer *textbuffer;
  textbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(textbuffer, text2.c_str(), -1);
  // Iterators.  
  GtkTextIter startiter, enditer;
  // Check all separate words.
  gtk_text_buffer_get_start_iter(textbuffer, &enditer);
  while (gtk_text_iter_forward_word_end(&enditer)) {
    startiter = enditer;
    gtk_text_iter_backward_word_start(&startiter);
    vector < bool > capspattern;
    unsigned int capscount = 0;
    GtkTextIter iter = startiter;
    while (gtk_text_iter_in_range(&iter, &startiter, &enditer)) {
      bool upper = g_unichar_isupper(gtk_text_iter_get_char(&iter));
      capspattern.push_back(upper);
      if (upper)
        capscount++;
      gtk_text_iter_forward_char(&iter);
    }
    // No further checking if words are too short.
    if (capspattern.size() < 2)
      continue;
    // No further checking if only small letters.
    if (capscount == 0)
      continue;
    // No further checking if all capitals.
    if (capscount == capspattern.size())
      continue;
    // No further checking if first letter capitalized only.
    if ((capspattern[0]) && (capscount == 1))
      continue;
    // Ok, there could be a problem of mixed capitalization.
    // Get the prefix before the first capital, and the suffix after it.
    ustring word = gtk_text_iter_get_text(&startiter, &enditer);
    ustring uncapitalized_prefix;
    ustring capitalized_suffix;
    for (unsigned int i = 1; i < capspattern.size(); i++) {
      if (capspattern[i]) {
        uncapitalized_prefix = word.substr(0, i);
        capitalized_suffix = word.substr(i, word.length() - i);
        break;
      }
    }
    // See whether the suffix is properly capitalized.
    unsigned int suffix_capital_count = 0;
    for (unsigned int i = 0; i < capitalized_suffix.length(); i++) {
      if (g_unichar_isupper(g_utf8_get_char(capitalized_suffix.substr(i, 1).c_str())))
        suffix_capital_count++;
    }
    bool suffix_properly_capitalized = false;
    if (suffix_capital_count == 1)
      suffix_properly_capitalized = true;
    if (suffix_capital_count == capitalized_suffix.length())
      suffix_properly_capitalized = true;
    // Give message and continue if capitalization error in suffix, but only
    // if this so-called wrongly capitalized suffix has not been approved af.
    if (!suffix_properly_capitalized) {
      if (capitalized_suffixes.find(capitalized_suffix) == capitalized_suffixes.end()) {
        mixed_capitalization_message(word);
        continue;
      }
    }
    // No further checking if this uncapitalized prefix is in the list,
    // or any is allowed.
    if (uncapitalized_prefixes.find(uncapitalized_prefix) != uncapitalized_prefixes.end())
      continue;
    if (allow_any_uncapitalized_prefixes)
      continue;
    // Ok, not in the list. Try again with lower case initial.
    ustring initial = uncapitalized_prefix.substr(0, 1);
    initial = initial.casefold();
    uncapitalized_prefix.replace(0, 1, initial);
    if (uncapitalized_prefixes.find(uncapitalized_prefix) != uncapitalized_prefixes.end())
      continue;
    // No further checking if the suffix is in the list of approved suffixes.
    if (capitalized_suffixes.find(capitalized_suffix) != capitalized_suffixes.end())
      continue;
    // Ok, not found, but it could be this suffix is in all capitals. Handle that.
    initial = capitalized_suffix.substr(0, 1);
    capitalized_suffix.erase(0, 1);
    capitalized_suffix = capitalized_suffix.casefold();
    capitalized_suffix.insert(0, initial);
    if (capitalized_suffixes.find(capitalized_suffix) != capitalized_suffixes.end())
      continue;
    // Ok, it appears we've got an error here -> give message.
    mixed_capitalization_message(word);
  }
  // Free memory
  g_object_unref(textbuffer);
}
