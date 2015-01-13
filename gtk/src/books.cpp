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

//extern typeof(book_record) books_table[];
extern book_record books_table[];

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

// Could be much more efficient if the id was the index into the books_table[]
ustring books_id_to_english(unsigned int id)
{
  for (unsigned int i = 0; i < bookdata_books_count(); i++)
    if (id == books_table[i].id)
      return books_table[i].name;
  return "";
}

// This requires the compiler flag -std=c++11. This, in turn, requires
// the ax_cxx_compile_stdcxx11.m4 script to make configure.ac know how
// to do the right thing.  On Ubuntu, I had to install the
// autoconf-archive package to get this macro. Then run 
// aclocal; autoconf; ./configure
// Matt Postiff 1/12/2015
#include <unordered_map>
std::unordered_map<std::string, int> bookmap;

void books_init(void)
{
  bookmap["front matter"]=67;
  bookmap["genesis"]=1;
  bookmap["exodus"]=2;
  bookmap["leviticus"]=3;
  bookmap["numbers"]=4;
  bookmap["deuteronomy"]=5;
  bookmap["joshua"]=6;
  bookmap["judges"]=7;
  bookmap["ruth"]=8;
  bookmap["1 samuel"]=9;
  bookmap["2 samuel"]=10;
 bookmap["1 kings"]=11;
 bookmap["2 kings"]=12;
 bookmap["1 chronicles"]=13;
 bookmap["2 chronicles"]=14;
 bookmap["ezra"]=15;
 bookmap["nehemiah"]=16;
 bookmap["esther"]=17;
 bookmap["job"]=18;
 bookmap["psalms"]=19;
 bookmap["proverbs"]=20;
 bookmap["ecclesiastes"]=21;
 bookmap["song of solomon"]=22;
 bookmap["isaiah"]=23;
 bookmap["jeremiah"]=24;
 bookmap["lamentations"]=25;
 bookmap["ezekiel"]=26;
 bookmap["daniel"]=27;
 bookmap["hosea"]=28;
 bookmap["joel"]=29;
 bookmap["amos"]=30;
 bookmap["obadiah"]=31;
 bookmap["jonah"]=32;
 bookmap["micah"]=33;
 bookmap["nahum"]=34;
 bookmap["habakkuk"]=35;
 bookmap["zephaniah"]=36;
 bookmap["haggai"]=37;
 bookmap["zechariah"]=38;
 bookmap["malachi"]=39;
 bookmap["matthew"]=40;
 bookmap["mark"]=41;
 bookmap["luke"]=42;
 bookmap["john"]=43;
 bookmap["acts"]=44;
 bookmap["romans"]=45;
 bookmap["1 corinthians"]=46;
 bookmap["2 corinthians"]=47;
 bookmap["galatians"]=48;
 bookmap["ephesians"]=49;
 bookmap["philippians"]=50;
 bookmap["colossians"]=51;
 bookmap["1 thessalonians"]=  52;
 bookmap["2 thessalonians"]=  53;
 bookmap["1 timothy"]=  54;
 bookmap["2 timothy"]=  55;
 bookmap["titus"]=  56;
 bookmap["philemon"]=  57;
 bookmap["hebrews"]=  58;
 bookmap["james"]=  59;
 bookmap["1 peter"]=  60;
 bookmap["2 peter"]=  61;
 bookmap["1 john"]=  62;
 bookmap["2 john"]=  63;
 bookmap["3 john"]=  64;
 bookmap["jude"]=65;
 bookmap["revelation"]=  66;
 bookmap["back matter"]=  68;
 bookmap["other material"]=  69;
 bookmap["tobit"]=  70;
 bookmap["judith"]=  71;
 bookmap["esther (greek)"]=  72;
 bookmap["wisdom of solomon"]=  73;
 bookmap["sirach"]=  74;
 bookmap["baruch"]=  75;
 bookmap["letter of jeremiah"]=  76;
 bookmap["song of the three children"]=  77;
 bookmap["susanna"]=  78;
 bookmap["bel and the dragon"]=  79;
 bookmap["1 maccabees"]=  80;
 bookmap["2 maccabees"]=  81;
 bookmap["1 esdras"]=  82;
 bookmap["prayer of manasses"]=  83;
 bookmap["psalm 151"]=  84;
 bookmap["3 maccabees"]=  85;
 bookmap["2 esdras"]=  86;
 bookmap["4 maccabees"]=  87;
 bookmap["daniel (greek)"]=  88;
}

// Return 0 if no book found
unsigned int books_english_to_id(const ustring & english)
{
  // According to gprof, this procedure took 10% of all execution time
  // of bibledit in a simple editing session. It used an O(n)
  // algorithm to match book names where n = 70 or so, and it is
  // called many times. We first attempt to match using the bookmap
  // hash at O(1) complexity, and if that fails, we fall back to the
  // old way.
  ustring s1(english.casefold());
  int id = bookmap[s1]; // the unordered_map provides O(1) time to lookup instead of O(n)
  if (id != 0) { return id; }

  // For some reason, there are many thousands of calls to this
  // routine with a blank string argument in english. I don't get it,
  // but we can return 0 if so.
  if (english.length() == 0) { return 0; }

  // Otherwise, search for it the old fashion way...
  for (unsigned int i = 0; i < bookdata_books_count(); i++) {
    ustring s2(books_table[i].name);
    s2 = s2.casefold();
    cerr << "Comparing '" << s1 << "' to " << s2 << endl;
    if (s1 == s2) {
      // ... and put it into the bookmap too to save time if we see it again later
      bookmap[s1] = books_table[i].id;
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
