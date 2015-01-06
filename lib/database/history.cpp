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


#include <database/history.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>


// Database resilience: It is written to during the night.
// If corruption occurs, the history is lost.


Database_History::Database_History ()
{
}


Database_History::~Database_History ()
{
}


sqlite3 * Database_History::connect ()
{
  return database_sqlite_connect ("history");
}


void Database_History::create ()
{
  sqlite3 * db = connect ();

  string sql = 
    "CREATE TABLE IF NOT EXISTS history ("
    "  timestamp integer,"
    "  author text,"
    "  bible text,"
    "  book integer,"
    "  chapter integer,"
    "  verse integer,"
    "  oldtext text,"
    "  modification text,"
    "  newtext text"
    ");";
  database_sqlite_exec (db, sql);
  
  sql = "CREATE INDEX IF NOT EXISTS author ON history (author)";
  database_sqlite_exec (db, sql);

  sql = "CREATE INDEX IF NOT EXISTS timestamp ON history (timestamp)";
  database_sqlite_exec (db, sql);

  sql = "CREATE INDEX IF NOT EXISTS bible ON history (bible)";
  database_sqlite_exec (db, sql);

  sql = "CREATE INDEX IF NOT EXISTS passage ON history (book, chapter, verse)";
  database_sqlite_exec (db, sql);
  
  database_sqlite_disconnect (db);
}


void Database_History::optimize ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "REINDEX history;");
  database_sqlite_exec (db, "VACUUM history;");
  database_sqlite_disconnect (db);
}


void Database_History::trim ()
{
  // Delete older items.
  //time = strtotime ("-6 months");
  //query = "DELETE FROM history WHERE timestamp < time;";
  //Database_SQLite::exec (this->db, query);
}


void Database_History::record (int timestamp, string author, string bible, int book, int chapter, int verse, string oldtext, string modification, string newtext)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("INSERT INTO history VALUES (");
  sql.add (timestamp);
  sql.add (",");
  sql.add (author);
  sql.add (",");
  sql.add (bible);
  sql.add (",");
  sql.add (book);
  sql.add (",");
  sql.add (chapter);
  sql.add (",");
  sql.add (verse);
  sql.add (",");
  sql.add (oldtext);
  sql.add (",");
  sql.add (modification);
  sql.add (",");
  sql.add (newtext);
  sql.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


int Database_History::count (string author, vector <string> bibles, int book, int chapter, int verse)
{
  string sql = query (author, bibles, book, chapter, verse, true, 0);
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["COUNT(*)"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) {
    return convert_to_int (result [0]);
  }
  return 0;
}


// Gets an entry.
// Variable start indicates where to start in the database.
vector <Database_History_Item> Database_History::get (string author, string bible, int book, int chapter, int verse, int start)
{
  vector <Database_History_Item> items;
  string sql = query (author, {bible}, book, chapter, verse, false, start);
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql);
  database_sqlite_disconnect (db);
  vector <string> timestamps = result ["timestamp"];
  vector <string> authors = result ["author"];
  vector <string> bibles = result ["bible"];
  vector <string> books = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> verses = result ["verse"];
  vector <string> oldtexts = result ["oldtext"];
  vector <string> modifications = result ["modification"];
  vector <string> newtexts = result ["newtext"];
  for (unsigned int i = 0; i < timestamps.size(); i++) {
    Database_History_Item item = Database_History_Item ();
    item.timestamp = convert_to_int (timestamps[i]);
    item.author = authors[i];
    item.bible = bibles[i];
    item.book = convert_to_int (books[i]);
    item.chapter = convert_to_int (chapters[i]);
    item.verse = convert_to_int (verses[i]);
    item.oldtext = oldtexts[i];
    item.modification = modifications[i];
    item.newtext = newtexts[i];
    items.push_back (item);
  }
  return items;
}


// This function returns an array of authors in the history database.
// The search is limited to the Bibles in array bibles.
// The authors are sorted by the number of modifications made by them.
vector <string> Database_History::authors (vector <string> bibles)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT author, COUNT(*) as count FROM history");
  sql.add ("WHERE");
  for (unsigned int i = 0; i < bibles.size(); i++) {
    if (i > 0) sql.add ("OR");
    sql.add ("bible =");
    sql.add (bibles[i]);
  }
  sql.add ("GROUP BY author ORDER BY count DESC;");
  sqlite3 * db = connect ();
  vector <string> authors = database_sqlite_query (db, sql.sql) ["author"];
  database_sqlite_disconnect (db);
  return authors;
}


string Database_History::query (string author, vector <string> bibles, int book, int chapter, int verse, bool count, int start)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT");
  if (count) sql.add ("COUNT(*)");
  else sql.add ("timestamp, author, bible, book, chapter, verse, oldtext, modification, newtext");
  sql.add ("FROM history WHERE 1");
  if (author != "") {
    sql.add ("AND author =");
    sql.add (author);
  }
  if (!bibles.empty()) {
    sql.add ("AND");
    sql.add ("(");
    for (unsigned int i = 0; i < bibles.size(); i++) {
      if (i > 0) sql.add ("OR");
      sql.add ("bible =");
      sql.add (bibles[i]);
    }
    sql.add (")");
  }
  if (book > 0) {
    sql.add ("AND book =");
    sql.add (book);
  }
  if (chapter > -1) {
    sql.add ("AND chapter =");
    sql.add (chapter);
  }
  if (verse > -1) {
    sql.add ("AND verse =");
    sql.add (verse);
  }
  if (!count) sql.add ("ORDER BY timestamp DESC");
  if (start >= 1) {
    sql.add ("LIMIT");
    sql.add (start);
    sql.add (", 50");
  }
  sql.add (";");
  return sql.sql;
}

