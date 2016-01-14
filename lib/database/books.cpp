/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <database/books.h>
#include <config/globals.h>
#include <database/sqlite.h>
#include <filter/string.h>
#include <filter/diff.h>


// All data is stored in the code in memory, not in a database on disk.


typedef struct
{
  const char *english;     // English name.
  const char *osis;        // OSIS abbreviation.
  const char *usfm;        // USFM ID.
  const char *bibleworks;  // BibleWorks abbreviation.
  const char *onlinebible; // Online Bible abbreviation.
  int id;                  // Bibledit's internal id.
  const char *type;        // The type of the book.
  bool onechapter;         // The book has one chapter.
} book_record;


/*
This table gives the books Bibledit knows about.
The books are put in the standard order.

A note about this data.

* The id should always be a number higher than 0, because 0 is taken for "not found".
* The id is connected to a book, and is used throughout the databases.
  Therefore, ids are not supposed to be changed; new ones can be added though.
* type: One of the following
    frontback - Frontmatter / Backmatter
    ot        - Old Testament
    nt        - New Testament
    other     - Other matter
    ap        - Apocrypha
*/
book_record books_table [] =
{
  {"Genesis",                    "Gen",     "GEN",  "Gen",  "Ge",    1, "ot",        false},
  {"Exodus",                     "Exod",    "EXO",  "Exo",  "Ex",    2, "ot",        false},
  {"Leviticus",                  "Lev",     "LEV",  "Lev",  "Le",    3, "ot",        false},
  {"Numbers",                    "Num",     "NUM",  "Num",  "Nu",    4, "ot",        false},
  {"Deuteronomy",                "Deut",    "DEU",  "Deu",  "De",    5, "ot",        false},
  {"Joshua",                     "Josh",    "JOS",  "Jos",  "Jos",   6, "ot",        false},
  {"Judges",                     "Judg",    "JDG",  "Jdg",  "Jud",   7, "ot",        false},
  {"Ruth",                       "Ruth",    "RUT",  "Rut",  "Ru",    8, "ot",        false},
  {"1 Samuel",                   "1Sam",    "1SA",  "1Sa",  "1Sa",   9, "ot",        false},
  {"2 Samuel",                   "2Sam",    "2SA",  "2Sa",  "2Sa",  10, "ot",        false},
  {"1 Kings",                    "1Kgs",    "1KI",  "1Ki",  "1Ki",  11, "ot",        false},
  {"2 Kings",                    "2Kgs",    "2KI",  "2Ki",  "2Ki",  12, "ot",        false},
  {"1 Chronicles",               "1Chr",    "1CH",  "1Ch",  "1Ch",  13, "ot",        false},
  {"2 Chronicles",               "2Chr",    "2CH",  "2Ch",  "2Ch",  14, "ot",        false},
  {"Ezra",                       "Ezra",    "EZR",  "Ezr",  "Ezr",  15, "ot",        false},
  {"Nehemiah",                   "Neh",     "NEH",  "Neh",  "Ne",   16, "ot",        false},
  {"Esther",                     "Esth",    "EST",  "Est",  "Es",   17, "ot",        false},
  {"Job",                        "Job",     "JOB",  "Job",  "Job",  18, "ot",        false},
  {"Psalms",                     "Ps",      "PSA",  "Psa",  "Ps",   19, "ot",        false},
  {"Proverbs",                   "Prov",    "PRO",  "Pro",  "Pr",   20, "ot",        false},
  {"Ecclesiastes",               "Eccl",    "ECC",  "Ecc",  "Ec",   21, "ot",        false},
  {"Song of Solomon",            "Song",    "SNG",  "Sol",  "So",   22, "ot",        false},
  {"Isaiah",                     "Isa",     "ISA",  "Isa",  "Isa",  23, "ot",        false},
  {"Jeremiah",                   "Jer",     "JER",  "Jer",  "Jer",  24, "ot",        false},
  {"Lamentations",               "Lam",     "LAM",  "Lam",  "La",   25, "ot",        false},
  {"Ezekiel",                    "Ezek",    "EZK",  "Eze",  "Eze",  26, "ot",        false},
  {"Daniel",                     "Dan",     "DAN",  "Dan",  "Da",   27, "ot",        false},
  {"Hosea",                      "Hos",     "HOS",  "Hos",  "Ho",   28, "ot",        false},
  {"Joel",                       "Joel",    "JOL",  "Joe",  "Joe",  29, "ot",        false},
  {"Amos",                       "Amos",    "AMO",  "Amo",  "Am",   30, "ot",        false},
  {"Obadiah",                    "Obad",    "OBA",  "Oba",  "Ob",   31, "ot",        true}, 
  {"Jonah",                      "Jonah",   "JON",  "Jon",  "Jon",  32, "ot",        false},
  {"Micah",                      "Mic",     "MIC",  "Mic",  "Mic",  33, "ot",        false},
  {"Nahum",                      "Nah",     "NAM",  "Nah",  "Na",   34, "ot",        false},
  {"Habakkuk",                   "Hab",     "HAB",  "Hab",  "Hab",  35, "ot",        false},
  {"Zephaniah",                  "Zeph",    "ZEP",  "Zep",  "Zep",  36, "ot",        false},
  {"Haggai",                     "Hag",     "HAG",  "Hag",  "Hag",  37, "ot",        false},
  {"Zechariah",                  "Zech",    "ZEC",  "Zec",  "Zec",  38, "ot",        false},
  {"Malachi",                    "Mal",     "MAL",  "Mal",  "Mal",  39, "ot",        false},
  {"Matthew",                    "Matt",    "MAT",  "Mat",  "Mt",   40, "nt",        false},
  {"Mark",                       "Mark",    "MRK",  "Mar",  "Mr",   41, "nt",        false},
  {"Luke",                       "Luke",    "LUK",  "Luk",  "Lu",   42, "nt",        false},
  {"John",                       "John",    "JHN",  "Joh",  "Joh",  43, "nt",        false},
  {"Acts",                       "Acts",    "ACT",  "Act",  "Ac",   44, "nt",        false},
  {"Romans",                     "Rom",     "ROM",  "Rom",  "Ro",   45, "nt",        false},
  {"1 Corinthians",              "1Cor",    "1CO",  "1Co",  "1Co",  46, "nt",        false},
  {"2 Corinthians",              "2Cor",    "2CO",  "2Co",  "2Co",  47, "nt",        false},
  {"Galatians",                  "Gal",     "GAL",  "Gal",  "Ga",   48, "nt",        false},
  {"Ephesians",                  "Eph",     "EPH",  "Eph",  "Eph",  49, "nt",        false},
  {"Philippians",                "Phil",    "PHP",  "Phi",  "Php",  50, "nt",        false},
  {"Colossians",                 "Col",     "COL",  "Col",  "Col",  51, "nt",        false},
  {"1 Thessalonians",            "1Thess",  "1TH",  "1Th",  "1Th",  52, "nt",        false},
  {"2 Thessalonians",            "2Thess",  "2TH",  "2Th",  "2Th",  53, "nt",        false},
  {"1 Timothy",                  "1Tim",    "1TI",  "1Ti",  "1Ti",  54, "nt",        false},
  {"2 Timothy",                  "2Tim",    "2TI",  "2Ti",  "2Ti",  55, "nt",        false},
  {"Titus",                      "Titus",   "TIT",  "Tit",  "Tit",  56, "nt",        false},
  {"Philemon",                   "Phlm",    "PHM",  "Phm",  "Phm",  57, "nt",        true}, 
  {"Hebrews",                    "Heb",     "HEB",  "Heb",  "Heb",  58, "nt",        false},
  {"James",                      "Jas",     "JAS",  "Jam",  "Jas",  59, "nt",        false},
  {"1 Peter",                    "1Pet",    "1PE",  "1Pe",  "1Pe",  60, "nt",        false},
  {"2 Peter",                    "2Pet",    "2PE",  "2Pe",  "2Pe",  61, "nt",        false},
  {"1 John",                     "1John",   "1JN",  "1Jo",  "1Jo",  62, "nt",        false},
  {"2 John",                     "2John",   "2JN",  "2Jo",  "2Jo",  63, "nt",        false},
  {"3 John",                     "3John",   "3JN",  "3Jo",  "3Jo",  64, "nt",        true}, 
  {"Jude",                       "Jude",    "JUD",  "Jud",  "Jude", 65, "nt",        true}, 
  {"Revelation",                 "Rev",     "REV",  "Rev",  "Re",   66, "nt",        false},
  {"Front Matter",               "",        "FRT",  "",     "",     67, "frontback", false},
  {"Back Matter",                "",        "BAK",  "",     "",     68, "frontback", false},
  {"Other Material",             "",        "OTH",  "",     "",     69, "other",     false},
  {"Tobit",                      "Tob",     "TOB",  "Tob",  "",     70, "ap",        false},
  {"Judith",                     "Jdt",     "JDT",  "Jdt",  "",     71, "ap",        false},
  {"Esther (Greek)",             "AddEsth", "ESG",  "EsG",  "",     72, "ap",        false},
  {"Wisdom of Solomon",          "Wis",     "WIS",  "Wis",  "",     73, "ap",        false},
  {"Sirach",                     "Sir",     "SIR",  "Sir",  "",     74, "ap",        false},
  {"Baruch",                     "Bar",     "BAR",  "Bar",  "",     75, "ap",        false},
  {"Letter of Jeremiah",         "EpJer",   "LJE",  "LJe",  "",     76, "ap",        true}, 
  {"Song of the Three Children", "PrAzar",  "S3Y",  "S3Y",  "",     77, "ap",        true}, 
  {"Susanna",                    "Sus",     "SUS",  "Sus",  "",     78, "ap",        true}, 
  {"Bel and the Dragon",         "Bel",     "BEL",  "Bel",  "",     79, "ap",        true}, 
  {"1 Maccabees",                "1Macc",   "1MA",  "1Ma",  "",     80, "ap",        false},
  {"2 Maccabees",                "2Macc",   "2MA",  "2Ma",  "",     81, "ap",        false},
  {"1 Esdras",                   "1Esd",    "1ES",  "1Es",  "",     82, "ap",        false},
  {"Prayer of Manasses",         "PrMan",   "MAN",  "Man",  "",     83, "ap",        true}, 
  {"Psalm 151",                  "Ps151",   "PS2",  "Ps2",  "",     84, "ap",        true}, 
  {"3 Maccabees",                "3Macc",   "3MA",  "3Ma",  "",     85, "ap",        false},
  {"2 Esdras",                   "2Esd",    "2ES",  "2Es",  "",     86, "ap",        false},
  {"4 Maccabees",                "4Macc",   "4MA",  "4Ma",  "",     87, "ap",        false},
  {"Daniel (Greek)",             "AddDan",  "DNT",  "Dnt",  "",     88, "ap",        false},
  {"Introductions",              "",        "INT",  "",     "",     89, "frontback", false},
  {"Concordance",                "",        "CNC",  "",     "",     90, "frontback", false},
  {"Glossary",                   "",        "GLO",  "",     "",     91, "frontback", false},
  {"Topical Index",              "",        "TDX",  "",     "",     92, "frontback", false},
  {"Names Index",                "",        "NDX",  "",     "",     93, "frontback", false},
  {"Extra A",                    "",        "XXA",  "",     "",     94, "frontback", false},
  {"Extra B",                    "",        "XXB",  "",     "",     95, "frontback", false},
  {"Extra C",                    "",        "XXC",  "",     "",     96, "frontback", false},
  {"Extra D",                    "",        "XXD",  "",     "",     97, "frontback", false},
  {"Extra E",                    "",        "XXE",  "",     "",     98, "frontback", false},
  {"Extra F",                    "",        "XXF",  "",     "",     99, "frontback", false},
  {"Extra G",                    "",        "XXG",  "",     "",    100, "frontback", false},
};


// For OSIS abbreviations see http://www.crosswire.org/wiki/OSIS_Book_Abbreviations.


vector <int> Database_Books::getIDs ()
{
  vector <int> ids;
  for (unsigned int i = 0; i < data_count (); i++) {
    // Exclude Apocrypha.
    int id = books_table[i].id;
    if (id < 70) ids.push_back (id);
  }
  return ids;
}


int Database_Books::getIdFromEnglish (string english)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (english == books_table[i].english) {
      return books_table[i].id;
    }
  }
  return 0;  
}


string Database_Books::getEnglishFromId (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return books_table[i].english;
    }
  }
  return "Unknown";
}


string Database_Books::getUsfmFromId (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return books_table[i].usfm;
    }
  }
  return "XXX";
}


string Database_Books::getBibleworksFromId (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return books_table[i].bibleworks;
    }
  }
  return "Xxx";
}


string Database_Books::getOsisFromId (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return books_table[i].osis;
    }
  }
  return "Unknown";
}


int Database_Books::getIdFromUsfm (string usfm)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (usfm == books_table[i].usfm) {
      return books_table[i].id;
    }
  }
  return 0;
}


int Database_Books::getIdFromOsis (string osis)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (osis == books_table[i].osis) {
      return books_table[i].id;
    }
  }
  return 0;
}


int Database_Books::getIdFromBibleworks (string bibleworks)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (bibleworks == books_table[i].bibleworks) {
      return books_table[i].id;
    }
  }
  return 0;
}


/*
Tries to interprete $text as the name of a Bible book.
Returns the book's identifier if it succeeds.
If it fails, it returns 0.
*/
int Database_Books::getIdLikeText (string text)
{
  // Go through all known book names and abbreviations.
  // Note how much the $text differs from the known names.
  // Then return the best match.
  vector <int> ids;
  vector <int> similarities;
  for (unsigned int i = 0; i < data_count(); i++) {
    int id = books_table[i].id;
    ids.push_back (id);
    similarities.push_back (filter_diff_similarity (text, books_table[i].english));
    ids.push_back (id);
    similarities.push_back (filter_diff_similarity (text, books_table[i].osis));
    ids.push_back (id);
    similarities.push_back (filter_diff_similarity (text, books_table[i].usfm));
    ids.push_back (id);
    similarities.push_back (filter_diff_similarity (text, books_table[i].bibleworks));
    ids.push_back (id);
    similarities.push_back (filter_diff_similarity (text, books_table[i].onlinebible));
  }
  quick_sort (similarities, ids, 0, ids.size());
  return ids.back ();
}


int Database_Books::getIdFromOnlinebible (string onlinebible)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (onlinebible == books_table[i].onlinebible) {
      return books_table[i].id;
    }
  }
  return 0;
}


string Database_Books::getOnlinebibleFromId (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return books_table[i].onlinebible;
    }
  }
  return "";
}


int Database_Books::getIdLastEffort (string text)
{
  // Remove all spaces.
  text = filter_string_str_replace (" ", "", text);
  text = unicode_string_casefold (text);
  // Length.
  int length = text.length ();
  // Go through all book data, convert everything to lower case, and remove all spaces, and compare only "length" characters.
  for (unsigned int i = 0; i < data_count(); i++) {
    string book;
    book = unicode_string_casefold (filter_string_str_replace (" ", "", books_table[i].english));
    if (text == book.substr (0, length)) return books_table[i].id;
    book = unicode_string_casefold (filter_string_str_replace (" ", "", books_table[i].osis));
    if (text == book.substr (0, length)) return books_table[i].id;
    book = unicode_string_casefold (filter_string_str_replace (" ", "", books_table[i].usfm));
    if (text == book.substr (0, length)) return books_table[i].id;
    book = unicode_string_casefold (filter_string_str_replace (" ", "", books_table[i].bibleworks));
    if (text == book.substr (0, length)) return books_table[i].id;
    book = unicode_string_casefold (filter_string_str_replace (" ", "", books_table[i].onlinebible));
    if (text == book.substr (0, length)) return books_table[i].id;
  }
  return 0;
}


int Database_Books::getSequenceFromId (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return i;
    }
  }
  return 0;
}


string Database_Books::getType (int id)
{
  for (unsigned int i = 0; i < data_count(); i++) {
    if (id == books_table[i].id) {
      return books_table[i].type;
    }
  }
  return "";
}


unsigned int Database_Books::data_count ()
{
  return sizeof (books_table) / sizeof (*books_table);
}
