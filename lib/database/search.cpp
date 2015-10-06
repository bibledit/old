/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <database/search.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <filter/text.h>
#include <config/globals.h>
#include <database/sqlite.h>
#include <database/bibles.h>
#include <database/config/bible.h>


/*

Database resilience: The index is re-created every night.

Suggestion: Improved searching on bible and notes, also using concepts like 'near' and 'relevance' and 'score'.

*/


sqlite3 * Database_Search::connect ()
{
  sqlite3 * db = database_sqlite_connect ("search");
  database_sqlite_exec (db, "PRAGMA temp_store = MEMORY;");
  database_sqlite_exec (db, "PRAGMA synchronous = OFF;");
  database_sqlite_exec (db, "PRAGMA journal_mode = OFF;");
  return db;
}


void Database_Search::create ()
{
  // Searching in SQLite could use the FTS3 or FTS4 module.
  // This module is optional. 
  // On some hosting accounts it may be available. On others it may not.
  // Therefore it cannot be used.
  // Searching uses a stanard table.
  sqlite3 * db = connect ();
  string sql = "CREATE TABLE IF NOT EXISTS bibles ("
               "bible text,"
               "book integer,"
               "chapter integer,"
               "verse integer,"
               "usfmraw text,"
               "usfmlower text,"
               "plainraw text,"
               "plainlower text"
               ");";
  database_sqlite_exec (db, sql);
  // No index: Saving will be faster on low power devices.
  // Since a translator usually saves more frequently than searching,
  // the database is optimized for saving, not for searching.
  // sql = "CREATE INDEX IF NOT EXISTS speedup ON bibles (bible, book, chapter)";
  // database_sqlite_exec (db, sql);
  sql = "DROP INDEX IF EXISTS speedup";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_Search::optimize () 
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "REINDEX bibles;");
  database_sqlite_exec (db, "VACUUM bibles;");
  database_sqlite_disconnect (db);
}


// Does a checkup on the health of the database.
// Returns false if the database is healthy.
// Returns true if the database was re-created, and is now healthy.
// If $recreate is true, then it will be recreated anyway.
bool Database_Search::checkup (bool recreate)
{
  // Do an integrity check, and bail out if the database is healthy.
  if (!recreate) if (database_sqlite_healthy ("search")) return false;
  // Recreate the database.
  string file = filter_url_create_root_path ("databases", "search.sqlite");
  filter_url_unlink (file);
  create ();
  return true;
}


// Updates the search fields for a chapter in Bible $name.
void Database_Search::updateSearchFields (string name, int book, int chapter)
{
  Database_Bibles database_bibles = Database_Bibles ();
  Database_Config_Bible database_config_bible = Database_Config_Bible ();

  string usfm = database_bibles.getChapter (name, book, chapter);
  string stylesheet = database_config_bible.getExportStylesheet (name);

  // Data to store.
  map <int, string> usfmraw;
  map <int, string> usfmlower;
  map <int, string> plainraw;
  map <int, string> plainlower;

  // Get the verses in the chapter.
  vector <int> verses = usfm_get_verse_numbers (usfm);
  set <int> uverses (verses.begin(), verses.end ());
  verses.assign (uverses.begin(), uverses.end ());
  sort (verses.begin (), verses.end());

  // One column contains the raw USFM as it is, and another one the lowercase text.
  for (auto verse : verses) {
    string raw = usfm_get_verse_text (usfm, verse);
    string lower = unicode_string_casefold (raw);
    usfmraw [verse] = raw;
    usfmlower [verse] = lower;
  }
  
  // Text filter for getting the plain text.
  Filter_Text filter_text = Filter_Text (name);
  filter_text.text_text = new Text_Text ();
  filter_text.initializeHeadingsAndTextPerVerse ();
  filter_text.addUsfmCode (usfm);
  filter_text.run (stylesheet);

  // Get the clean verse texts.
  map <int, string> texts = filter_text.getVersesText ();
  for (auto & element : texts) {
    plainraw [element.first].append (element.second + "\n");
  }

  // Add any clean headings.
  map <int, string> headings = filter_text.verses_headings;
  for (auto & element : headings) {
    plainraw [element.first].append (element.second + "\n");
  }
  
  // Create the lower case plain text.
  for (auto & element : plainraw) {
    string raw = element.second;
    string lower = unicode_string_casefold (raw);
    plainlower [element.first] = lower;
  }
  
  // Get all possible verses.
  vector <int> allverses;
  for (auto & element : usfmraw) allverses.push_back (element.first);
  for (auto & element : plainraw) allverses.push_back (element.first);
  {
    set <int> uallverses (allverses.begin(), allverses.end());
    allverses.assign (uallverses.begin(), uallverses.end());
  }
  sort (allverses.begin (), allverses.end ());

  // Store everything.
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "BEGIN;");
  SqliteSQL sql;
  sql.add ("DELETE FROM bibles WHERE bible =");
  sql.add (name);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add (";");
  database_sqlite_exec (db, sql.sql);
  for (int verse : allverses) {
    SqliteSQL sql;
    sql.add ("INSERT INTO bibles (bible, book, chapter, verse, usfmraw, usfmlower, plainraw, plainlower) VALUES (");
    sql.add (name);
    sql.add (", ");
    sql.add (book);
    sql.add (", ");
    sql.add (chapter);
    sql.add (", ");
    sql.add (verse);
    sql.add (", ");
    sql.add (usfmraw [verse]);
    sql.add (", ");
    sql.add (usfmlower [verse]);
    sql.add (", ");
    sql.add (plainraw [verse]);
    sql.add (", ");
    sql.add (plainlower [verse]);
    sql.add (");");
    database_sqlite_exec (db, sql.sql);
  }
  database_sqlite_exec (db, "COMMIT;");
  database_sqlite_disconnect (db);
}


/*
* Searches the text of the Bibles.
* Returns an array with the rowid's of matching verses.
* search: Contains the text to search for.
* bibles: Array of Bible names to search in.
*/
vector <int> Database_Search::searchText (string search, vector <string> bibles)
{
  if (search == "") return { };

  search = unicode_string_casefold (search);
  search = filter_string_str_replace (",", "", search);
  
  SqliteSQL sql;
  sql.add ("SELECT rowid FROM bibles WHERE");
  if (!bibles.empty()) {
    sql.add ("(");
    for (unsigned int i = 0; i < bibles.size(); i++) {
      if (i > 0) sql.add ("OR");
      sql.add ("bible =");
      sql.add (bibles[i]);
    }
    sql.add (")");
  } else {
    sql.add ("1");
  }
  sql.add ("AND plainlower LIKE");
  search.insert (0, "%");
  search.append ("%");
  sql.add (search);
  sql.add (";");
 
  vector <int> ids;
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["rowid"];
  for (auto & id : result) ids.push_back (convert_to_int (id));
  database_sqlite_disconnect (db);
  
  return ids;
}


/*
* Performs a case-insensitive search of the text of one bible.
* Returns an array with the rowid's of matching verses.
* search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleText (string bible, string search)
{
  search = unicode_string_casefold (search);

  SqliteSQL sql;
  sql.add ("SELECT rowid FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND plainlower LIKE");
  search.insert (0, "%");
  search.append ("%");
  sql.add (search);

  vector <int> ids;
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["rowid"];
  for (auto & id : result) ids.push_back (convert_to_int (id));
  database_sqlite_disconnect (db);
  
  return ids;
}


/*
* Performs a case-sensitive search of the text of one bible.
* Returns an array with the rowid's of matching verses.
* search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleTextCaseSensitive (string bible, string search)
{
  sqlite3 * db = connect ();

  database_sqlite_exec (db, "PRAGMA case_sensitive_like = true;");

  SqliteSQL sql;
  sql.add ("SELECT rowid FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND plainraw LIKE");
  search.insert (0, "%");
  search.append ("%");
  sql.add (search);
  sql.add (";");

  vector <int> ids;
  vector <string> result = database_sqlite_query (db, sql.sql) ["rowid"];
  for (auto & id : result) ids.push_back (convert_to_int (id));

  database_sqlite_exec (db, "PRAGMA case_sensitive_like = false;");

  database_sqlite_disconnect (db);

  return ids;
}


/*
* Performs a case-insensitive search of the USFM of one bible.
* Returns an array with the rowid's of matching verses.
* search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleUsfm (string bible, string search)
{
  search = unicode_string_casefold (search);
  
  SqliteSQL sql;
  sql.add ("SELECT rowid FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND usfmlower LIKE");
  search.insert (0, "%");
  search.append ("%");
  sql.add (search);
  sql.add (";");
  
  sqlite3 * db = connect ();
  vector <int> ids;
  vector <string> result = database_sqlite_query (db, sql.sql) ["rowid"];
  for (auto & id : result) ids.push_back (convert_to_int (id));
  database_sqlite_disconnect (db);
  
  return ids;
}


/*
* Performs a case-sensitive search of the USFM of one bible.
* Returns an array with the rowid's of matching verses.
* search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleUsfmCaseSensitive (string bible, string search)
{
  sqlite3 * db = connect ();

  database_sqlite_exec (db, "PRAGMA case_sensitive_like = true;");

  SqliteSQL sql;
  sql.add ("SELECT rowid FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND usfmraw LIKE");
  search.insert (0, "%");
  search.append ("%");
  sql.add (search);
  sql.add (";");

  vector <int> ids;
  vector <string> result = database_sqlite_query (db, sql.sql) ["rowid"];
  for (auto & id : result) ids.push_back (convert_to_int (id));

  database_sqlite_exec (db, "PRAGMA case_sensitive_like = false;");

  database_sqlite_disconnect (db);
  
  return ids;
}


// Returns the Bible and passage for a rowid.
Passage Database_Search::getBiblePassage (int rowid)
{
  Passage passage = Passage ();
  SqliteSQL sql;
  sql.add ("SELECT bible, book, chapter, verse FROM bibles WHERE rowid =");
  sql.add (rowid);
  sql.add (";");

  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  if (!result.empty ()) {
    passage.bible = result ["bible"] [0];
    passage.book = convert_to_int (result ["book"] [0]);
    passage.chapter = convert_to_int (result ["chapter"] [0]);
    passage.verse = result ["verse"] [0];
  }
  database_sqlite_disconnect (db);

  return passage;
}


// Gets the plain raw text for the bible and passage given.  
string Database_Search::getBibleVerseText (string bible, int book, int chapter, int verse)
{
  SqliteSQL sql;
  sql.add ("SELECT plainraw FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("AND verse =");
  sql.add (verse);
  sql.add (";");
  string plainraw;
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["plainraw"];
  if (!result.empty ()) plainraw = result [0];
  database_sqlite_disconnect (db);
  return plainraw;
}


// Gets the raw USFM for the bible and passage given.
string Database_Search::getBibleVerseUsfm (string bible, int book, int chapter, int verse)
{
  SqliteSQL sql;
  sql.add ("SELECT usfmraw FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("AND verse =");
  sql.add (verse);
  sql.add (";");
  string usfmraw;
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["plainraw"];
  if (!result.empty ()) usfmraw = result [0];
  database_sqlite_disconnect (db);
  return usfmraw;
}


vector <string> Database_Search::getBibles ()
{
  sqlite3 * db = connect ();
  vector <string> bibles = database_sqlite_query (db, "SELECT DISTINCT bible FROM bibles ORDER BY bible ASC;") ["bible"];
  database_sqlite_disconnect (db);
  return bibles;
}


void Database_Search::deleteBible (string bible)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


vector <int> Database_Search::getBooks (string bible)
{
  vector <int> books;
  SqliteSQL sql;
  sql.add ("SELECT DISTINCT book FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("ORDER BY book ASC;");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["book"];
  for (auto & book : result) books.push_back (convert_to_int (book));
  return books;
}


void Database_Search::deleteBook (string bible, int book)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


vector <int> Database_Search::getChapters (string bible, int book)
{
  vector <int> chapters;
  SqliteSQL sql;
  sql.add ("SELECT DISTINCT chapter FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("ORDER BY chapter ASC;");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["chapter"];
  database_sqlite_disconnect (db);
  for (auto & chapter : result) chapters.push_back (convert_to_int (chapter));
  return chapters;
}


void Database_Search::deleteChapter (string bible, int book, int chapter)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


vector <int> Database_Search::getVerses (string bible, int book, int chapter)
{
  vector <int> verses;
  SqliteSQL sql;
  sql.add ("SELECT DISTINCT verse FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("ORDER BY chapter ASC;");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["verse"];
  for (auto & verse : result) verses.push_back (convert_to_int (verse));
  database_sqlite_disconnect (db);
  return verses;
}


int Database_Search::getVerseCount (string bible)
{
  SqliteSQL sql;
  sql.add ("SELECT count(*) FROM bibles WHERE bible =");
  sql.add (bible);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["count(*)"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) {
    return convert_to_int (result [0]);
  }
  return 0;
}


