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


#include <database/login.h>
#include <database/sqlite.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <filter/roles.h>
#include <filter/date.h>


// This database is resilient.
// The data is stored in a SQLite database.
// This part is read often, and infrequently written to.
// Due to the infrequent write operations, there is a low and acceptable change of corruption.


// The name of the database.
const char * Database_Login::database ()
{
  return "login";
}


void Database_Login::create ()
{
  SqliteDatabase sql (database ());
  sql.add ("CREATE TABLE IF NOT EXISTS logins ("
           " username text,"
           " address text,"
           " agent text,"
           " fingerprint text,"
           " cookie text,"
           " touch boolean,"
           " timestamp integer"
           ");");
  sql.execute ();
}


void Database_Login::trim ()
{
  // Remove persistent logins after 365 days of inactivity.
  SqliteDatabase sql (database ());
  sql.add ("DELETE FROM logins WHERE timestamp < ");
  sql.add (timestamp () - 365);
  sql.add (";");
  sql.execute ();
}


void Database_Login::optimize () // Todo
{
  if (!healthy ()) {
    // (Re)create damaged or non-existing database.
    filter_url_unlink (database_sqlite_file (database ()));
    create ();
  } else {
  }
  // Vacuum it.
  SqliteDatabase sql (database ());
  sql.add ("PRAGMA temp_store = MEMORY;");
  sql.execute ();
  sql.clear ();
  sql.add ("VACUUM;");
  sql.execute ();
}


bool Database_Login::healthy ()
{
  return database_sqlite_healthy (database ());
}


// Sets the login security tokens for a user.
// Also store whether the device is touch-enabled.
// It only writes to the table if the combination of username and tokens differs from what the table already contains.
void Database_Login::setTokens (string username, string address, string agent, string fingerprint, bool touch)
{
  if (username == getUsername (address, agent, fingerprint)) return;
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteDatabase sql (database ());
  sql.add ("INSERT INTO logins (username, address, agent, fingerprint, touch, timestamp) VALUES (");
  sql.add (username);
  sql.add (",");
  sql.add (address);
  sql.add (",");
  sql.add (agent);
  sql.add (",");
  sql.add (fingerprint);
  sql.add (",");
  sql.add (touch);
  sql.add (",");
  sql.add (timestamp ());
  sql.add (");");
  sql.execute ();
}


// Remove the login security tokens for a user.
void Database_Login::removeTokens (string username)
{
  SqliteDatabase sql (database ());
  sql.add ("DELETE FROM logins WHERE username =");
  sql.add (username);
  sql.add (";");
  sql.execute ();
}


// Remove the login security tokens for a user based on the tokens themselves.
void Database_Login::removeTokens (string username, string address, string agent, string fingerprint)
{
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteDatabase sql (database ());
  sql.add ("DELETE FROM logins WHERE username =");
  sql.add (username);
  sql.add ("AND address =");
  sql.add (address);
  sql.add ("AND agent =");
  sql.add (agent);
  sql.add ("AND fingerprint =");
  sql.add (fingerprint);
  sql.add (";");
  sql.execute ();
}


void Database_Login::renameTokens (string username_existing, string username_new,
                                   string address, string agent, string fingerprint)
{
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteDatabase sql (database ());
  sql.add ("UPDATE logins SET username =");
  sql.add (username_new);
  sql.add ("WHERE username =");
  sql.add (username_existing);
  sql.add ("AND address =");
  sql.add (address);
  sql.add ("AND agent =");
  sql.add (agent);
  sql.add ("AND fingerprint =");
  sql.add (fingerprint);
  sql.add (";");
  sql.execute ();
}


// Returns the username that matches the remote IP $address and the browser's user $agent,
// and the other fingerprints from the user.
string Database_Login::getUsername (string address, string agent, string fingerprint)
{
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteDatabase sql (database ());
  sql.add ("SELECT rowid, timestamp, username FROM logins WHERE address =");
  sql.add (address);
  sql.add ("AND agent =");
  sql.add (agent);
  sql.add ("AND fingerprint =");
  sql.add (fingerprint);
  sql.add (";");
  map <string, vector <string> > result = sql.query ();
  if (result.empty()) return "";
  string username = result ["username"][0];
  int stamp = convert_to_int (result ["timestamp"] [0]);
  if (stamp != timestamp ()) {
    // Touch the timestamp. This occurs once a day.
    int rowid = convert_to_int (result ["rowid"] [0]);
    sql.clear ();
    sql.add ("UPDATE logins SET timestamp =");
    sql.add (timestamp ());
    sql.add ("WHERE rowid =");
    sql.add (rowid);
    sql.execute ();
  }
  return username;
}


// Returns whether the device that matches the remote IP $address and the browser's user $agent,
// and the other fingerprint, is touch-enabled.
bool Database_Login::getTouchEnabled (string address, string agent, string fingerprint)
{
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteDatabase sql (database ());
  sql.add ("SELECT touch FROM logins WHERE address =");
  sql.add (address);
  sql.add ("AND agent =");
  sql.add (agent);
  sql.add ("AND fingerprint =");
  sql.add (fingerprint);
  sql.add (";");
  vector <string> result = sql.query () ["touch"];
  if (!result.empty()) return convert_to_bool (result [0]);
  return false;
}


void Database_Login::testTimestamp ()
{
  SqliteDatabase sql (database ());
  sql.add ("UPDATE logins SET timestamp = timestamp - 370;");
  sql.execute ();
}


// Gets the current timestamp.
int Database_Login::timestamp ()
{
  return filter_date_seconds_since_epoch () / 86400;
}
