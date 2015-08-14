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


#include <database/strong.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>


// This is the database for the Strong's definitions.
// Resilience: It is never written to. 
// Chances of corruption are nearly zero.


// Get Strong's definition for $strong's number.
string Database_Strong::get (string strong)
{
  bool greek = strong.find ("G") != string::npos;
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT definition FROM");
  if (greek) {
    sql.add ("greekstrong");
  } else {
    sql.add ("hebrewstrong");
  }
  sql.add ("WHERE id =");
  sql.add (strong);
  sql.add (";");
  sqlite3 * db;
  if (greek) {
    db = database_sqlite_connect ("greekstrong");
  } else {
    db = database_sqlite_connect ("hebrewstrong");
  }
  vector <string> definitions = database_sqlite_query (db, sql.sql) ["definition"];
  database_sqlite_disconnect (db);
  if (!definitions.empty ()) return definitions[0];
  return "";
}
