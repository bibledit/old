/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <database/volatile.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>


// Database resilience: It is recreated every night.


sqlite3 * Database_Volatile::connect ()
{
  return database_sqlite_connect ("volatile");
}


void Database_Volatile::create ()
{
  sqlite3 * db = connect ();
  string sql = 
    "CREATE TABLE IF NOT EXISTS volatile ("
    " id integer,"
    " key text,"
    " value text"
    ");";
  database_sqlite_exec (db, sql);
  sql = "DELETE FROM volatile;";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


vector <string> Database_Volatile::getKeys (int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT key FROM volatile WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> keys = database_sqlite_query (db, sql.sql)["key"];
  database_sqlite_disconnect (db);
  return keys;
}


string Database_Volatile::getValue (int id, const string& key)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT value FROM volatile WHERE id =");
  sql.add (id);
  sql.add ("AND key =");
  sql.add (key);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> values = database_sqlite_query (db, sql.sql)["value"];
  database_sqlite_disconnect (db);
  for (auto value : values) {
    return value;
  }
  return "";
}


void Database_Volatile::setValue (int id, const string& key, const string& value)
{
  SqliteSQL sql1 = SqliteSQL ();
  sql1.add ("DELETE FROM volatile WHERE id =");
  sql1.add (id);
  sql1.add ("AND key =");
  sql1.add (key);
  sql1.add (";");
  SqliteSQL sql2 = SqliteSQL ();
  sql2.add ("INSERT INTO volatile VALUES (");
  sql2.add (id);
  sql2.add (",");
  sql2.add (key);
  sql2.add (",");
  sql2.add (value);
  sql2.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql1.sql);
  database_sqlite_exec (db, sql2.sql);
  database_sqlite_disconnect (db);
}

