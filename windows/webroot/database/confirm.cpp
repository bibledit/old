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


#include <database/confirm.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>
#include <filter/date.h>


// Handles email and web page confirmations.
// Database resilience: It is used infrequently.
// It does not contain essential information.


Database_Confirm::Database_Confirm ()
{
}


Database_Confirm::~Database_Confirm ()
{
}


sqlite3 * Database_Confirm::connect ()
{
  return database_sqlite_connect ("confirm");
}


void Database_Confirm::create ()
{
  sqlite3 * db = connect ();
  string sql = "CREATE TABLE IF NOT EXISTS confirm ("
               " id integer,"
               " query text,"
               " timestamp integer,"
               " mailto text,"
               " subject text,"
               " body text"
               ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_Confirm::optimize ()
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "VACUUM confirm;");
  database_sqlite_disconnect (db);
}


// getNewID - returns a new unique confirmation ID as an integer
unsigned int Database_Confirm::getNewID ()
{
  unsigned int id = 0;
  do {
    id = filter_string_rand (100000000, 999999999);
  } while (IDExists (id));
  return id;
}


// Returns true if the $id exists
bool Database_Confirm::IDExists (unsigned int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT id FROM confirm WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> ids = database_sqlite_query (db, sql.sql) ["id"];
  database_sqlite_disconnect (db);
  return !ids.empty ();
}


// stores a confirmation cycle
void Database_Confirm::store (unsigned int id, string query, string to, string subject, string body)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("INSERT INTO confirm VALUES (");
  sql.add (id);
  sql.add (",");
  sql.add (query);
  sql.add (",");
  sql.add (filter_date_seconds_since_epoch ());
  sql.add (",");
  sql.add (to);
  sql.add (",");
  sql.add (subject);
  sql.add (",");
  sql.add (body);
  sql.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Search the database for an existing ID in $subject.
// If it exists, it returns the ID number, else it returns 0.
unsigned int Database_Confirm::searchID (string subject)
{
  sqlite3 * db = connect ();
  vector <string> ids = database_sqlite_query (db, "SELECT id FROM confirm;") ["id"];
  database_sqlite_disconnect (db);
  for (string id : ids) {
    size_t pos = subject.find (id);
    if (pos != string::npos) {
      return convert_to_int (id);
    }
  }
  return 0;
}


// Returns the query for $id.
string Database_Confirm::getQuery (unsigned int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT query FROM confirm WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["query"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) return result [0];
  return "";
}


// Returns the To: address for $id.
string Database_Confirm::getMailTo (unsigned int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT mailto FROM confirm WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["mailto"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) return result [0];
  return "";
}


// Returns the Subject: for $id.
string Database_Confirm::getSubject (unsigned int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT subject FROM confirm WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["subject"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) return result [0];
  return "";
}


// Returns the email's body for $id.
string Database_Confirm::getBody (unsigned int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT body FROM confirm WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["body"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) return result [0];
  return "";
}


// Deletes $id from the table.
void Database_Confirm::erase (unsigned int id)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("DELETE FROM confirm WHERE id =");
  sql.add (id);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


void Database_Confirm::trim ()
{
  // Leave entries for no more than 30 days.
  unsigned int time = filter_date_seconds_since_epoch () - 2592000; 
  SqliteSQL sql = SqliteSQL ();
  sql.add ("DELETE FROM confirm WHERE timestamp <");
  sql.add (time);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


