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


#include <database/commits.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <database/sqlite.h>


// Database resilience. It is only written to during the night.
// In case of corruption, some information is lost.
// Remove the database file, and re-run setup to correct the problem.


Database_Commits::Database_Commits ()
{
}


Database_Commits::~Database_Commits ()
{
}


sqlite3 * Database_Commits::connect ()
{
  return database_sqlite_connect ("commits");
}


void Database_Commits::create ()
{
  sqlite3 * db = connect ();
  string sql = "CREATE TABLE IF NOT EXISTS commits ("
               " bible text,"
               " sha1 text"
               ");";
  database_sqlite_exec (db, sql);
  database_sqlite_exec (db, "CREATE INDEX IF NOT EXISTS bible ON commits (bible)");
  database_sqlite_exec (db, "CREATE INDEX IF NOT EXISTS sha1 ON commits (sha1)");
  database_sqlite_disconnect (db);
}


void Database_Commits::optimize ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "REINDEX commits;");
  database_sqlite_exec (db, "VACUUM commits;");
  database_sqlite_disconnect (db);
}


void Database_Commits::record (string bible, string sha1)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("INSERT INTO commits VALUES (");
  sql.add (bible);
  sql.add (",");
  sql.add (sha1);
  sql.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


vector <string> Database_Commits::get (string bible)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT sha1 FROM commits WHERE bible =");
  sql.add (bible);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> commits = database_sqlite_query (db, sql.sql) ["sha1"];
  database_sqlite_disconnect (db);
  return commits;
}


