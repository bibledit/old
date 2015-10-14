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


#include <database/logs.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <database/sqlite.h>
#include <filter/date.h>
#include <journal/logic.h>
#include <config/logic.h>


string Database_Logs::folder ()
{
  return filter_url_create_root_path ("logbook");
}


void Database_Logs::log (string description, int level)
{
  if (config_logic_demo_enabled ()) {
    debug (description); // Todo debug
  }
  // No new lines.
  description = filter_string_str_replace ("\n", " ", description);
  // Discard empty line.
  if (filter_string_trim (description).empty()) return;
  // Truncate long entry.
  int length = description.length ();
  if (length > 1000) {
    description.erase (1000);
    description.append ("... This entry was too large and has been truncated: " + convert_to_string (length) + " bytes");
  }
  // Save this logbook entry to a filename with seconds and microseconds.
  string seconds = convert_to_string (filter_date_seconds_since_epoch ());
  string time = seconds + filter_string_fill (convert_to_string (filter_date_numerical_microseconds ()), 8, '0');
  string file = filter_url_create_path (folder (), time);
  // The microseconds granularity depends on the platform.
  // On Windows it is lower than on Linux.
  // There may be the rare case of more than one entry per file.
  // Append the data so it won't overwrite an earlier entry.
  if (file_exists (file)) {
    description.insert (0, " | ");
  } else {
    description.insert (0, convert_to_string (level) + " " + seconds + " ");
  }
  filter_url_file_put_contents_append (file, description);
  // Delay to cover for lower usec granularity on Windows.
  if (config_logic_windows ()) {
    this_thread::sleep_for (chrono::milliseconds (1));
  }
}


sqlite3 * Database_Logs::connect ()
{
  return database_sqlite_connect ("logs2");
}


void Database_Logs::create ()
{
  sqlite3 * db = connect ();
  string sql = "CREATE TABLE IF NOT EXISTS logs ("
               " timestamp integer,"
               " entry text"
               ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Does a checkup on the health of the main database.
// Optionally recreates it.
void Database_Logs::checkup ()
{
  string database = "logs2";
  if (database_sqlite_healthy (database)) return;

  // Recreate the database.
  string file = database_sqlite_file (database);
  filter_url_unlink (file);
  create ();
  Database_Logs::log ("The Journal database was damaged and has been recreated");
}


void Database_Logs::rotate ()
{
  sqlite3 * db = connect ();
  
  // Speed up adding and removing records.
  database_sqlite_exec (db, "PRAGMA synchronous = OFF;");

  // Transfer the journal entries from the filesystem into the database.
  // This speeds up subsequent reading of the Journal by the users.
  // Use a mechanism that handles huge amounts of entries.
  // The PHP function scandir choked on this or took a very long time.
  // The PHP functions opendir / readdir / closedir handled it better.
  // C++ uses the better method of the two, just to be sure.

  string directory = folder ();
  int counter = 0;
  vector <string> files;
  DIR * dir = opendir (directory.c_str());
  if (dir) {
    struct dirent * direntry;
    while ((direntry = readdir (dir)) != NULL) {
      string name = direntry->d_name;
      if (name != "." && name != ".." && name != "gitflag" && name != ".DS_Store") {
        files.push_back (name);
      }
      // Limit the number of entries to avoid getting stuck in them.
      counter++;
      if (counter > 10000) break;
    }
  }
  closedir (dir);

  sort (files.begin(), files.end());
  
  bool filtered_entries = false;
  
  for (unsigned int i = 0; i < files.size(); i++) {
    string path = filter_url_create_path (directory, files [i]);
    int timestamp = convert_to_int (files [i].substr (0, 10));
    // Some of the code below had suppressed errors in PHP.
    // This was to ensure that transferring the entries does not generate additional journal entries.
    // This would lead to an infinite loop, as has been noticed on shared hosting.
    // It has also been observed that huge journal entries keep generating other huge entries,
    // which causes the disk to get full.
    // Therefore huge journal entries will be truncated, and a short message written to the journal.
    string entry = filter_url_file_get_contents (path);
    int filesize = filter_url_filesize (path);
    if (filesize > 10000) {
      entry = entry.substr (0, 100);
      entry += "... This entry was too large and has been truncated: " + convert_to_string (filesize) + " bytes";
    }
    filter_url_unlink (path);
    // Filtering of certain entries.
    if (journal_logic_filter_entry (entry)) {
      filtered_entries = true;
    } else {
      entry = database_sqlite_no_sql_injection (entry);
      string sql = "INSERT INTO logs VALUES (" + convert_to_string (timestamp) + ", '" + entry + "');";
      database_sqlite_exec (db, sql);
    }
  }

  // Remove records older than five days from the database, or younger in case of a tiny journal.
#ifdef HAVE_TINYJOURNAL
  string timestamp = convert_to_string (filter_date_seconds_since_epoch () - (14400));
#else
  string timestamp = convert_to_string (filter_date_seconds_since_epoch () - (6 * 86400));
#endif
  string sql = "DELETE FROM logs WHERE timestamp < " + timestamp + ";";
  database_sqlite_exec (db, sql);

  sql = "VACUUM logs;";
  database_sqlite_exec (db, sql);

  database_sqlite_disconnect (db);
  
  if (filtered_entries) {
    log (journal_logic_filtered_message ());
  }
  
  // If there are too many files still left in the logbook folder,
  // that means that there's some problem that prevents the logbook entries from being recorded in the database.
  // This may lead to an infinite loop as has been noticed at times,
  // and this may quickly exhaust the available inodes on the filesystem.
  // Therefore if there are too many files still left in the logbook folder, clear them out.
  // This leads to loss of logbook information, but be it so.
  // Loss of information is better in this case than a server that gets stuck.
  // Also clear the logbook database out.
  files = filter_url_scandir (directory);
  if (files.size () > 100) {
    clear ();
    log ("Irrecoverable journal errors: Everything was cleared out");
  }
}


// Get the logbook entries for variable "day".
// Day 0 is the last 24 hours, day 1 is 24 more hours back, and so on.
vector <string> Database_Logs::get (int day, string & lastfilename)
{
  // A day is considered a period of 24 hours starting now.
  int firstsecond = filter_date_seconds_since_epoch () - ((day + 1) * 86400);
  int lastsecond = firstsecond + 86400;
  lastfilename = convert_to_string (lastsecond) + "00000000";

  // Read the entries from the database.
  vector <string> entries;
  sqlite3 * db = connect ();
  string query = "SELECT entry FROM logs WHERE timestamp >= " + convert_to_string (firstsecond) + " AND timestamp <= " + convert_to_string (lastsecond) + " ORDER BY rowid ASC;";
  entries = database_sqlite_query (db, query)["entry"];
  database_sqlite_disconnect (db);

  // Add entries from the filesystem in case of day 0.
  if (day == 0) {
    string directory = folder ();
    vector <string> files = filter_url_scandir (directory);
    for (unsigned int i = 0; i < files.size(); i++) {
      string file = files [i];
      string path = filter_url_create_path (directory, file);
      string contents = filter_url_file_get_contents (path);
      entries.push_back (contents);
      // Last second gets updated based on the filename.
      lastfilename = file;
    }
  }

  // Done.  
  return entries;
}


// Gets journal entry more recent than "filename".
// Updates "filename" to the item it got.
string Database_Logs::getNext (string &filename)
{
  string directory = folder ();
  vector <string> files = filter_url_scandir (directory);
  for (unsigned int i = 0; i < files.size (); i++) {
    string file = files [i];
    if (file > filename) {
      filename = file;
      string path = filter_url_create_path (directory, file);
      string contents = filter_url_file_get_contents (path);
      return contents;
    }
  }
  return "";
}


// Used for unit testing.
// Updated entries in the database at a timestamp of "oldseconds"
// to a timestamp of "newseconds".
void Database_Logs::update (int oldseconds, int newseconds)
{
  sqlite3 * db = connect ();
  for (int i = -1; i <= 1; i++) {
    string sql = "UPDATE logs SET timestamp = " + convert_to_string (newseconds) + " WHERE timestamp = " + convert_to_string (oldseconds + i) + ";";
    database_sqlite_exec (db, sql);
  }
  database_sqlite_disconnect (db);
}


// Clears all journal entries.
void Database_Logs::clear ()
{
  string directory = folder ();
  vector <string> files = filter_url_scandir (directory);
  for (auto file : files) {
    filter_url_unlink (filter_url_create_path (directory, file));
  }
  sqlite3 * db = connect ();
  database_sqlite_exec (db, "DELETE FROM logs;");
  database_sqlite_disconnect (db);
  checkup ();
  log ("The journal was cleared");
}


void Database_Logs::debug (string description)
{
  int seconds = filter_date_seconds_since_epoch ();
  seconds = filter_date_local_seconds (seconds);
  
  string timestamp;
  timestamp.append (convert_to_string (filter_date_numerical_year (seconds)));
  timestamp.append ("-");
  timestamp.append (convert_to_string (filter_date_numerical_month (seconds)));
  timestamp.append ("-");
  timestamp.append (convert_to_string (filter_date_numerical_month_day (seconds)));
  timestamp.append (" ");
  timestamp.append (convert_to_string (filter_date_numerical_hour (seconds)));
  timestamp.append (":");
  timestamp.append (convert_to_string (filter_date_numerical_minute (seconds)));
  timestamp.append (":");
  timestamp.append (convert_to_string (filter_date_numerical_second (seconds)));
  
  description.insert (0, timestamp + " ");
  
  string file = filter_url_create_root_path ("tmp", "debug.txt");
  filter_url_file_put_contents_append (file, description);
}


