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


#include <database/check.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <database/sqlite.h>


// Database resilience.
// Write operations to table "suppress" are very infrequent.
// Write operations to table "output" happen every night.
// In case of database corruption, running the Bibledit setup re-creates
// this table. The table does not contain important data.
// In cases of extreme corruption, the database file should be manually removed
// before running setup.


sqlite3 * Database_Check::connect ()
{
  return database_sqlite_connect ("check");
}


void Database_Check::create ()
{
  sqlite3 * db = connect ();
  string sql = "CREATE TABLE IF NOT EXISTS output ("
               " bible integer,"
               " book integer,"
               " chapter integer,"
               " verse integer,"
               " data text"
               ");";
  database_sqlite_exec (db, sql);
  sql =        "CREATE TABLE IF NOT EXISTS suppress ("
               " bible integer,"
               " book integer,"
               " chapter integer,"
               " verse integer,"
               " data text"
               ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_Check::optimize ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "VACUUM output;");
  database_sqlite_exec (db, "VACUUM suppress;");
  database_sqlite_disconnect (db);
}


void Database_Check::truncateOutput (string bible)
{
  sqlite3 * db = connect ();
  if (bible == "") {
    database_sqlite_exec (db, "DELETE FROM output;");
  } else {
    Database_Bibles database_bibles = Database_Bibles ();
    int bible_id = database_bibles.getID (bible);
    SqliteSQL sql = SqliteSQL ();
    sql.add ("DELETE FROM output WHERE bible =");
    sql.add (bible_id);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  database_sqlite_disconnect (db);
}


void Database_Check::recordOutput (string bible, int book, int chapter, int verse, string data) // Todo
{
  Database_Bibles database_bibles = Database_Bibles ();
  int bible_id = database_bibles.getID (bible);
  int count = 0;
  sqlite3 * db = connect ();
  // Check whether this is a suppressed item.
  // If it was suppressed, do not record it.
  {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("SELECT count(*) FROM suppress WHERE bible =");
    sql.add (bible_id);
    sql.add ("AND book = ");
    sql.add (book);
    sql.add ("AND chapter = ");
    sql.add (chapter);
    sql.add ("AND verse = ");
    sql.add (verse);
    sql.add ("AND data = ");
    sql.add (data);
    sql.add (";");
    sqlite3 * db = connect ();
    vector <string> result = database_sqlite_query (db, sql.sql) ["count(*)"];
    database_sqlite_disconnect (db);
    if (!result.empty ()) {
      count = convert_to_int (result [0]);
    }
  }
  if (count == 0) {
    // Check how often $data has been recorded already.
    {
      SqliteSQL sql = SqliteSQL ();
      sql.add ("SELECT count(*) FROM output WHERE bible =");
      sql.add (bible_id);
      sql.add ("AND data = ");
      sql.add (data);
      sql.add (";");
      sqlite3 * db = connect ();
      vector <string> result = database_sqlite_query (db, sql.sql) ["count(*)"];
      database_sqlite_disconnect (db);
      if (!result.empty ()) {
        count = convert_to_int (result [0]);
      }
    }
    // Record the data no more than so often.
    if (count < 10) {
      SqliteSQL sql = SqliteSQL ();
      sql.add ("INSERT INTO output VALUES (");
      sql.add (bible_id);
      sql.add (",");
      sql.add (book);
      sql.add (",");
      sql.add (chapter);
      sql.add (",");
      sql.add (verse);
      sql.add (",");
      sql.add (data);
      sql.add (");");
      database_sqlite_exec (db, sql.sql);
    }
    // Store message saying that no more of this messages will be stored.
    // This is for situations where the checks produce thousands and thousands of results.
    // That would choke the entire server.
    // It has been seen on a service provider that its system shut Bibledit's server down
    // due to excessive CPU usage during a long time.
    if (count == 9) {
      data.append (" (displaying no more of these)");
      SqliteSQL sql = SqliteSQL ();
      sql.add ("INSERT INTO output VALUES (");
      sql.add (bible_id);
      sql.add (",");
      sql.add (book);
      sql.add (",");
      sql.add (chapter);
      sql.add (",");
      sql.add (verse);
      sql.add (",");
      sql.add (data);
      sql.add (");");
      database_sqlite_exec (db, sql.sql);
    }
  }
  database_sqlite_disconnect (db);
}


vector <Database_Check_Hit> Database_Check::getHits () // Todo easier selection mechanism: Read from output only.
{
  vector <Database_Check_Hit> hits;
  string sql = "SELECT rowid, bible, book, chapter, verse, data FROM output;";
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql);
  database_sqlite_disconnect (db);
  vector <string> rowids = result ["rowid"];
  vector <string> bibles = result ["bible"];
  vector <string> books = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> verses = result ["verse"];
  vector <string> data = result ["data"];
  for (unsigned int i = 0; i < rowids.size(); i++) {
    Database_Check_Hit hit = Database_Check_Hit ();
    hit.rowid = convert_to_int (rowids [i]);
    hit.bible = convert_to_int (bibles [i]);
    hit.book = convert_to_int (books [i]);
    hit.chapter = convert_to_int (chapters [i]);
    hit.verse = convert_to_int (verses [i]);
    hit.data = data [i];
    hits.push_back (hit);
  }
  return hits;
}


void Database_Check::approve (int id) // Todo move from output to suppress. No copy.
{
  // The query moves all values, apart from the auto_increment id.
  sqlite3 * db = connect ();
  {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("INSERT INTO suppress (bible, book, chapter, verse, data) SELECT bible, book, chapter, verse, data FROM output WHERE rowid =");
    sql.add (id);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("DELETE FROM output WHERE rowid =");
    sql.add (id);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  database_sqlite_disconnect (db);
}


void Database_Check::erase (int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("DELETE FROM output WHERE rowid =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


Passage Database_Check::getPassage (int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT book, chapter, verse FROM output WHERE rowid =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  vector <string> books = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> verses = result ["verse"];
  for (unsigned int i = 0; i < books.size(); i++) {
    Passage passage = Passage ("", convert_to_int (books[i]), convert_to_int (chapters[i]), verses[i]);
    return passage;
  }
  return Passage ("", 0, 0, "");
}


vector <Database_Check_Hit> Database_Check::getSuppressions ()
{
  vector <Database_Check_Hit> hits;
  string sql = "SELECT rowid, bible, book, chapter, verse, data FROM suppress;";
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql);
  database_sqlite_disconnect (db);
  vector <string> rowids = result ["rowid"];
  vector <string> bibles = result ["bible"];
  vector <string> books = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> verses = result ["verse"];
  vector <string> data = result ["data"];
  for (unsigned int i = 0; i < rowids.size(); i++) {
    Database_Check_Hit hit = Database_Check_Hit ();
    hit.rowid = convert_to_int (rowids [i]);
    hit.bible = convert_to_int (bibles [i]);
    hit.book = convert_to_int (books [i]);
    hit.chapter = convert_to_int (chapters [i]);
    hit.verse = convert_to_int (verses [i]);
    hit.data = data [i];
    hits.push_back (hit);
  }
  return hits;
}


void Database_Check::release (int id) // Todo move from suppress back to the output table.
{
  sqlite3 * db = connect ();
  {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("INSERT INTO output (bible, book, chapter, verse, data) SELECT bible, book, chapter, verse, data FROM suppress WHERE rowid =");
    sql.add (id);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("DELETE FROM suppress WHERE rowid =");
    sql.add (id);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  database_sqlite_disconnect (db);
}

