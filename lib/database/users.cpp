/*
Copyright (ƒ) 2003-2015 Teus Benschop.

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


#include <database/users.h>
#include <database/sqlite.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/date.h>


// This database is resilient.
// Part of the data is stored in a SQLite database.
// This part is read often, and infrequently written to.
// Due to the infrequent write operations, there is a low and acceptable change of corruption.
// Another part of the data is stored in the bare filesystem.
// This part is written to often. It contains non-important information.
// In the unlikely case of corruption, it will get fixed next time it is written to.


Database_Users::Database_Users ()
{
}


Database_Users::~Database_Users ()
{
}


sqlite3 * Database_Users::connect ()
{
  return database_sqlite_connect ("users");
}


void Database_Users::create ()
{
  sqlite3 * db = connect ();
  string sql;
  sql = "CREATE TABLE IF NOT EXISTS users ("
        " username text,"
        " password text,"
        " level integer,"
        " email text"
        ");";
  database_sqlite_exec (db, sql);
  sql = "CREATE TABLE IF NOT EXISTS teams ("
        " username text,"
        " bible text,"
        " readonly boolean"
        ");";
  database_sqlite_exec (db, sql);
  sql = "CREATE TABLE IF NOT EXISTS logins ("
        " username text,"
        " address text,"
        " agent text,"
        " fingerprint text,"
        " cookie text,"
        " touch boolean"
        ");";
  database_sqlite_exec (db, sql);
  sql = "CREATE TABLE IF NOT EXISTS readonly ("
        " username text,"
        " bible text,"
        " book integer"
        ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


void Database_Users::upgrade ()
{
  sqlite3 * db = connect ();
  string sql;

  // Upgrade table "users".
  // Column 'timestamp' is available in older databases. It is not in use.
  // It cannot be dropped easily in SQLite. Leave it for just now.

  // Upgrade table "logins" and add a boolean column for "touch",
  // indicating whether the device the user works on is touch-enabled.
  sql = "PRAGMA table_info (logins);";
  vector <string> columns = database_sqlite_query (db, sql) ["name"];
  if (find (columns.begin(), columns.end(), "touch") == columns.end()) {
    sql = "ALTER TABLE logins ADD COLUMN touch boolean;";
    database_sqlite_exec (db, sql);
  }

  // Copy read-only settings from the teams table to the readonly table,
  // because Bibledit now sets read-only access per book, rather than per Bible.
  sql = "SELECT username, bible FROM teams WHERE readonly;";
  map <string, vector <string> > result = database_sqlite_query (db, sql);
  vector <string> username = result ["username"];
  vector <string> bible = result ["bible"];
  if (username.size () == bible.size ()) {
    for (size_t i = 0; i < username.size (); i++) {
      for (int b = 1; b <= 66; b++) {
        SqliteSQL sql = SqliteSQL ();
        sql.add ("INSERT INTO readonly VALUES (");
        sql.add (username[i]);
        sql.add (",");
        sql.add (bible[i]);
        sql.add (",");
        sql.add (b);
        sql.add (");");
        database_sqlite_exec (db, sql.sql);
      }
    }
  }
  
  // Clear any read-only settings in the teams table,
  // because that information is no longer taken from that table.
  sql = "UPDATE teams SET readonly = 0;";
  database_sqlite_exec (db, sql);
  
  database_sqlite_disconnect (db);
}


string Database_Users::mainFolder ()
{
  return filter_url_create_root_path ("databases", "users");
}


string Database_Users::timestampFile (string user)
{
  return filter_url_create_path (mainFolder (), "timestamp" + user);
}


void Database_Users::trim ()
{
  // Remove persistent logins after two days of inactivity.
  int daysAgo = filter_date_seconds_since_epoch () - (2 * 86400);
  vector <string> users = getUsers ();
  sqlite3 * db = connect ();
  for (unsigned int i = 0; i < users.size(); i++) {
    string username = users [i];
    int timestamp = getTimestamp (username);
    if (timestamp < daysAgo) {
      username = database_sqlite_no_sql_injection (username);
      string sql = "UPDATE logins SET fingerprint = '' WHERE username = '" + username + "';";
      database_sqlite_exec (db, sql);
    }
  }
  database_sqlite_disconnect (db);
}


void Database_Users::optimize ()
{
  sqlite3 * db = connect ();
  string sql = "VACUUM users;";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Add the user details to the database.
void Database_Users::addNewUser (string username, string password, int level, string email)
{
  username = database_sqlite_no_sql_injection (username);
  email = database_sqlite_no_sql_injection (email);
  sqlite3 * db = connect ();
  string sql = "INSERT INTO users (username, level, email) VALUES ('" + username + "', " + convert_to_string (level) + ", '" + email + "');";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
  updateUserPassword (username, password);
}


// Updates the password for user.
void Database_Users::updateUserPassword (string user, string password)
{
  user = database_sqlite_no_sql_injection (user);
  password = md5 (password);
  sqlite3 * db = connect ();
  string sql = "UPDATE users SET password = '" + password + "' WHERE username = '" + user + "';";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Returns true if the username and password match.
bool Database_Users::matchUsernamePassword (string username, string password)
{
  username = database_sqlite_no_sql_injection (username);
  password = md5 (password);
  sqlite3 * db = connect ();
  string sql = "SELECT * FROM users WHERE username = '" + username + "' and password = '" + password + "';";
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return (!result.empty());
}


// Returns true if the email and password match.
bool Database_Users::matchEmailPassword (string email, string password)
{
  email = database_sqlite_no_sql_injection (email);
  password = md5 (password);
  string sql = "SELECT * FROM users WHERE email = '" + email + "' and password = '" + password + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return (!result.empty());
}


// Returns the query to execute to add a new user.
string Database_Users::addNewUserQuery (string username, string password, int level, string email)
{
  username = database_sqlite_no_sql_injection (username);
  password = md5 (password);
  email = database_sqlite_no_sql_injection (email);
  string query = "INSERT INTO users (username, password, level, email) VALUES ('" + username + "', '" + password + "', " + convert_to_string (level) + ", '" + email + "');";
  return query;
}


// Returns the username that belongs to the email.
string Database_Users::getEmailToUser (string email)
{
  email = database_sqlite_no_sql_injection (email);
  string sql = "SELECT username FROM users WHERE email = '" + email + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  if (!result.empty()) return result [0];
  return "";
}


// Returns the email address that belongs to user.
string Database_Users::getUserToEmail (string user)
{
  user = database_sqlite_no_sql_injection (user);
  string sql = "SELECT email FROM users WHERE username = '" + user + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["email"];
  database_sqlite_disconnect (db);
  if (!result.empty()) return result [0];
  return "";
}


// Returns true if the username exists in the database.
bool Database_Users::usernameExists (string user)
{
  user = database_sqlite_no_sql_injection (user);
  string sql = "SELECT * FROM users WHERE username = '" + user + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return !result.empty ();
}


// Returns true if the email address exists in the database.
bool Database_Users::emailExists (string email)
{
  email  = database_sqlite_no_sql_injection (email);
  string sql  = "SELECT * FROM users WHERE email = '" + email + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return !result.empty ();
}


// Returns the level that belongs to the user.
int Database_Users::getUserLevel (string user)
{
  user = database_sqlite_no_sql_injection (user);
  string sql = "SELECT level FROM users WHERE username = '" + user + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["level"];
  database_sqlite_disconnect (db);
  if (!result.empty()) return convert_to_int (result [0]);
  return Filter_Roles::guest ();
}


// Updates the level of a given user.
void Database_Users::updateUserLevel (string user, int level)
{
  user = database_sqlite_no_sql_injection (user);
  string sql = "UPDATE users SET level = " + convert_to_string (level) + " WHERE username = '" + user + "';";
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Remove a user from the database.
void Database_Users::removeUser (string user)
{
  user = database_sqlite_no_sql_injection (user);
  sqlite3 * db = connect ();
  string sql = "DELETE FROM users WHERE username = '" + user + "'";
  database_sqlite_exec (db, sql);
  sql = "DELETE FROM teams WHERE username = '" + user + "'";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Returns an array with the usernames of the site administrators.
vector <string> Database_Users::getAdministrators ()
{
  string sql = "SELECT username FROM users WHERE level = " + convert_to_string (Filter_Roles::admin ()) + ";";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return result;
}


// Returns the query to update a user's email address.
string Database_Users::updateEmailQuery (string username, string email)
{
  return "UPDATE users SET email = '" + email + "' WHERE username = '" + username + "';";
}


// Updates the "email" for "user".
void Database_Users::updateUserEmail (string user, string email)
{
  user = database_sqlite_no_sql_injection (user);
  email = database_sqlite_no_sql_injection (email);
  string sql = updateEmailQuery (user, email);
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Return an array with the available users.
vector <string> Database_Users::getUsers ()
{
  string sql = "SELECT username FROM users;";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return result;
}


// Returns the md5 hash for the $user's password.
string Database_Users::getmd5 (string user)
{
  user = database_sqlite_no_sql_injection (user);
  string sql = "SELECT password FROM users WHERE username = '" + user + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["password"];
  database_sqlite_disconnect (db);
  if (!result.empty()) return result [0];
  return "";
}


// Sets the login security tokens for a user.
// Also store whether the device is touch-enabled.
// It only writes to the table if the combination of username and tokens differs from what the table already contains.
void Database_Users::setTokens (string username, string address, string agent, string fingerprint, bool touch)
{
  if (username == getUsername (address, agent, fingerprint)) return;
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteSQL sql;
  sql.add ("INSERT INTO logins (username, address, agent, fingerprint, touch) VALUES (");
  sql.add (username);
  sql.add (",");
  sql.add (address);
  sql.add (",");
  sql.add (agent);
  sql.add (",");
  sql.add (fingerprint);
  sql.add (",");
  sql.add (touch);
  sql.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Remove the login securityh tokens for a user.
void Database_Users::removeTokens (string username)
{
  username = database_sqlite_no_sql_injection (username);
  string sql = "DELETE FROM logins WHERE username = '" + username + "';";
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Returns the username that matches the remote IP $address and the browser's user $agent,
// and the other fingerprints from the user.
string Database_Users::getUsername (string address, string agent, string fingerprint)
{
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteSQL sql;
  sql.add ("SELECT username FROM logins WHERE address =");
  sql.add (address);
  sql.add ("AND agent =");
  sql.add (agent);
  sql.add ("AND fingerprint =");
  sql.add (fingerprint);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["username"];
  database_sqlite_disconnect (db);
  if (!result.empty()) return result [0];
  return "";
}


// Returns whether the device that matches the remote IP $address and the browser's user $agent,
// and the other fingerprints is touch-enabled.
bool Database_Users::getTouchEnabled (string address, string agent, string fingerprint)
{
  address = md5 (address);
  agent = md5 (agent);
  fingerprint = md5 (fingerprint);
  SqliteSQL sql;
  sql.add ("SELECT touch FROM logins WHERE address =");
  sql.add (address);
  sql.add ("AND agent =");
  sql.add (agent);
  sql.add ("AND fingerprint =");
  sql.add (fingerprint);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["touch"];
  database_sqlite_disconnect (db);
  if (!result.empty()) return convert_to_bool (result [0]);
  return false;
}


// Ping the access timestamp for $username.
void Database_Users::pingTimestamp (string username)
{
  int existingTimestamp = getTimestamp (username);
  int timestamp = filter_date_seconds_since_epoch ();
  if (timestamp != existingTimestamp) {
    string file = timestampFile (username);
    filter_url_file_put_contents (file, convert_to_string (timestamp));
  }
}


// Gets the access timestamp for $username.
int Database_Users::getTimestamp (string username)
{
  string file = timestampFile (username);
  string timestamp = filter_url_file_get_contents (file);
  return convert_to_int (timestamp);
}


// Get array with the teams.
vector <string> Database_Users::getTeams ()
{
  string sql = "SELECT DISTINCT bible FROM teams;";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["bible"];
  database_sqlite_disconnect (db);
  return result;
}


// Give a $user access to a $bible.
void Database_Users::grantAccess2Bible (string user, string bible)
{
  revokeAccess2Bible (user, bible);
  user = database_sqlite_no_sql_injection (user);
  bible = database_sqlite_no_sql_injection (bible);
  string sql = "INSERT INTO teams (username, bible) VALUES ('" + user + "', '" + bible + "');";
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}

 
// Revoke a $user's access to a $bible.
void Database_Users::revokeAccess2Bible (string user, string bible)
{
  user = database_sqlite_no_sql_injection (user);
  bible = database_sqlite_no_sql_injection (bible);
  string sql = "DELETE FROM teams WHERE username = '" + user + "' AND bible = '" + bible + "';";
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Get the Bibles the $user has access to.
vector <string> Database_Users::getBibles4User (string user)
{
  user = database_sqlite_no_sql_injection (user);
  string sql = "SELECT DISTINCT bible FROM teams WHERE username = '" + user + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["bible"];
  database_sqlite_disconnect (db);
  return result;
}


// Get the users who have access to a $bible.
vector <string> Database_Users::getUsers4Bible (string bible)
{
  bible = database_sqlite_no_sql_injection (bible);
  string sql = "SELECT DISTINCT username FROM teams WHERE bible = '" + bible + "';";
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql) ["username"];
  database_sqlite_disconnect (db);
  return result;
}


// Returns whether a $user has access to a $bible: true or false.
// If no more than one team has been formed, all users have write access to all Bibles.
// Administrators and Managers have access to any Bible.
bool Database_Users::hasAccess2Bible (string user, string bible)
{
  vector <string> teams = getTeams ();
  if (teams.size () <= 1) return true;
  int level = getUserLevel (user);
  if (level >= Filter_Roles::manager ()) return true;
  vector <string> bibles = getBibles4User (user);
  return in_array (bible, bibles);
}


// Returns true or false depending on whether $user has read-only access to $bible.
bool Database_Users::hasReadOnlyAccess2Bible (string user, string bible)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT count(*) FROM readonly WHERE username =");
  sql.add (user);
  sql.add ("AND bible =");
  sql.add (bible);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["count(*)"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) return convert_to_bool (result[0]);
  // Entry not found for user/bible: Default is not read-only.
  return false;
}


// Set $readonly access for $user to $bible $book to true or false.
void Database_Users::setReadOnlyAccess2Book (string user, string bible, int book, bool readonly)
{
  sqlite3 * db = connect ();
  {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("DELETE FROM readonly WHERE username =");
    sql.add (user);
    sql.add ("AND bible =");
    sql.add (bible);
    sql.add ("AND book =");
    sql.add (book);
    sql.add (";");
    database_sqlite_exec (db, sql.sql);
  }
  if (readonly) {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("INSERT INTO readonly VALUES (");
    sql.add (user);
    sql.add (",");
    sql.add (bible);
    sql.add (",");
    sql.add (book);
    sql.add (");");
    database_sqlite_exec (db, sql.sql);
  }
  database_sqlite_disconnect (db);
}


// Returns true or false depending on whether $user has read-only access to $bible $book.
bool Database_Users::hasReadOnlyAccess2Book (string user, string bible, int book)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT count(*) FROM readonly WHERE username =");
  sql.add (user);
  sql.add ("AND bible =");
  sql.add (bible);
  sql.add ("AND book =");
  sql.add (book);
  sql.add (";");
  sqlite3 * db = connect ();
  vector <string> result = database_sqlite_query (db, sql.sql) ["count(*)"];
  database_sqlite_disconnect (db);
  if (!result.empty ()) return convert_to_bool (result[0]);
  return false;
}


void Database_Users::execute (const string& sql)
{
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}
