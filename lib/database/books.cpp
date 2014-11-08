/*
Copyright (©) 2003-2014 Teus Benschop.

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


// This is a database for the various book names.

// It is safe to store this in a SQLite database, 
// because normally only read operations occur, 
// so there is a very low risk of database corruption.


Database_Books::Database_Books ()
{
}


Database_Books::~Database_Books ()
{
}


sqlite3 * Database_Books::connect ()
{
  sqlite3 * db = database_sqlite_connect ("books");
  return db;
}


void Database_Books::create ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "PRAGMA synchronous = OFF;");
  database_sqlite_exec (db, "DROP TABLE IF EXISTS books;");
  string sql = "CREATE TABLE IF NOT EXISTS books ("
               "id integer,"
               "english text,"
               "osis text,"
               "usfm text,"
               "bibleworks text,"
               "onlinebible text,"
               "type text,"
               "onechapter integer"
               ");";
  database_sqlite_exec (db, sql);

  /*
  The following contains book names as follows:
  ID | English name | OSIS | USFM | BibleWorks | Online Bible | type | chapter
  Clarifications:
  ID: Bibledit internal book ID.
  English name:
  OSIS: The OSIS abbreviation.
  USFM: The USFM ID.
  BibleWorks: The BibleWorks abbreviation.
  Online Bible: Online Bible abbreviation.
  type: One of the following
        frontback - Frontmatter / Backmatter
        ot        - Old Testament
        nt        - New Testament
        other     - Other matter
        ap        - Apocrypha
  One chapter: If true, this book has only one chapter.
  The order in which the books are presented is the standard order.
  */
  string fragment = "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES ";
  database_sqlite_exec (db, fragment + "(1, 'Genesis', 'Gen', 'GEN', 'Gen', 'Ge', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(2, 'Exodus', 'Exod', 'EXO', 'Exo', 'Ex', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(3, 'Leviticus', 'Lev', 'LEV', 'Lev', 'Le', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(4, 'Numbers', 'Num', 'NUM', 'Num', 'Nu', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(5, 'Deuteronomy', 'Deut', 'DEU', 'Deu', 'De', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(6, 'Joshua', 'Josh', 'JOS', 'Jos', 'Jos', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(7, 'Judges', 'Judg', 'JDG', 'Jdg', 'Jud', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(8, 'Ruth', 'Ruth', 'RUT', 'Rut', 'Ru', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(9, '1 Samuel', '1Sam', '1SA', '1Sa', '1Sa', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(10, '2 Samuel', '2Sam', '2SA', '2Sa', '2Sa', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(11, '1 Kings', '1Kgs', '1KI', '1Ki', '1Ki', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(12, '2 Kings', '2Kgs', '2KI', '2Ki', '2Ki', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(13, '1 Chronicles', '1Chr', '1CH', '1Ch', '1Ch', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(14, '2 Chronicles', '2Chr', '2CH', '2Ch', '2Ch', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(15, 'Ezra', 'Ezra', 'EZR', 'Ezr', 'Ezr', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(16, 'Nehemiah', 'Neh', 'NEH', 'Neh', 'Ne', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(17, 'Esther', 'Esth', 'EST', 'Est', 'Es', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(18, 'Job', 'Job', 'JOB', 'Job', 'Job', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(19, 'Psalms', 'Ps', 'PSA', 'Psa', 'Ps', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(20, 'Proverbs', 'Prov', 'PRO', 'Pro', 'Pr', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(21, 'Ecclesiastes', 'Eccl', 'ECC', 'Ecc', 'Ec', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(22, 'Song of Solomon', 'Song', 'SNG', 'Sol', 'So', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(23, 'Isaiah', 'Isa', 'ISA', 'Isa', 'Isa', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(24, 'Jeremiah', 'Jer', 'JER', 'Jer', 'Jer', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(25, 'Lamentations', 'Lam', 'LAM', 'Lam', 'La', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(26, 'Ezekiel', 'Ezek', 'EZK', 'Eze', 'Eze', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(27, 'Daniel', 'Dan', 'DAN', 'Dan', 'Da', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(28, 'Hosea', 'Hos', 'HOS', 'Hos', 'Ho', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(29, 'Joel', 'Joel', 'JOL', 'Joe', 'Joe', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(30, 'Amos', 'Amos', 'AMO', 'Amo', 'Am', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(31, 'Obadiah', 'Obad', 'OBA', 'Oba', 'Ob', 'ot', 1);");
  database_sqlite_exec (db, fragment + "(32, 'Jonah', 'Jonah', 'JON', 'Jon', 'Jon', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(33, 'Micah', 'Mic', 'MIC', 'Mic', 'Mic', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(34, 'Nahum', 'Nah', 'NAM', 'Nah', 'Na', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(35, 'Habakkuk', 'Hab', 'HAB', 'Hab', 'Hab', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(36, 'Zephaniah', 'Zeph', 'ZEP', 'Zep', 'Zep', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(37, 'Haggai', 'Hag', 'HAG', 'Hag', 'Hag', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(38, 'Zechariah', 'Zech', 'ZEC', 'Zec', 'Zec', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(39, 'Malachi', 'Mal', 'MAL', 'Mal', 'Mal', 'ot', 0);");
  database_sqlite_exec (db, fragment + "(40, 'Matthew', 'Matt', 'MAT', 'Mat', 'Mt', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(41, 'Mark', 'Mark', 'MRK', 'Mar', 'Mr', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(42, 'Luke', 'Luke', 'LUK', 'Luk', 'Lu', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(43, 'John', 'John', 'JHN', 'Joh', 'Joh', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(44, 'Acts', 'Acts', 'ACT', 'Act', 'Ac', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(45, 'Romans', 'Rom', 'ROM', 'Rom', 'Ro', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(46, '1 Corinthians', '1Cor', '1CO', '1Co', '1Co', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(47, '2 Corinthians', '2Cor', '2CO', '2Co', '2Co', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(48, 'Galatians', 'Gal', 'GAL', 'Gal', 'Ga', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(49, 'Ephesians', 'Eph', 'EPH', 'Eph', 'Eph', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(50, 'Philippians', 'Phil', 'PHP', 'Phi', 'Php', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(51, 'Colossians', 'Col', 'COL', 'Col', 'Col', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(52, '1 Thessalonians', '1Thess', '1TH', '1Th', '1Th', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(53, '2 Thessalonians', '2Thess', '2TH', '2Th', '2Th', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(54, '1 Timothy', '1Tim', '1TI', '1Ti', '1Ti', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(55, '2 Timothy', '2Tim', '2TI', '2Ti', '2Ti', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(56, 'Titus', 'Titus', 'TIT', 'Tit', 'Tit', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(57, 'Philemon', 'Phlm', 'PHM', 'Phm', 'Phm', 'nt', 1);");
  database_sqlite_exec (db, fragment + "(58, 'Hebrews', 'Heb', 'HEB', 'Heb', 'Heb', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(59, 'James', 'Jas', 'JAS', 'Jam', 'Jas', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(60, '1 Peter', '1Pet', '1PE', '1Pe', '1Pe', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(61, '2 Peter', '2Pet', '2PE', '2Pe', '2Pe', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(62, '1 John', '1John', '1JN', '1Jo', '1Jo', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(63, '2 John', '2John', '2JN', '2Jo', '2Jo', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(64, '3 John', '3John', '3JN', '3Jo', '3Jo', 'nt', 1);");
  database_sqlite_exec (db, fragment + "(65, 'Jude', 'Jude', 'JUD', 'Jud', 'Jude', 'nt', 1);");
  database_sqlite_exec (db, fragment + "(66, 'Revelation', 'Rev', 'REV', 'Rev', 'Re', 'nt', 0);");
  database_sqlite_exec (db, fragment + "(67, 'Front Matter', '', 'FRT', '', '', 'frontback', 0);");
  database_sqlite_exec (db, fragment + "(68, 'Back Matter', '', 'BAK', '', '', 'frontback', 0);");
  database_sqlite_exec (db, fragment + "(69, 'Other Material', '', 'OTH', '', '', 'other', 0);");
  database_sqlite_exec (db, fragment + "(70, 'Tobit', 'Tob', 'TOB', 'Tob', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(71, 'Judith', 'Jdt', 'JDT', 'Jdt', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(72, 'Esther (Greek)', 'AddEsth', 'ESG', 'EsG', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(73, 'Wisdom of Solomon', 'Wis', 'WIS', 'Wis', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(74, 'Sirach', 'Sir', 'SIR', 'Sir', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(75, 'Baruch', 'Bar', 'BAR', 'Bar', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(76, 'Letter of Jeremiah', 'EpJer', 'LJE', 'LJe', '', 'ap', 1);");
  database_sqlite_exec (db, fragment + "(77, 'Song of the Three Children', 'PrAzar', 'S3Y', 'S3Y', '', 'ap', 1);");
  database_sqlite_exec (db, fragment + "(78, 'Susanna', 'Sus', 'SUS', 'Sus', '', 'ap', 1);");
  database_sqlite_exec (db, fragment + "(79, 'Bel and the Dragon', 'Bel', 'BEL', 'Bel', '', 'ap', 1);");
  database_sqlite_exec (db, fragment + "(80, '1 Maccabees', '1Macc', '1MA', '1Ma', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(81, '2 Maccabees', '2Macc', '2MA', '2Ma', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(82, '1 Esdras', '1Esd', '1ES', '1Es', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(83, 'Prayer of Manasses', 'PrMan', 'MAN', 'Man', '', 'ap', 1);");
  database_sqlite_exec (db, fragment + "(84, 'Psalm 151', 'Ps151', 'PS2', 'Ps2', '', 'ap', 1);");
  database_sqlite_exec (db, fragment + "(85, '3 Maccabees', '3Macc', '3MA', '3Ma', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(86, '2 Esdras', '2Esd', '2ES', '2Es', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(87, '4 Maccabees', '4Macc', '4MA', '4Ma', '', 'ap', 0);");
  database_sqlite_exec (db, fragment + "(88, 'Daniel (Greek)', 'AddDan', 'DNT', 'Dnt', '', 'ap', 0);");

  database_sqlite_disconnect (db);
}


vector <int> Database_Books::getIDs ()
{
  vector <int> ids;
  sqlite3 * db = connect ();
  string sql = "SELECT id FROM books WHERE id < 70 ORDER BY rowid;"; // Exclude Apocrypha.
  vector <string> idss = database_sqlite_query (db, sql) ["id"];
  database_sqlite_disconnect (db);
  for (auto id : idss) ids.push_back (convert_to_int (id));
  return ids;
}


int Database_Books::getIdFromEnglish (string english)
{
  SqliteSQL sql;
  sql.add ("SELECT id FROM books WHERE english =");
  sql.add (english);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> id = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  if (!id.empty ()) return convert_to_int (id [0]);
  return 0;
}


string Database_Books::getEnglishFromId (int id)
{
  string unknown = gettext ("Unknown");
  // Care for empty input.
  if (id == 0) return unknown;
  // Get the English name of the book.
  SqliteSQL sql;
  sql.add ("SELECT english FROM books WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> english = database_sqlite_query (db, sql.sql) ["english"];
  database_sqlite_disconnect (db);
  if (!english.empty ()) return english [0];
  return unknown;
}


string Database_Books::getUsfmFromId (int id)
{
  SqliteSQL sql;
  sql.add ("SELECT usfm FROM books WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> usfm = database_sqlite_query (db, sql.sql) ["usfm"];
  database_sqlite_disconnect (db);
  if (!usfm.empty ()) return usfm [0];
  return "XXX";
}


string Database_Books::getBibleworksFromId (int id)
{
  SqliteSQL sql;
  sql.add ("SELECT bibleworks FROM books WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> bibleworks = database_sqlite_query (db, sql.sql) ["bibleworks"];
  database_sqlite_disconnect (db);
  if (!bibleworks.empty ()) return bibleworks [0];
  return "Xxx";
}


int Database_Books::getIdFromUsfm (string usfm)
{
  SqliteSQL sql;
  sql.add ("SELECT id FROM books WHERE usfm =");
  sql.add (usfm);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> id = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  if (!id.empty ()) return convert_to_int (id [0]);
  return 0;
}


int Database_Books::getIdFromOsis (string osis)
{
  SqliteSQL sql;
  sql.add ("SELECT id FROM books WHERE osis =");
  sql.add (osis);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> id = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  if (!id.empty ()) return convert_to_int (id [0]);
  return 0;
}


int Database_Books::getIdFromBibleworks (string bibleworks)
{
  SqliteSQL sql;
  sql.add ("SELECT id FROM books WHERE bibleworks =");
  sql.add (bibleworks);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> id = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  if (!id.empty ()) return convert_to_int (id [0]);
  return 0;
}


/*
Tries to interprete $text as the name of a Bible book.
Returns the book's identifier if it succeeds.
If it fails, it returns 0.
*/
int Database_Books::getIdLikeText (string text)
{
  SqliteSQL sql;
  sql.add ("SELECT id FROM books WHERE english LIKE");
  sql.add (text + "%");
  sql.add ("OR osis LIKE");
  sql.add (text + "%");
  sql.add ("OR usfm LIKE");
  sql.add (text + "%");
  sql.add ("OR bibleworks LIKE");
  sql.add (text + "%");
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> id = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  if (!id.empty ()) return convert_to_int (id [0]);
  return 0;
}


int Database_Books::getIdFromOnlinebible (string onlinebible)
{
  SqliteSQL sql;
  sql.add ("SELECT id FROM books WHERE onlinebible =");
  sql.add (onlinebible);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> id = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  if (!id.empty ()) return convert_to_int (id [0]);
  return 0;
}


string Database_Books::getOnlinebibleFromId (int id)
{
  SqliteSQL sql;
  sql.add ("SELECT onlinebible FROM books WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> onlinebible = database_sqlite_query (db, sql.sql) ["onlinebible"];
  database_sqlite_disconnect (db);
  if (!onlinebible.empty ()) return onlinebible [0];
  return "";
}


int Database_Books::getSequenceFromId (int id)
{
  SqliteSQL sql;
  sql.add ("SELECT rowid FROM books WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> rowid = database_sqlite_query (db, sql.sql) ["rowid"];
  database_sqlite_disconnect (db);
  if (!rowid.empty ()) return convert_to_int (rowid [0]);
  return 0;
}


string Database_Books::getType (int id)
{
  SqliteSQL sql;
  sql.add ("SELECT type FROM books WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> type = database_sqlite_query (db, sql.sql) ["type"];
  database_sqlite_disconnect (db);
  if (!type.empty ()) return type [0];
  return "";
}
