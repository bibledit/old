/*
Copyright (ƒ) 2003-2016 Teus Benschop.

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


#include <database/privileges.h>
#include <database/sqlite.h>
#include <filter/url.h>
#include <filter/string.h>


// This database is resilient.
// The data is stored in a SQLite database.
// This part is read often, and infrequently written to.
// Due to the infrequent write operations, there is a low and acceptable change of corruption.


// The name of the database.
const char * Database_Privileges::database ()
{
  return "privileges";
}


void Database_Privileges::create ()
{
  SqliteDatabase sql (database ());
  
  sql.add ("CREATE TABLE IF NOT EXISTS bibles ("
           " username text,"
           " bible text,"
           " book integer,"
           " write boolean"
           ");");
  sql.execute ();
  
  sql.add ("CREATE TABLE IF NOT EXISTS features ("
           " username text,"
           " feature integer"
           ");");
  sql.execute ();
}


void Database_Privileges::upgrade ()
{
}


void Database_Privileges::optimize ()
{
  // Recreate damaged database.
  if (!healthy ()) {
    filter_url_unlink (database_sqlite_file (database ()));
    create ();
  }
  // Vacuum it.
  SqliteDatabase sql (database ());
  sql.add ("VACUUM;");
  sql.execute ();
}


bool Database_Privileges::healthy ()
{
  return database_sqlite_healthy (database ());
}


// Give a privilege to a $username to access $bible $book to read it, or also to $write it.
// When the $book = 0, then the privilege is taken to apply to all possible books in the $bible.
void Database_Privileges::setBibleBook (string username, string bible, int book, bool write)
{
  // First remove any entry.
  removeBibleBook (username, bible, book);
  // Store the new entry.
  SqliteDatabase sql (database ());
  sql.add ("INSERT INTO bibles VALUES (");
  sql.add (username);
  sql.add (",");
  sql.add (bible);
  sql.add (",");
  sql.add (book);
  sql.add (",");
  sql.add (write);
  sql.add (");");
  sql.execute ();
}


// Read the privilege from the database whether $username has access to $bible $book.
// The privileges are stored in $read for read-only access,
// and in $write for write access.
// When the $book = 0, then it applies to the entire Bible.
void Database_Privileges::getBibleBook (string username, string bible, int book, bool & read, bool & write)
{
  SqliteDatabase sql (database ());
  sql.add ("SELECT write FROM bibles WHERE username =");
  sql.add (username);
  sql.add ("AND bible =");
  sql.add (bible);
  sql.add ("AND (book =");
  sql.add (book);
  sql.add ("OR book = 0)");
  // The reason for ordering the output is that, when there's more than one entry,
  // when the database contains book 0 and book n, that the setting for book n has preference.
  // Because book 0 is the general setting for all the books in the entire Bible,
  /// and book n is the more specific setting for one book.
  sql.add ("ORDER BY book DESC;");
  vector <string> result = sql.query () ["write"];
  if (result.empty()) {
    // Not in database: No access.
    read = false;
    write = false;
  } else {
    // Occurs in database: Read access.
    read = true;
    // Take write access from the database field.
    write = convert_to_bool (result [0]);
  }
}


int Database_Privileges::getBibleBookCount ()
{
  SqliteDatabase sql (database ());
  sql.add ("SELECT count(*) FROM bibles;");
  vector <string> result = sql.query () ["count(*)"];
  if (result.empty ()) return 0;
  return convert_to_int (result [0]);
}


// Returns true if a record for $username / $bible / $book exists in the database.
// When the $book = 0, it takes any book.
bool Database_Privileges::getBibleBookExists (string username, string bible, int book)
{
  SqliteDatabase sql (database ());
  sql.add ("SELECT rowid FROM bibles WHERE username =");
  sql.add (username);
  sql.add ("AND bible =");
  sql.add (bible);
  if (book) {
    sql.add ("AND book =");
    sql.add (book);
  }
  sql.add (";");
  vector <string> result = sql.query () ["rowid"];
  return !result.empty();
}


// Remove the privilege of a $username to have access to $bible $book.
// Removing the privilege for $book 0 removes them for all possible books.
void Database_Privileges::removeBibleBook (string username, string bible, int book)
{
  SqliteDatabase sql (database ());
  username = database_sqlite_no_sql_injection (username);
  sql.add ("DELETE FROM bibles WHERE username =");
  sql.add (username);
  sql.add ("AND bible =");
  sql.add (bible);
  if (book) {
    sql.add ("AND book =");
    sql.add (book);
  }
  sql.add (";");
  sql.execute ();
}


void Database_Privileges::setFeature (string username, int feature, bool enabled)
{
  SqliteDatabase sql (database ());
  username = database_sqlite_no_sql_injection (username);
  sql.add ("DELETE FROM features WHERE username =");
  sql.add (username);
  sql.add ("AND feature =");
  sql.add (feature);
  sql.add (";");
  sql.execute ();
  if (enabled) {
    sql.clear ();
    sql.add ("INSERT INTO features VALUES (");
    sql.add (username);
    sql.add (",");
    sql.add (feature);
    sql.add (");");
    sql.execute ();
  }
}


bool Database_Privileges::getFeature (string username, int feature)
{
  SqliteDatabase sql (database ());
  sql.add ("SELECT rowid FROM features WHERE username =");
  sql.add (username);
  sql.add ("AND feature =");
  sql.add (feature);
  sql.add (";");
  vector <string> result = sql.query () ["rowid"];
  if (result.empty()) return false;
  return true;
}
