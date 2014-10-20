/*
Copyright (©) 2003-2014 Teus Benschop.

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
#include <vector>
#include <sstream>
#include <fstream>
#include <libgen.h>
#include <sys/stat.h>
#include <cstring>
#include <filter/url.h>
#include <filter/string.h>
#include <sys/time.h>
#include <config/globals.h>


using namespace std;


Database_Logs::Database_Logs () // Todo working here.
{
}


Database_Logs::~Database_Logs ()
{
}



/* C++Port

// Singleton logic.


private function connect ()
{
  $db = Database_SQLite::connect ("logs");
  return $db;
}


public function create ()
{
  $db = self::connect ();
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS logs (
timestamp integer,
entry text
);
EOD;
  Database_SQLite::exec ($db, $sql);
  unset ($db);
}


// Does a checkup on the health of the main database.
// Optionally recreates it.
public function checkup ()
{
  $database_file = "../databases/logs.sqlite";
  $database_path = realpath (__DIR__ . "/" . $database_file);

  // Do an integrity check, and bail out if the database is healthy.
  // An empty file appears healthy too, so deal with that.
  if (filesize ($database_path) > 0) {
    $db = self::connect ();
    $query = "PRAGMA integrity_check;";
    $result = Database_SQLite::query ($db, $query);
    $ok = false;
    foreach ($result as $row) {
      if ($row[0] == "ok") $ok = true;
    }
    unset ($db);
    if ($ok) return;
  }

  // Recreate the database.
  @unlink ($database_path);
  $this->create ();
  $this->log ("The Journal database was damaged and has been recreated");
}


public function rotate ()
{
  $db = self::connect ();
  
  // Speed up adding and removing records.
  Database_SQLite::exec ($db, "PRAGMA synchronous = OFF;");

  // Transfer the journal entries from the filesystem into the database.
  // This speeds up subsequent reading of the Journal by the users.
  // Use a mechanism that handles huge amounts of entries.
  // Function scandir would choke on this, or take a very long time.
  // Function opendir / readdir / closedir could handle it better.
  $folder = $this->folder ();
  $counter = 0;
  $files = array ();
  $handle = opendir ($folder);
  while (($entry = readdir ($handle)) !== false) {
    if ($entry != "." && $entry != ".." && $entry != ".htaccess") {
      $files [] = $entry;
    }
    // Read a limited number of entries to avoid getting stuck in them.
    $counter++;
    if ($counter > 10000) break;
    
  }
  closedir ($handle);
  sort ($files);
  foreach ($files as $file) {
    $path = $folder . "/" . $file;
    $timestamp = $file;
    // Some of the code below have suppressed errors.
    // This is to ensure that transferring the entries does not generate additional entries entries,
    // which could lead to an infinite loop, as has been noticed on shared hosting.
    // It has also been observed that huge journal entries keep generating other huge entries,
    // which causes the disk to get full.
    // Therefore huge journal entries will be truncated, and a short message written to the Journal.
    $filesize = filesize ($path);
    if ($filesize > 10000) {
      @$fp = fopen ($path, 'r');
      @fseek ($fp, 0);
      @$entry = fread ($fp, 100);
      @fclose ($fp);
      $entry .= "... This entry was too large and has been truncated: $filesize bytes";
    } else {
      @$entry = file_get_contents ($path);
    }
    @unlink ($path);
    $timestamp = Database_SQLiteInjection::no ($timestamp);
    $entry = Database_SQLiteInjection::no ($entry);
    $query = "INSERT INTO logs VALUES ($timestamp, '$entry');";
    Database_SQLite::exec ($db, $query);
  }

  // Remove records older than five days from the database.
  $second = strtotime ("today") -  (6 * 86400);
  $timestamp = $second . str_repeat ("0", 8);
  $timestamp = Database_SQLiteInjection::no ($timestamp);
  $query = "DELETE FROM logs WHERE timestamp < $timestamp;";
  Database_SQLite::exec ($db, $query);

  $query = "VACUUM logs;";
  Database_SQLite::exec ($db, $query);

  unset ($db);
}


// Get the logbook entries for $day.
// Day 0 is today, day 1 is yesterday, and so on.
public function get ($day)
{
  $day = (int) $day;
  //$firstsecond = strtotime ("today") -  ($day * 86400);
  // A day is considered not a natural day, but a period of 24 hours from now.
  $day++;
  $firstsecond = time () -  ($day * 86400);
  $lastsecond = $firstsecond + 86400 - 1;
  $firstfilename = $firstsecond . str_repeat ("0", 8);
  $lastfilename = $lastsecond . str_repeat ("9", 8);

  $lastentry = $firstfilename;

  $entries = array ();

  // Read them from the database.
  $db = self::connect ();
  $firsttimestamp = Database_SQLiteInjection::no ($firstfilename);
  $lasttimestamp = Database_SQLiteInjection::no ($lastfilename);
  $query = "SELECT entry FROM logs WHERE timestamp >= $firsttimestamp AND timestamp <= $lasttimestamp ORDER BY rowid ASC;";
  $result = Database_SQLite::query ($db, $query);
  foreach ($result as $row) {
    $entries [] = $row[0];
  }
  unset ($db);
  
  // Add them from the filesystem.
  $files = scandir ($this->folder ());
  $files = Filter_Folders::cleanup ($files);
  foreach ($files as $file) {
    if (($file >= $firstfilename) && ($file <= $lastfilename)) {
      $lastentry = $file;
      $entries [] = file_get_contents ($this->folder () . "/$file");
    }
  }

  return array ($lastentry, $entries);
}


// Gets journal entry more recent than $filename.
public function getNext ($filename)
{
  $files = scandir ($this->folder ());
  $files = Filter_Folders::cleanup ($files);
  foreach ($files as $file) {
    if ($file > $filename) {
      $entry = file_get_contents ($this->folder () . "/$file");
      return array ($file, $entry);
    }
  }

  return NULL;
}


*/


string Database_Logs::folder ()
{
  return filter_url_create_root_path ("logbook");
}


void Database_Logs::log (string description, int level) // Todo work here.
{
  // No new lines.
  description = filter_string_str_replace ("\n", " ", description);

  // During unit testing, send everything to the terminal as well.
  if (config_globals_unit_testing) cout << description << endl;

  // Save this logbook entry to a filename with seconds and microseconds.
  struct timeval tv;
  gettimeofday (&tv, NULL);
  char time [64];
  sprintf (time, "%u%08u", (unsigned int) tv.tv_sec, (unsigned int) tv.tv_usec);
  vector <string> components;
  components.push_back (folder ());
  components.push_back (time);
  string file = filter_url_create_path (components);

  char seconds [64];
  sprintf (seconds, "%u", (unsigned int) tv.tv_sec);

  // The microseconds granularity depends on the platform.
  // On Windows it is lower than on Linux.
  // There may be the rare case of more than one entry per file.
  // Append the data so it won't overwrite an earlier entry.
  filter_url_put_file_contents_apend (file, filter_string_convert_to_string (level) + " " + seconds + " " + description);
}


