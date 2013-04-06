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

#include "check_count_words.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "checks.h"

CheckCountWords::CheckCountWords(const ustring & project, const vector < unsigned int >&books, const ustring & extrachars, bool sortword, bool sortcount, unsigned int excludecount, bool gui)
/*
It counts the words in the project.
project: project to check.
books: books to check; if empty it checks them all.
extrachars: characters that are part of a word.
sortword: sort on word.
sortcount: sort on count.
excludecount: exclude words with a count of n and up.
gui: show graphical progressbar.
*/
{
  // Init variables.
  cancelled = false;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Counting words", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Deal with extra word-forming characters.
  ustring extrachars_cleaned = get_non_alphanumericals(extrachars);
  for (unsigned int i = 0; i < extrachars_cleaned.length(); i++) {
    extra_character.push_back(extrachars_cleaned.substr(i, 1));
    ustring s = "BIBLEDITREPLACEMENT";
    for (unsigned int i2 = 0; i2 < i; i2++)
      s.append("X");
    temporal_replacement.push_back(s);
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
      // Handle extra characters.
      for (unsigned int i = 0; i < extra_character.size(); i++) {
        replace_text(lines[ln], extra_character[i], temporal_replacement[i]);
      }
      // Categorize the line for a cleaner results.
      CategorizeLine categorizeline(lines[ln]);
      // Count the words from all categories.
      ParseWords pwid(categorizeline.id);
      count(pwid.words);
      ParseWords pwintro(categorizeline.intro);
      count(pwintro.words);
      ParseWords pwhead(categorizeline.head);
      count(pwhead.words);
      ParseWords pwchap(categorizeline.chap);
      count(pwchap.words);
      ParseWords pwstudy(categorizeline.study);
      count(pwstudy.words);
      ParseWords pwnote(categorizeline.note);
      count(pwnote.words);
      ParseWords pwref(categorizeline.ref);
      count(pwref.words);
      ParseWords pwverse(categorizeline.verse);
      count(pwverse.words);
    }
  }
  // Store results.
  if (gui) {
    progresswindow->set_fraction(0.2);
    progresswindow->set_text("Processing results");
  }
  words.assign(wordset.begin(), wordset.end());
  for (unsigned int i = 0; i < words.size(); i++) {
    counts.push_back(counter[words[i]]);
  }
  // Change temporal replacements back to the original characters.
  if (gui)
    progresswindow->set_fraction(0.4);
  for (unsigned int i = 0; i < words.size(); i++) {
    for (int i2 = extra_character.size() - 1; i2 >= 0; i2--) {
      replace_text(words[i], temporal_replacement[i2], extra_character[i2]);
    }
  }
  // Store total counts.
  if (gui)
    progresswindow->set_fraction(0.6);
  total_unique_count = words.size();
  total_count = 0;
  for (unsigned int i = 0; i < counts.size(); i++) {
    total_count += counts[i];
  }
  // Exclude counts above a certain limit, if given.
  if (gui)
    progresswindow->set_fraction(0.8);
  if (excludecount > 0) {
    vector < ustring > mywords;
    vector < unsigned int >mycounts;
    for (unsigned int i = 0; i < words.size(); i++) {
      if (counts[i] < excludecount) {
        mywords.push_back(words[i]);
        mycounts.push_back(counts[i]);
      }
    }
    words.clear();
    words = mywords;
    counts.clear();
    counts = mycounts;
  }
  // Store filtered counts.
  filtered_unique_count = words.size();
  filtered_count = 0;
  for (unsigned int i = 0; i < counts.size(); i++) {
    filtered_count += counts[i];
  }
  // Sorting, if requested.
  if (gui)
    progresswindow->set_fraction(1);
  if (sortword) {
    quick_sort(words, counts, 0, words.size());
  }
  if (sortcount) {
    quick_sort(counts, words, 0, counts.size());
  }
}

CheckCountWords::~CheckCountWords()
{
  if (progresswindow)
    delete progresswindow;
}

void CheckCountWords::count(vector < ustring > &words)
{
  // Count each word.
  for (unsigned int i = 0; i < words.size(); i++) {
    wordset.insert(words[i]);
    counter[words[i]]++;
  }
}
