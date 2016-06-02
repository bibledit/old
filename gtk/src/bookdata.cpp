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


#include "bookdata.h"


/*
This table gives the books Bibledit knows about.
The books are put in the standard order.

A note about this data.
The id should always be a number higher than 0, because 0 is taken for "not
found".
The id is connected to a book, and is used in the notes database too.
Therefore, ids are not supposed to be changed; new ones can be added though.
*/
book_record books_table[] =
{
  {
  "Front Matter",               "",        "FRT",  "",     "",       "",                67, btFrontBackMatter,  false}, {
  "Genesis",                    "Gen",     "GEN",  "Gen",  "Ge",     "genesis",          1, btOldTestament,     false}, {
  "Exodus",                     "Exod",    "EXO",  "Exo",  "Ex",     "exodus",           2, btOldTestament,     false}, {
  "Leviticus",                  "Lev",     "LEV",  "Lev",  "Le",     "leviticus",        3, btOldTestament,     false}, {
  "Numbers",                    "Num",     "NUM",  "Num",  "Nu",     "numbers",          4, btOldTestament,     false}, {
  "Deuteronomy",                "Deut",    "DEU",  "Deu",  "De",     "deuteronomy",      5, btOldTestament,     false}, {
  "Joshua",                     "Josh",    "JOS",  "Jos",  "Jos",    "joshua",           6, btOldTestament,     false}, {
  "Judges",                     "Judg",    "JDG",  "Jdg",  "Jud",    "judges",           7, btOldTestament,     false}, {
  "Ruth",                       "Ruth",    "RUT",  "Rut",  "Ru",     "ruth",             8, btOldTestament,     false}, {
  "1 Samuel",                   "1Sam",    "1SA",  "1Sa",  "1Sa",    "1-samuel",         9, btOldTestament,     false}, {
  "2 Samuel",                   "2Sam",    "2SA",  "2Sa",  "2Sa",    "2-samuel",        10, btOldTestament,     false}, {
  "1 Kings",                    "1Kgs",    "1KI",  "1Ki",  "1Ki",    "1-kings",         11, btOldTestament,     false}, {
  "2 Kings",                    "2Kgs",    "2KI",  "2Ki",  "2Ki",    "2-kings",         12, btOldTestament,     false}, {
  "1 Chronicles",               "1Chr",    "1CH",  "1Ch",  "1Ch",    "1-chronicles",    13, btOldTestament,     false}, {
  "2 Chronicles",               "2Chr",    "2CH",  "2Ch",  "2Ch",    "2-chronicles",    14, btOldTestament,     false}, {
  "Ezra",                       "Ezra",    "EZR",  "Ezr",  "Ezr",    "ezra",            15, btOldTestament,     false}, {
  "Nehemiah",                   "Neh",     "NEH",  "Neh",  "Ne",     "nehemiah",        16, btOldTestament,     false}, {
  "Esther",                     "Esth",    "EST",  "Est",  "Es",     "esther",          17, btOldTestament,     false}, {
  "Job",                        "Job",     "JOB",  "Job",  "Job",    "job",             18, btOldTestament,     false}, {
  "Psalms",                     "Ps",      "PSA",  "Psa",  "Ps",     "psalms",          19, btOldTestament,     false}, {
  "Proverbs",                   "Prov",    "PRO",  "Pro",  "Pr",     "proverbs",        20, btOldTestament,     false}, {
  "Ecclesiastes",               "Eccl",    "ECC",  "Ecc",  "Ec",     "ecclesiastes",    21, btOldTestament,     false}, {
  "Song of Solomon",            "Song",    "SNG",  "Sol",  "So",     "song-of-solomon", 22, btOldTestament,     false}, {
  "Isaiah",                     "Isa",     "ISA",  "Isa",  "Isa",    "isaiah",          23, btOldTestament,     false}, {
  "Jeremiah",                   "Jer",     "JER",  "Jer",  "Jer",    "jeremiah",        24, btOldTestament,     false}, {
  "Lamentations",               "Lam",     "LAM",  "Lam",  "La",     "lamentations",    25, btOldTestament,     false}, {
  "Ezekiel",                    "Ezek",    "EZK",  "Eze",  "Eze",    "ezekiel",         26, btOldTestament,     false}, {
  "Daniel",                     "Dan",     "DAN",  "Dan",  "Da",     "daniel",          27, btOldTestament,     false}, {
  "Hosea",                      "Hos",     "HOS",  "Hos",  "Ho",     "hosea",           28, btOldTestament,     false}, {
  "Joel",                       "Joel",    "JOL",  "Joe",  "Joe",    "joel",            29, btOldTestament,     false}, {
  "Amos",                       "Amos",    "AMO",  "Amo",  "Am",     "amos",            30, btOldTestament,     false}, {
  "Obadiah",                    "Obad",    "OBA",  "Oba",  "Ob",     "obadiah",         31, btOldTestament,     true},  {
  "Jonah",                      "Jonah",   "JON",  "Jon",  "Jon",    "jonah",           32, btOldTestament,     false}, {
  "Micah",                      "Mic",     "MIC",  "Mic",  "Mic",    "micah",           33, btOldTestament,     false}, {
  "Nahum",                      "Nah",     "NAM",  "Nah",  "Na",     "nahum",           34, btOldTestament,     false}, {
  "Habakkuk",                   "Hab",     "HAB",  "Hab",  "Hab",    "habakkuk",        35, btOldTestament,     false}, {
  "Zephaniah",                  "Zeph",    "ZEP",  "Zep",  "Zep",    "zephaniah",       36, btOldTestament,     false}, {
  "Haggai",                     "Hag",     "HAG",  "Hag",  "Hag",    "haggai",          37, btOldTestament,     false}, {
  "Zechariah",                  "Zech",    "ZEC",  "Zec",  "Zec",    "zechariah",       38, btOldTestament,     false}, {
  "Malachi",                    "Mal",     "MAL",  "Mal",  "Mal",    "malachi",         39, btOldTestament,     false}, {
  "Matthew",                    "Matt",    "MAT",  "Mat",  "Mt",     "matthew",         40, btNewTestament,     false}, {
  "Mark",                       "Mark",    "MRK",  "Mar",  "Mr",     "mark",            41, btNewTestament,     false}, {
  "Luke",                       "Luke",    "LUK",  "Luk",  "Lu",     "luke",            42, btNewTestament,     false}, {
  "John",                       "John",    "JHN",  "Joh",  "Joh",    "john",            43, btNewTestament,     false}, {
  "Acts",                       "Acts",    "ACT",  "Act",  "Ac",     "acts",            44, btNewTestament,     false}, {
  "Romans",                     "Rom",     "ROM",  "Rom",  "Ro",     "romans",          45, btNewTestament,     false}, {
  "1 Corinthians",              "1Cor",    "1CO",  "1Co",  "1Co",    "1-corinthians",   46, btNewTestament,     false}, {
  "2 Corinthians",              "2Cor",    "2CO",  "2Co",  "2Co",    "2-corinthians",   47, btNewTestament,     false}, {
  "Galatians",                  "Gal",     "GAL",  "Gal",  "Ga",     "galatians",       48, btNewTestament,     false}, {
  "Ephesians",                  "Eph",     "EPH",  "Eph",  "Eph",    "ephesians",       49, btNewTestament,     false}, {
  "Philippians",                "Phil",    "PHP",  "Phi",  "Php",    "philippians",     50, btNewTestament,     false}, {
  "Colossians",                 "Col",     "COL",  "Col",  "Col",    "colossians",      51, btNewTestament,     false}, {
  "1 Thessalonians",            "1Thess",  "1TH",  "1Th",  "1Th",    "1-thessalonians", 52, btNewTestament,     false}, {
  "2 Thessalonians",            "2Thess",  "2TH",  "2Th",  "2Th",    "2-thessalonians", 53, btNewTestament,     false}, {
  "1 Timothy",                  "1Tim",    "1TI",  "1Ti",  "1Ti",    "1-timothy",       54, btNewTestament,     false}, {
  "2 Timothy",                  "2Tim",    "2TI",  "2Ti",  "2Ti",    "2-timothy",       55, btNewTestament,     false}, {
  "Titus",                      "Titus",   "TIT",  "Tit",  "Tit",    "titus",           56, btNewTestament,     false}, {
  "Philemon",                   "Phlm",    "PHM",  "Phm",  "Phm",    "philemon",        57, btNewTestament,     true},  {
  "Hebrews",                    "Heb",     "HEB",  "Heb",  "Heb",    "hebrews",         58, btNewTestament,     false}, {
  "James",                      "Jas",     "JAS",  "Jam",  "Jas",    "james",           59, btNewTestament,     false}, {
  "1 Peter",                    "1Pet",    "1PE",  "1Pe",  "1Pe",    "1-peter",         60, btNewTestament,     false}, {
  "2 Peter",                    "2Pet",    "2PE",  "2Pe",  "2Pe",    "2-peter",         61, btNewTestament,     false}, {
  "1 John",                     "1John",   "1JN",  "1Jo",  "1Jo",    "1-john",          62, btNewTestament,     false}, {
  "2 John",                     "2John",   "2JN",  "2Jo",  "2Jo",    "2-john",          63, btNewTestament,     false}, {
  "3 John",                     "3John",   "3JN",  "3Jo",  "3Jo",    "3-john",          64, btNewTestament,     true},  {
  "Jude",                       "Jude",    "JUD",  "Jud",  "Jude",   "jude",            65, btNewTestament,     true},  {
  "Revelation",                 "Rev",     "REV",  "Rev",  "Re",     "revelation",      66, btNewTestament,     false}, {
  "Back Matter",                "",        "BAK",  "",     "",       "",                68, btFrontBackMatter,  false}, {
  "Other Material",             "",        "OTH",  "",     "",       "",                69, btOtherMaterial,    false}, {
  "Tobit",                      "Tob",     "TOB",  "Tob",  "",       "",                70, btApocrypha,        false}, {
  "Judith",                     "Jdt",     "JDT",  "Jdt",  "",       "",                71, btApocrypha,        false}, {
  "Esther (Greek)",             "AddEsth", "ESG",  "EsG",  "",       "",                72, btApocrypha,        false}, {
  "Wisdom of Solomon",          "Wis",     "WIS",  "Wis",  "",       "",                73, btApocrypha,        false}, {
  "Sirach",                     "Sir",     "SIR",  "Sir",  "",       "",                74, btApocrypha,        false}, {
  "Baruch",                     "Bar",     "BAR",  "Bar",  "",       "",                75, btApocrypha,        false}, {
  "Letter of Jeremiah",         "EpJer",   "LJE",  "LJe",  "",       "",                76, btApocrypha,        true},  {
  "Song of the Three Children", "PrAzar",  "S3Y",  "S3Y",  "",       "",                77, btApocrypha,        true},  {
  "Susanna",                    "Sus",     "SUS",  "Sus",  "",       "",                78, btApocrypha,        true},  {
  "Bel and the Dragon",         "Bel",     "BEL",  "Bel",  "",       "",                79, btApocrypha,        true},  {
  "1 Maccabees",                "1Macc",   "1MA",  "1Ma",  "",       "",                80, btApocrypha,        false}, {
  "2 Maccabees",                "2Macc",   "2MA",  "2Ma",  "",       "",                81, btApocrypha,        false}, {
  "1 Esdras",                   "1Esd",    "1ES",  "1Es",  "",       "",                82, btApocrypha,        false}, {
  "Prayer of Manasses",         "PrMan",   "MAN",  "Man",  "",       "",                83, btApocrypha,        true},  {
  "Psalm 151",                  "Ps151",   "PS2",  "Ps2",  "",       "",                84, btApocrypha,        true},  {
  "3 Maccabees",                "3Macc",   "3MA",  "3Ma",  "",       "",                85, btApocrypha,        false}, {
  "2 Esdras",                   "2Esd",    "2ES",  "2Es",  "",       "",                86, btApocrypha,        false}, {
  "4 Maccabees",                "4Macc",   "4MA",  "4Ma",  "",       "",                87, btApocrypha,        false}, {
  "Daniel (Greek)",             "AddDan",  "DNT",  "Dnt",  "",       "",                88, btApocrypha,        false}
};


unsigned int bookdata_books_count()
{
  unsigned int count = sizeof(books_table) / sizeof(*books_table);
  return count;
}


// For OSIS abbreviations see http://www.crosswire.org/wiki/OSIS_Book_Abbreviations.
