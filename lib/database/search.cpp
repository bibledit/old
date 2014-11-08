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


#include <database/search.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <database/sqlite.h>
#include <database/bibles.h>
#include <database/config/bible.h>


/*

Database resilience: The index is re-created every night.

Suggestion: Improved searching on bible and notes, also using concepts like 'near' and 'relevance' and 'score'.

*/


Database_Search::Database_Search ()
{
}


Database_Search::~Database_Search ()
{
}


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
  sql = "CREATE INDEX IF NOT EXISTS speedup ON bibles (bible, book, chapter)";
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
  /* C++Port


  // Data to store.
  $usfmraw = array ();
  $usfmlower = array ();
  $plainraw = array ();
  $plainlower = array ();

  // Get the verses in the chapter.
  $verses = Filter_Usfm::getVerseNumbers ($usfm);
  $verses = array_unique ($verses);
  sort ($verses, SORT_NUMERIC);

  // One column contains the raw USFM as it is, and another one the lowercase text.
  foreach ($verses as $verse) {
    $raw = Filter_Usfm::getVerseText ($usfm, $verse);
    $lower = mb_convert_case ($raw, MB_CASE_LOWER);
    $usfmraw [$verse] = $raw;
    $usfmlower [$verse] = $lower;
  }
  
  // Text filter for getting the plain text.
  $filter_text = new Filter_Text ($name);
  $filter_text->text_text = new Text_Text ();
  $filter_text->initializeHeadingsAndTextPerVerse ();
  $filter_text->addUsfmCode ($usfm);
  $filter_text->run ($stylesheet);

  // Get the clean verse texts.
  $texts = $filter_text->getVersesText ();
  foreach ($texts as $verse => $text) {
    if (!isset ($plainraw [$verse])) $plainraw [$verse] = "";
    $plainraw [$verse] .= "$text\n";
  }

  // Add any clean headings.
  $headings = $filter_text->verses_headings;
  foreach ($headings as $verse => $heading) {
    if (!isset ($plainraw [$verse])) $plainraw [$verse] = "";
    $plainraw [$verse] .= "$heading\n";
  }
  
  // Create the lower case plain text.
  foreach ($plainraw as $verse => $raw) {
    $plainlower [$verse] = mb_convert_case ($raw, MB_CASE_LOWER);
  }
  
  // Get all possible verses.
  $allverses = array_merge (array_keys ($usfmraw), array_keys ($plainraw));
  $allverses = array_unique ($allverses);

  // Store everything.
  Database_SQLite::exec ($this->db, "BEGIN;");
  $name = Database_SQLiteInjection::no ($name);
  $book = Database_SQLiteInjection::no ($book);
  $chapter = Database_SQLiteInjection::no ($chapter);
  $query = "DELETE FROM bibles WHERE bible = '$name' AND book = $book AND chapter = $chapter;";
  Database_SQLite::exec ($this->db, $query);
  foreach ($allverses as $verse) {
    if ($verse == "") $verse = 0;
    $verse = (integer) $verse;
    @$ur = Database_SQLiteInjection::no ($usfmraw [$verse]);
    @$ul = Database_SQLiteInjection::no ($usfmlower [$verse]);
    @$pr = Database_SQLiteInjection::no ($plainraw [$verse]);
    @$pl = Database_SQLiteInjection::no ($plainlower [$verse]);
    $query = "INSERT INTO bibles (bible, book, chapter, verse, usfmraw, usfmlower, plainraw, plainlower) VALUES ('$name', $book, $chapter, $verse, '$ur', '$ul', '$pr', '$pl');";
    Database_SQLite::exec ($this->db, $query);
  }
  Database_SQLite::exec ($this->db, "COMMIT;");
  */
}


/*
* Searches the text of the Bibles.
* Returns an array with the rowid's of matching verses.
* $search: Contains the text to search for.
* $bibles: Array of Bible names to search in.
*/
vector <int> Database_Search::searchText (string search, vector <string> bibles)
{
  if (bibles.empty()) cout << search;
  /* C++Port
  $ids = array ();

  if ($search == "") {
    return $ids;
  }

  $search = mb_convert_case ($search, MB_CASE_LOWER);
  $search = str_replace (",", "", $search);
  $search = Database_SQLiteInjection::no ($search);

  $bibleCondition = "1";
  if (!empty ($bibles)) {
    $bibleCondition = " ( ";
    foreach ($bibles as $offset => $bible) {
      if ($offset) $bibleCondition .= " OR ";
      $bible = Database_SQLiteInjection::no ($bible);
      $bibleCondition .= " bible = '$bible' ";
    }
    $bibleCondition .= " ) ";
  }

  $query = "SELECT rowid FROM bibles WHERE $bibleCondition AND plainlower LIKE '%$search%';";
  $hits = array ();
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $hits [] = $row [0];
  }
  
  return $hits;
  */
  return {};
}


/*
* Performs a case-insensitive search of the text of one $bible.
* Returns an array with the rowid's of matching verses.
* $search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleText (string bible, string search)
{
  cout << bible << search << endl;
  /* C++Port
  $ids = array ();

  $bible = Database_SQLiteInjection::no ($bible);

  $search = mb_convert_case ($search, MB_CASE_LOWER);
  $search = Database_SQLiteInjection::no ($search);

  $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND plainlower LIKE '%$search%';";
  $hits = array ();
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $hits [] = $row [0];
  }
  
  return $hits;
  */
  return {};
}


/*
* Performs a case-sensitive search of the text of one $bible.
* Returns an array with the rowid's of matching verses.
* $search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleTextCaseSensitive (string bible, string search)
{
  cout << bible << search << endl;
  /* C++Port
  $ids = array ();

  $bible = Database_SQLiteInjection::no ($bible);
  $search = Database_SQLiteInjection::no ($search);

  $query = "PRAGMA case_sensitive_like = true;";
  Database_SQLite::exec ($this->db, $query);
  
  $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND plainraw LIKE '%$search%';";
  $hits = array ();
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $hits [] = $row [0];
  }

  $query = "PRAGMA case_sensitive_like = false;";
  Database_SQLite::exec ($this->db, $query);
  
  return $hits;
  */
  return {};
}


/*
* Performs a case-insensitive search of the USFM of one $bible.
* Returns an array with the rowid's of matching verses.
* $search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleUsfm (string bible, string search)
{
  cout << bible << search << endl;
  /* C++Port
  $ids = array ();

  $bible = Database_SQLiteInjection::no ($bible);

  $search = mb_convert_case ($search, MB_CASE_LOWER);
  $search = Database_SQLiteInjection::no ($search);

  $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND usfmlower LIKE '%$search%';";
  $hits = array ();
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $hits [] = $row [0];
  }
  
  return $hits;
  */
  return {};
}


/*
* Performs a case-sensitive search of the USFM of one $bible.
* Returns an array with the rowid's of matching verses.
* $search: Contains the text to search for.
*/
vector <int> Database_Search::searchBibleUsfmCaseSensitive (string bible, string search)
{
  cout << bible << search << endl;
  /* C++Port
  $ids = array ();

  $bible = Database_SQLiteInjection::no ($bible);
  $search = Database_SQLiteInjection::no ($search);

  $query = "PRAGMA case_sensitive_like = true;";
  Database_SQLite::exec ($this->db, $query);
  
  $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND usfmraw LIKE '%$search%';";
  $hits = array ();
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $hits [] = $row [0];
  }

  $query = "PRAGMA case_sensitive_like = false;";
  Database_SQLite::exec ($this->db, $query);
  
  return $hits;
  */
  return {};
}


// Returns the Bible and passage for a $rowid.
map <string, string> Database_Search::getBiblePassage (int rowid)
{
  cout << rowid << endl;
  /* C++Port
  $rowid = Database_SQLiteInjection::no ($rowid);
  $query = "SELECT bible, book, chapter, verse FROM bibles WHERE rowid = $rowid;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    unset ($row [0]);
    unset ($row [1]);
    unset ($row [2]);
    unset ($row [3]);
    return $row;
  }
  return NULL;
  */
  return {};
}


// Gets the plain raw text for the $bible and passage given.  
string Database_Search::getBibleVerseText (string bible, int book, int chapter, int verse)
{
  cout << bible <<  book << chapter << verse << endl;
  /* C++Port
  $bible = Database_SQLiteInjection::no ($bible);
  $book = Database_SQLiteInjection::no ($book);
  $chapter = Database_SQLiteInjection::no ($chapter);
  $verse = Database_SQLiteInjection::no ($verse);
  $query = "SELECT plainraw FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    return $row[0];
  }
  */
  return "";
}


// Gets the raw USFM for the $bible and passage given.
string Database_Search::getBibleVerseUsfm (string bible, int book, int chapter, int verse)
{
  cout << bible <<  book << chapter << verse << endl;
  /* C++Port
  $bible = Database_SQLiteInjection::no ($bible);
  $book = Database_SQLiteInjection::no ($book);
  $chapter = Database_SQLiteInjection::no ($chapter);
  $verse = Database_SQLiteInjection::no ($verse);
  $query = "SELECT usfmraw FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    return $row[0];
  }
  */
  return "";
}


vector <string> Database_Search::getBibles ()
{
  /* C++Port
  $bibles = array ();
  $query = "SELECT DISTINCT bible FROM bibles ORDER BY bible ASC;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $bibles [] = $row[0];
  }
  return $bibles;
  */
  return { };
}


void Database_Search::deleteBible (string bible)
{
  cout << bible << endl;
  /* C++Port
  $bible = Database_SQLiteInjection::no ($bible);
  $query = "DELETE FROM bibles WHERE bible = '$bible';";
  Database_SQLite::exec ($this->db, $query);
  */
}


vector <int> Database_Search::getBooks (string bible)
{
  cout << bible << endl;
  /*
  $bible = Database_SQLiteInjection::no ($bible);
  $books = array ();
  $query = "SELECT DISTINCT book FROM bibles WHERE bible = '$bible' ORDER BY book ASC;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $books [] = $row[0];
  }
  return $books;
  */
  return { };
}


void Database_Search::deleteBook (string bible, int book)
{
  cout << bible <<  book << endl;
  /* Todo
  $bible = Database_SQLiteInjection::no ($bible);
  $book = Database_SQLiteInjection::no ($book);
  $query = "DELETE FROM bibles WHERE bible = '$bible' AND book = $book;";
  Database_SQLite::exec ($this->db, $query);
  */
}


vector <int> Database_Search::getChapters (string bible, int book)
{
  cout << bible <<  book << endl;
  /* Todo
  $bible = Database_SQLiteInjection::no ($bible);
  $book = Database_SQLiteInjection::no ($book);
  $chapters = array ();
  $query = "SELECT DISTINCT chapter FROM bibles WHERE bible = '$bible' AND book = $book ORDER BY chapter ASC;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $chapters [] = $row[0];
  }
  return $chapters;
  */
  return { };
}


void Database_Search::deleteChapter (string bible, int book, int chapter)
{
  cout << bible <<  book << chapter << endl;
  /* Todo
  $bible = Database_SQLiteInjection::no ($bible);
  $book = Database_SQLiteInjection::no ($book);
  $chapter = Database_SQLiteInjection::no ($chapter);
  $query = "DELETE FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter;";
  Database_SQLite::exec ($this->db, $query);
  */
}


vector <int> Database_Search::getVerses (string bible, int book, int chapter)
{
  cout << bible <<  book << chapter << endl;
  /*
  $bible = Database_SQLiteInjection::no ($bible);
  $book = Database_SQLiteInjection::no ($book);
  $chapter = Database_SQLiteInjection::no ($chapter);
  $verses = array ();
  $query = "SELECT DISTINCT verse FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter ORDER BY chapter ASC;";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    $verses [] = $row[0];
  }
  return $verses;
  */
  return { };
}


int Database_Search::getVerseCount (string bible)
{
  cout << bible << endl;
  /*
  $bible = Database_SQLiteInjection::no ($bible);
  $query = "SELECT count(*) FROM bibles WHERE bible = '$bible';";
  $result = Database_SQLite::query ($this->db, $query);
  foreach ($result as $row) {
    return $row[0];
  }
  */
  return 0;
}


