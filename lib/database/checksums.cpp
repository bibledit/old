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


#include <database/checksums.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>


// Database resilience: It is recreated every night.


void Database_Checksums::create ()
{
  if (!database_sqlite_healthy (name ())) {
    filter_url_unlink (database_sqlite_file (name ())); // Todo test health.
  }
  sqlite3 * db = connect ();
  string sql = 
    "CREATE TABLE IF NOT EXISTS notes ("
    " first integer,"
    " last integer,"
    " value text"
    ");";
  database_sqlite_exec (db, sql);
  sql = "DELETE FROM notes;";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_Checksums::putNotes (int first, int last, const string& checksum) // Todo test it.
{
  sqlite3 * db = connect ();
  {
    // Remove possible existing range.
    SqliteSQL sql = SqliteSQL ();
    sql.add ("DELETE FROM notes WHERE first =");
    sql.add (first);
    sql.add ("AND last =");
    sql.add (last);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  {
    // Store the new checksum for the range.
    SqliteSQL sql = SqliteSQL ();
    sql.add ("INSERT INTO notes VALUES (");
    sql.add (first);
    sql.add (",");
    sql.add (last);
    sql.add (",");
    sql.add (checksum);
    sql.add (");");
    database_sqlite_exec (db, sql.sql);
  }
  database_sqlite_disconnect (db);
}


string Database_Checksums::getNotes (int first, int last) // Todo test it.
{
  // Receive the checksum for the exact range.
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT value FROM notes WHERE first =");
  sql.add (first);
  sql.add ("AND last =");
  sql.add (last);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> values = database_sqlite_query (db, sql.sql)["value"];
  database_sqlite_disconnect (db);
  for (auto value : values) {
    return value;
  }
  return "";
}


void Database_Checksums::eraseNote (int identifier) // Todo test it.
{
  // Remove ranges that contain the note identifier.
  sqlite3 * db = connect ();
  SqliteSQL sql = SqliteSQL ();
  sql.add ("DELETE FROM notes WHERE first <=");
  sql.add (identifier);
  sql.add ("AND last >=");
  sql.add (identifier);
  sql.add (";");
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


const char * Database_Checksums::name ()
{
  return "checksums";
}


sqlite3 * Database_Checksums::connect ()
{
  return database_sqlite_connect (name ());
}
