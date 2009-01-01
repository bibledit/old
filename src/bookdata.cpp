/*
** Copyright (©) 2003-2009 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
typeof(book_record) books_table[] =
{
  {
  "Front Matter", "", "FRT", "", 67, btFrontBackMatter, false}, {
  "Genesis", "Gen", "GEN", "Gen", 1, btOldTestament, false}, {
  "Exodus", "Exod", "EXO", "Exo", 2, btOldTestament, false}, {
  "Leviticus", "Lev", "LEV", "Lev", 3, btOldTestament, false}, {
  "Numbers", "Num", "NUM", "Num", 4, btOldTestament, false}, {
  "Deuteronomy", "Deut", "DEU", "Deu", 5, btOldTestament, false}, {
  "Joshua", "Josh", "JOS", "Jos", 6, btOldTestament, false}, {
  "Judges", "Judg", "JDG", "Jdg", 7, btOldTestament, false}, {
  "Ruth", "Ruth", "RUT", "Rut", 8, btOldTestament, false}, {
  "1 Samuel", "1Sam", "1SA", "1Sa", 9, btOldTestament, false}, {
  "2 Samuel", "2Sam", "2SA", "2Sa", 10, btOldTestament, false}, {
  "1 Kings", "1Kgs", "1KI", "1Ki", 11, btOldTestament, false}, {
  "2 Kings", "2Kgs", "2KI", "2Ki", 12, btOldTestament, false}, {
  "1 Chronicles", "1Chr", "1CH", "1Ch", 13, btOldTestament, false}, {
  "2 Chronicles", "2Chr", "2CH", "2Ch", 14, btOldTestament, false}, {
  "Ezra", "Ezra", "EZR", "Ezr", 15, btOldTestament, false}, {
  "Nehemiah", "Neh", "NEH", "Neh", 16, btOldTestament, false}, {
  "Esther", "Esth", "EST", "Est", 17, btOldTestament, false}, {
  "Job", "Job", "JOB", "Job", 18, btOldTestament, false}, {
  "Psalms", "Ps", "PSA", "Psa", 19, btOldTestament, false}, {
  "Proverbs", "Prov", "PRO", "Pro", 20, btOldTestament, false}, {
  "Ecclesiastes", "Eccl", "ECC", "Ecc", 21, btOldTestament, false}, {
  "Song of Solomon", "Song", "SNG", "Sol", 22, btOldTestament, false}, {
  "Isaiah", "Isa", "ISA", "Isa", 23, btOldTestament, false}, {
  "Jeremiah", "Jer", "JER", "Jer", 24, btOldTestament, false}, {
  "Lamentations", "Lam", "LAM", "Lam", 25, btOldTestament, false}, {
  "Ezekiel", "Ezek", "EZK", "Eze", 26, btOldTestament, false}, {
  "Daniel", "Dan", "DAN", "Dan", 27, btOldTestament, false}, {
  "Hosea", "Hos", "HOS", "Hos", 28, btOldTestament, false}, {
  "Joel", "Joel", "JOL", "Joe", 29, btOldTestament, false}, {
  "Amos", "Amos", "AMO", "Amo", 30, btOldTestament, false}, {
  "Obadiah", "Obad", "OBA", "Oba", 31, btOldTestament, true}, {
  "Jonah", "Jonah", "JON", "Jon", 32, btOldTestament, false}, {
  "Micah", "Mic", "MIC", "Mic", 33, btOldTestament, false}, {
  "Nahum", "Nah", "NAM", "Nah", 34, btOldTestament, false}, {
  "Habakkuk", "Hab", "HAB", "Hab", 35, btOldTestament, false}, {
  "Zephaniah", "Zeph", "ZEP", "Zep", 36, btOldTestament, false}, {
  "Haggai", "Hag", "HAG", "Hag", 37, btOldTestament, false}, {
  "Zechariah", "Zech", "ZEC", "Zec", 38, btOldTestament, false}, {
  "Malachi", "Mal", "MAL", "Mal", 39, btOldTestament, false}, {
  "Matthew", "Matt", "MAT", "Mat", 40, btNewTestament, false}, {
  "Mark", "Mark", "MRK", "Mar", 41, btNewTestament, false}, {
  "Luke", "Luke", "LUK", "Luk", 42, btNewTestament, false}, {
  "John", "John", "JHN", "Joh", 43, btNewTestament, false}, {
  "Acts", "Acts", "ACT", "Act", 44, btNewTestament, false}, {
  "Romans", "Rom", "ROM", "Rom", 45, btNewTestament, false}, {
  "1 Corinthians", "1Cor", "1CO", "1Co", 46, btNewTestament, false}, {
  "2 Corinthians", "2Cor", "2CO", "2Co", 47, btNewTestament, false}, {
  "Galatians", "Gal", "GAL", "Gal", 48, btNewTestament, false}, {
  "Ephesians", "Eph", "EPH", "Eph", 49, btNewTestament, false}, {
  "Philippians", "Phil", "PHP", "Phi", 50, btNewTestament, false}, {
  "Colossians", "Col", "COL", "Col", 51, btNewTestament, false}, {
  "1 Thessalonians", "1Thess", "1TH", "1Th", 52, btNewTestament, false}, {
  "2 Thessalonians", "2Thess", "2TH", "2Th", 53, btNewTestament, false}, {
  "1 Timothy", "1Tim", "1TI", "1Ti", 54, btNewTestament, false}, {
  "2 Timothy", "2Tim", "2TI", "2Ti", 55, btNewTestament, false}, {
  "Titus", "Titus", "TIT", "Tit", 56, btNewTestament, false}, {
  "Philemon", "Phlm", "PHM", "Phm", 57, btNewTestament, true}, {
  "Hebrews", "Heb", "HEB", "Heb", 58, btNewTestament, false}, {
  "James", "Jas", "JAS", "Jam", 59, btNewTestament, false}, {
  "1 Peter", "1Pet", "1PE", "1Pe", 60, btNewTestament, false}, {
  "2 Peter", "2Pet", "2PE", "2Pe", 61, btNewTestament, false}, {
  "1 John", "1John", "1JN", "1Jo", 62, btNewTestament, false}, {
  "2 John", "2John", "2JN", "2Jo", 63, btNewTestament, false}, {
  "3 John", "3John", "3JN", "3Jo", 64, btNewTestament, true}, {
  "Jude", "Jude", "JUD", "Jud", 65, btNewTestament, true}, {
  "Revelation", "Rev", "REV", "Rev", 66, btNewTestament, false}, {
  "Back Matter", "", "BAK", "", 68, btFrontBackMatter, false}, {
  "Other Material", "", "OTH", "", 69, btOtherMaterial, false}, {
  "Tobit", "Tobit", "TOB", "Tob", 70, btApocrypha, false}, {
  "Judith", "Jdt", "JDT", "Jdt", 71, btApocrypha, false}, {
  "Esther (Greek)", "AddEsth", "ESG", "EsG", 72, btApocrypha, false}, {
  "Wisdom of Solomon", "Wis", "WIS", "Wis", 73, btApocrypha, false}, {
  "Sirach", "Sir", "SIR", "Sir", 74, btApocrypha, false}, {
  "Baruch", "Bar", "BAR", "Bar", 75, btApocrypha, false}, {
  "Letter of Jeremiah", "EpJer", "LJE", "LJe", 76, btApocrypha, true}, {
  "Song of the Three Children", "PrAzar", "S3Y", "S3Y", 77, btApocrypha, true}, {
  "Susanna", "Sus", "SUS", "Sus", 78, btApocrypha, true}, {
  "Bel and the Dragon", "Bel", "BEL", "Bel", 79, btApocrypha, true}, {
  "1 Maccabees", "1Macc", "1MA", "1Ma", 80, btApocrypha, false}, {
  "2 Maccabees", "2Macc", "2MA", "2Ma", 81, btApocrypha, false}, {
  "1 Esdras", "1Esd", "1ES", "1Es", 82, btApocrypha, false}, {
  "Prayer of Manasses", "PrMan", "MAN", "Man", 83, btApocrypha, true}, {
  "Psalm 151", "Ps151", "PS2", "Ps2", 84, btApocrypha, true}, {
  "3 Maccabees", "3Macc", "3MA", "3Ma", 85, btApocrypha, false}, {
  "2 Esdras", "2Esd", "2ES", "2Es", 86, btApocrypha, false}, {
  "4 Maccabees", "4Macc", "4MA", "4Ma", 87, btApocrypha, false}, {
  "Daniel (Greek)", "AddDan", "DNT", "Dnt", 88, btApocrypha, false}, {
  "XXA", "XXA", "XXA", "XXA", 89, btOtherMaterial, false}, {
  "XXB", "XXB", "XXB", "XXB", 90, btOtherMaterial, false}, {
  "XXC", "XXC", "XXC", "XXC", 91, btOtherMaterial, false}, {
  "XXD", "XXD", "XXD", "XXD", 92, btOtherMaterial, false}, {
  "XXE", "XXE", "XXE", "XXE", 93, btOtherMaterial, false}, {
  "XXF", "XXF", "XXF", "XXF", 94, btOtherMaterial, false}, {
  "XXG", "XXG", "XXG", "XXG", 95, btOtherMaterial, false}
};

unsigned int bookdata_books_count()
{
  unsigned int count = sizeof(books_table) / sizeof(*books_table);
  return count;
}
