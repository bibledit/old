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

#include "check_repetition.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "scripturechecks.h"
#include "tiny_utilities.h"

CheckRepetition::CheckRepetition(const ustring & project, const vector < unsigned int >&books, bool ignorecase, const ustring & only_these, const ustring & ignore_these, bool gui)
/*
It checks repeating whole words in the text.
project: project to check.
books: books to check; if empty it checks them all.
ignorecase: whether to ignore the case.
only_these: if given, filename of the words to only include.
ignore_these: if given, filename of the words to ignore.
gui: whether to show graphical progressbar.
*/
{
  // Variables.
  cancelled = false;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // Read possible words to only include, or to exclude.
  set < ustring > onlythese;
  if (!only_these.empty()) {
    ReadText rt(only_these, true, false);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      if (ignorecase)
        rt.lines[i] = rt.lines[i].casefold();
      onlythese.insert(rt.lines[i]);
    }
  }
  set < ustring > ignorethese_set;
  vector < ustring > ignorethese_list;
  if (!ignore_these.empty()) {
    ReadText rt(ignore_these, true, false);
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      if (ignorecase)
        rt.lines[i] = rt.lines[i].casefold();
      ignorethese_set.insert(rt.lines[i]);
      ignorethese_list.push_back(rt.lines[i]);
    }
    ignorethese_list = rt.lines;
  }
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Repeating words", true);
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
        ParseWords parsewords(text);
        ustring previousword;
        for (unsigned int i = 0; i < parsewords.words.size(); i++) {
          ustring word = parsewords.words[i];
          if (ignorecase)
            word = word.casefold();
          if (word == previousword) {
            bool print = true;
            if (!onlythese.empty()) {
              if (onlythese.find(word) == onlythese.end())
                print = false;
            }
            if (print) {
              if (!ignorethese_set.empty())
                if (ignorethese_set.find(word) != ignorethese_set.end())
                  print = false;
            }
            // Deal with certain texts between repeating words, so that if these texts
            // are found in between, this repetition is ignored.
            if (print) {
              if (ignorecase)
                text = text.casefold();
              for (unsigned int i2 = 0; i2 < ignorethese_list.size(); i2++) {
                ustring assembly = word + ignorethese_list[i2] + word;
                if (text.find(assembly) != string::npos) {
                  print = false;
                  continue;
                }
              }
            }
            if (print) {
              ustring message = "Repeated: ";
              message.append(word);
              references.push_back(books_id_to_english(mybooks[bk]) + " " + convert_to_string(chapters[ch]) + ":" + verses[vs]);
              comments.push_back(message);
            }
          }
          previousword = word;
        }
      }
    }
  }
}

CheckRepetition::~CheckRepetition()
{
  if (progresswindow)
    delete progresswindow;
}
