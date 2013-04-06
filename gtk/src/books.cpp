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

#include "books.h"
#include "directories.h"
#include "gwrappers.h"
#include "utilities.h"
#include "shell.h"
#include "settings.h"
#include "localizedbooks.h"

extern typeof(book_record) books_table[];

void books_order(const ustring & project, vector < unsigned int >&books)
// Books read from the database usually are out of order. 
// Reorder the books in agreement with the user's settings.
// When Bibledit got new defined books, there will be books not yet in the user's
// settings. These will be reordered depending on their relative position with
// the existing books.
{
  // Configuration
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  vector < int >project_books = projectconfig->book_order_get();

  // Sort part or all of the books following the project.
  vector < unsigned int >books_sorted;
  {
    set < unsigned int >books_sorted_inserted;
    set < unsigned int >bookset(books.begin(), books.end());
    for (unsigned int i = 0; i < project_books.size(); i++) {
      if (bookset.find(project_books[i]) != bookset.end()) {
        if (books_sorted_inserted.find(project_books[i]) == books_sorted_inserted.end()) {
          books_sorted.push_back(project_books[i]);
          books_sorted_inserted.insert(project_books[i]);
        }
      }
    }
  }

  // Give a sequence to each book already sorted.
  vector < unsigned int >books_sorted_sequences;
  for (unsigned int i = 0; i < books_sorted.size(); i++) {
    unsigned int sequence;
    for (unsigned int i2 = 0; i2 < bookdata_books_count(); i2++) {
      if (books_sorted[i] == books_table[i2].id) {
        sequence = i2;
        break;
      }
    }
    books_sorted_sequences.push_back(sequence);
  }

  // Store any books still left out.
  vector < unsigned int >books_left;
  {
    set < unsigned int >bookset(books_sorted.begin(), books_sorted.end());
    for (unsigned int i = 0; i < books.size(); i++) {
      if (bookset.find(books[i]) == bookset.end()) {
        books_left.push_back(books[i]);
      }
    }
  }

  // Give a sequence to each book left out.
  vector < unsigned int >books_left_sequences;
  for (unsigned int i = 0; i < books_left.size(); i++) {
    unsigned int sequence;
    for (unsigned int i2 = 0; i2 < bookdata_books_count(); i2++) {
      if (books_left[i] == books_table[i2].id) {
        sequence = i2;
        break;
      }
    }
    books_left_sequences.push_back(sequence);
  }

  // If we had no books, just add the ones left out, after sorting them.
  if (books_sorted.empty()) {
    books.clear();
    quick_sort(books_left_sequences, books_left, 0, books_left_sequences.size());
    for (unsigned int i = 0; i < books_left.size(); i++) {
      books.push_back(books_left[i]);
    }
    return;
  }
  // Depending on the sequence of each book left out, insert it at the right 
  // location in the already sorted books.
  // The algorithm is that we check to which book each of them comes nearest,
  // and the insert it before of after that location, depending on whether it 
  // follows or precedes it.
  for (unsigned int i = 0; i < books_left.size(); i++) {
    unsigned int smallest_absolute_distance = 1000;
    size_t insert_location = 1000;
    unsigned int sequence_left = books_left_sequences[i];
    for (unsigned int i2 = 0; i2 < books_sorted.size(); i2++) {
      unsigned int sequence_sorted = books_sorted_sequences[i2];
      unsigned int absolute_distance = ABS(sequence_sorted - sequence_left);
      if (absolute_distance < smallest_absolute_distance) {
        smallest_absolute_distance = absolute_distance;
        insert_location = i2;
      }
    }
    if (sequence_left > books_sorted_sequences[insert_location])
      insert_location++;
    if (insert_location >= books_sorted.size()) {
      books_sorted.push_back(books_left[i]);
      books_sorted_sequences.push_back(books_left_sequences[i]);
    } else {
      vector < unsigned int >::iterator book_iter = books_sorted.begin();
      vector < unsigned int >::iterator sequence_iter = books_sorted_sequences.begin();
      for (size_t i2 = 0; i2 < insert_location; i2++) {
        book_iter++;
        sequence_iter++;
      }
      books_sorted.insert(book_iter, books_left[i]);
      books_sorted_sequences.insert(sequence_iter, books_left_sequences[i]);
    }
  }

  // Store the books already sorted, with the possible ones left out, in the
  // books variable.
  books.clear();
  for (unsigned int i = 0; i < books_sorted.size(); i++) {
    books.push_back(books_sorted[i]);
  }
}

void books_standard_order(vector < unsigned int >&books)
// This orders the books into the standard order.
{
  set < unsigned int >books_set(books.begin(), books.end());
  books.clear();
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    if (books_set.find(books_table[i].id) != books_set.end())
      books.push_back(books_table[i].id);
  }
}

unsigned int books_name_to_id(const ustring & language, const ustring & book)
/*
This returns the id of "book" in "language". 

The id is Bibledit's internal id for a given book. 
This id uniquely identifies the book. 

We could have identified books by the Paratext ID, or by the OSIS name, but as 
there aren't Paratext IDs or OSIS names for every book that Bibledit supports, 
we need to have our own id.
*/
{
  extern BookLocalizations *booklocalizations;
  return booklocalizations->name2id(language, book);
}

unsigned int books_abbreviation_to_id(const ustring & language, const ustring & abbreviation)
// This returns the id of "abbreviation" in "language". 
{
  extern BookLocalizations *booklocalizations;
  return booklocalizations->abbrev2id(language, abbreviation);
}

unsigned int books_abbreviation_to_id_loose(const ustring & language, const ustring & abbreviation)
// This returns the id of "abbreviation" in "language". 
// It uses a looser searching algorithm.
{
  extern BookLocalizations *booklocalizations;
  return booklocalizations->abbrev2id_loose(language, abbreviation);
}

ustring books_id_to_name(const ustring & language, unsigned int id)
// Returns the language's human readable bookname from the Bibledit id.
{
  extern BookLocalizations *booklocalizations;
  return booklocalizations->id2name(language, id);
}

ustring books_id_to_abbreviation(const ustring & language, unsigned int id)
// Returns the language's abbreviation of the Bibledit id.
{
  extern BookLocalizations *booklocalizations;
  return booklocalizations->id2abbrev(language, id);
}

ustring books_id_to_paratext(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].paratext;
  return "";
}

unsigned int books_paratext_to_id(const ustring & paratext)
{
  ustring s1(paratext.casefold());
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring s2(books_table[i].paratext);
    s2 = s2.casefold();
    if (s1 == s2) {
      return books_table[i].id;
    }
  }
  return 0;
}

ustring books_id_to_bibleworks(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].bibleworks;
  return "";
}

unsigned int books_bibleworks_to_id(const ustring & bibleworks)
{
  ustring s1(bibleworks.casefold());
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring s2(books_table[i].bibleworks);
    s2 = s2.casefold();
    if (s1 == s2) {
      return books_table[i].id;
    }
  }
  return 0;
}

ustring books_id_to_osis(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].osis;
  return "";
}

unsigned int books_osis_to_id(const ustring & osis)
{
  ustring s1(osis.casefold());
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring s2(books_table[i].osis);
    s2 = s2.casefold();
    if (s1 == s2) {
      return books_table[i].id;
    }
  }
  return 0;
}

ustring books_id_to_english(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].name;
  return "";
}

unsigned int books_english_to_id(const ustring & english)
{
  ustring s1(english.casefold());
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring s2(books_table[i].name);
    s2 = s2.casefold();
    if (s1 == s2) {
      return books_table[i].id;
    }
  }
  return 0;
}

ustring books_id_to_online_bible(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].onlinebible;
  return "";
}

unsigned int books_online_bible_to_id(const ustring & onlinebible)
{
  ustring s1(onlinebible.casefold());
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring s2(books_table[i].onlinebible);
    s2 = s2.casefold();
    if (s1 == s2) {
      return books_table[i].id;
    }
  }
  return 0;
}

BookType books_id_to_type(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].type;
  return btUnknown;
}

vector < unsigned int >books_type_to_ids(BookType type)
// Gives the book ids of a certain type. 
// If the unknown type is given, it means "doesn't matter", so it gives all ids.
{
  vector < unsigned int >ids;
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    if ((type == books_table[i].type) || (type == btUnknown)) {
      ids.push_back(books_table[i].id);
    }
  }
  return ids;
}

bool books_id_to_one_chapter(unsigned int id)
// Gives whether this is a book with one chapter.
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].onechapter;
  return false;
}

/*
There are still many functions around that employ books_id_to_english or
books_english_to_id. Check them all, and see if they can be modified.
*/
