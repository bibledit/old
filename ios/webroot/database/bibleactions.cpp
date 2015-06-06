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


#include <database/bibleactions.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <database/sqlite.h>


Database_BibleActions::Database_BibleActions ()
{
}


Database_BibleActions::~Database_BibleActions ()
{
}


sqlite3 * Database_BibleActions::connect ()
{
  return database_sqlite_connect ("bibleactions");
}


void Database_BibleActions::create ()
{
  sqlite3 * db = connect ();
  string sql = "CREATE TABLE IF NOT EXISTS bibleactions ("
               " bible text,"
               " book integer,"
               " chapter integer,"
               " usfm text"
               ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_BibleActions::clear ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "DROP TABLE IF EXISTS bibleactions;");
  database_sqlite_disconnect (db);
}


void Database_BibleActions::optimize ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "VACUUM bibleactions;");
  database_sqlite_disconnect (db);
}


void Database_BibleActions::record (string bible, int book, int chapter, string usfm)
{
  if (getUsfm (bible, book, chapter).empty ()) {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("INSERT INTO bibleactions VALUES (");
    sql.add (bible);
    sql.add (",");
    sql.add (book);
    sql.add (",");
    sql.add (chapter);
    sql.add (",");
    sql.add (usfm);
    sql.add (");");
    sqlite3 * db = connect ();
    database_sqlite_exec (db, sql.sql);
    database_sqlite_disconnect (db);
  }
}


vector <string> Database_BibleActions::getBibles ()
{
  sqlite3 * db = connect ();
  vector <string> notes = database_sqlite_query (db, "SELECT DISTINCT bible FROM bibleactions ORDER BY bible;")["bible"];
  database_sqlite_disconnect (db);
  return notes;
}


vector <int> Database_BibleActions::getBooks (string bible)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT DISTINCT book FROM bibleactions WHERE bible =");
  sql.add (bible);
  sql.add ("ORDER BY book;");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql)["book"];
  database_sqlite_disconnect (db);
  vector <int> books;
  for (auto book : result) books.push_back (convert_to_int (book));
  return books;
}


vector <int> Database_BibleActions::getChapters (string bible, int book)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT DISTINCT chapter FROM bibleactions WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("ORDER BY chapter;");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql)["chapter"];
  database_sqlite_disconnect (db);
  vector <int> chapters;
  for (auto chapter : result) chapters.push_back (convert_to_int (chapter));
  return chapters;
}


string Database_BibleActions::getUsfm (string bible, int book, int chapter)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT usfm FROM bibleactions WHERE bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql)["usfm"];
  database_sqlite_disconnect (db);
  for (auto usfm : result) return usfm;
  return "";
}


void Database_BibleActions::erase (string bible, int book, int chapter)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("DELETE FROM bibleactions WHERE bible =");
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
 
