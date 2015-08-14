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


sqlite3 * Database_Strong::connectheb ()
{
  return database_sqlite_connect ("hebrewstrong");
}


// Get Strong's word definition for $strong's number.
string Database_Strong::word (string strong)
{
  return get (strong, "word");
}


// Get Strong's source definition for $strong's number.
string Database_Strong::source (string strong)
{
  return get (strong, "source");
}


// Get Strong's meaning definition for $strong's number.
string Database_Strong::meaning (string strong)
{
  return get (strong, "meaning");
}


// Get Strong's usage definition for $strong's number.
string Database_Strong::usage (string strong)
{
  return get (strong, "usage");
}


// Get Strong's $item definition for $strong's number.
string Database_Strong::get (string strong, string item)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT");
  sql.add (item.c_str ());
  sql.add ("FROM hebrewstrong WHERE id =");
  sql.add (strong);
  sql.add (";");
  sqlite3 * db = connectheb ();
  vector <string> items = database_sqlite_query (db, sql.sql) [item];
  database_sqlite_disconnect (db);
  if (!items.empty ()) return items[0];
  return "";
}
