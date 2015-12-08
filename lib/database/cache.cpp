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


#include <database/cache.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/date.h>
#include <filter/shell.h>
#include <database/sqlite.h>
#include <database/logs.h>
#include <config/logic.h>


// Databases resilience:
// They contain cached data.
// They are checked on health every night.


string Database_Cache::database_resource (string resource)
{
  // Name of the database for this resource.
  resource = database_cache_clean_name (resource);
  return "cache_resource_" + resource;
}


void Database_Cache::create (string resource)
{
  SqliteDatabase sql = SqliteDatabase (database_resource (resource));
  sql.add ("CREATE TABLE IF NOT EXISTS cache ("
           "book integer, chapter integer, verse integer, "
           "value text, days integer);");
  sql.execute ();
}


// Does a checkup on the health of the main database.
// Optionally recreates it.
void Database_Cache::check ()
{
  vector <string> names = filter_url_scandir (filter_url_create_root_path ("databases"));
  string filefragment = database_resource ("");
  for (auto & name : names) {
    if (name.find (".sqlite.sqlite") != string::npos) {
      // Because of a bug elsewhere it generates *.sqlite.sqlite files in the Cloud.
      // Delete them.
      string path = filter_url_create_root_path ("databases", name);
      filter_url_unlink (path);
    } else if (name.find (filefragment) != string::npos) {
      if (database_sqlite_healthy (name)) continue;
      string resource = name.substr (filefragment.length ());
      string file = database_sqlite_file (database_resource (resource));
      filter_url_unlink (file);
      create (resource);
      Database_Logs::log ("The resource cache was damaged and has been reset: " + resource);
    }
  }
}


void Database_Cache::trim ()
{
  if (config_logic_client_prepared ()) {
    // A client trims nothing at all of the cache.
    // If it would trim, then a client wihtout internet might find itself without resources
    // at an inconvenient moment.
  } else {
    // On the server, check whether a SWORD module has not been accessed during the last nnn days.
    int now = filter_date_seconds_since_epoch () / 86400;
    vector <string> names = filter_url_scandir (filter_url_create_root_path ("databases"));
    string filefragment = database_resource ("");
    for (auto & name : names) {
      if (name.find (filefragment) != string::npos) {
        string resource = name.substr (filefragment.length ());
        resource = resource.substr (0, resource.length () - database_sqlite_suffix ().length ());
        int day = days (resource, 0, 0, 0);
        // Zero days means: Permanent cache.
        if (day) {
          if (day < (now - 100)) {
            // Too old: Delete the database.
            string file = database_sqlite_file (database_resource (resource));
            filter_url_unlink (file);
          }
        }
      }
    }
  }
}


// Returns true if the cache for the $resource exists.
bool Database_Cache::exists (string resource)
{
  string file = database_sqlite_file (database_resource (resource));
  return file_exists (file);
}


// Returns true if a cached value for $resource/book/chapter/verse exists.
bool Database_Cache::exists (string resource, int book, int chapter, int verse)
{
  SqliteDatabase sql = SqliteDatabase (database_resource (resource));
  sql.add ("SELECT count(*) FROM cache WHERE book =");
  sql.add (book);
  sql.add ("AND chapter = ");
  sql.add (chapter);
  sql.add ("AND verse = ");
  sql.add (verse);
  sql.add (";");
  vector <string> result = sql.query () ["count(*)"];
  int count = 0;
  if (!result.empty ()) count = convert_to_int (result [0]);
  return (count > 0);
}


// Caches a value.
void Database_Cache::cache (string resource, int book, int chapter, int verse, string value)
{
  SqliteDatabase sql = SqliteDatabase (database_resource (resource));

  sql.clear ();
  sql.add ("DELETE FROM cache WHERE book =");
  sql.add (book);
  sql.add ("AND chapter = ");
  sql.add (chapter);
  sql.add ("AND verse = ");
  sql.add (verse);
  sql.add (";");
  sql.execute ();
  
  int days = filter_date_seconds_since_epoch () / 86400;

  sql.clear ();
  sql.add ("INSERT INTO cache VALUES (");
  sql.add (book);
  sql.add (",");
  sql.add (chapter);
  sql.add (",");
  sql.add (verse);
  sql.add (",");
  sql.add (value);
  sql.add (",");
  sql.add (days);
  sql.add (");");
  sql.execute ();
}


// Retrieves a cached value.
string Database_Cache::retrieve (string resource, int book, int chapter, int verse)
{
  SqliteDatabase sql = SqliteDatabase (database_resource (resource));
  sql.add ("SELECT value, days FROM cache WHERE book =");
  sql.add (book);
  sql.add ("AND chapter = ");
  sql.add (chapter);
  sql.add ("AND verse = ");
  sql.add (verse);
  sql.add (";");
  map <string, vector <string> > result = sql.query ();
  if (result.empty ()) return "";
  
  int days = convert_to_int (result ["days"] [0]);
  if (days) {
    int now = filter_date_seconds_since_epoch () / 86400;
    if (days != now) {
      sql.clear ();
      sql.add ("UPDATE cache SET days =");
      sql.add (now);
      sql.add ("WHERE book =");
      sql.add (book);
      sql.add ("AND chapter = ");
      sql.add (chapter);
      sql.add ("AND verse = ");
      sql.add (verse);
      sql.add (";");
      sql.execute ();
    }
  }
  
  return result ["value"] [0];
}


// Gets the number of days for resource/passage since the Linux epoch.
int Database_Cache::days (string resource, int book, int chapter, int verse)
{
  SqliteDatabase sql = SqliteDatabase (database_resource (resource));
  sql.add ("SELECT days FROM cache WHERE book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("AND verse =");
  sql.add (verse);
  sql.add (";");
  vector <string> result = sql.query () ["days"];
  if (result.empty ()) return 0;
  return convert_to_int (result [0]);
}


string database_cache_clean_name (string name)
{
  // Remove invalid characters in Windows filenames.
  name = filter_string_str_replace ("\\", "", name);
  name = filter_string_str_replace ("/",  "", name);
  name = filter_string_str_replace (":",  "", name);
  name = filter_string_str_replace ("*",  "", name);
  name = filter_string_str_replace ("?",  "", name);
  name = filter_string_str_replace ("\"", "", name);
  name = filter_string_str_replace ("<",  "", name);
  name = filter_string_str_replace (">",  "", name);
  name = filter_string_str_replace ("|",  "", name);
  return name;
}


string database_cache_full_path (string file)
{
  return filter_url_create_root_path ("databases", "cache", file);
}


string database_cache_split_file (string file)
{
  if (file.size () > 9) file.insert (9, "/");
  if (file.size () > 18) file.insert (18, "/");
  if (file.size () > 27) file.insert (27, "/");
  file.append (".txt");
  return file;
}


bool database_cache_exists (string schema)
{
  schema = database_cache_clean_name (schema);
  schema = database_cache_split_file (schema);
  schema = database_cache_full_path (schema);
  return file_exists (schema);
}


void database_cache_put (string schema, string contents)
{
  schema = database_cache_clean_name (schema);
  schema = database_cache_split_file (schema);
  schema = database_cache_full_path (schema);
  string path = filter_url_dirname (schema);
  if (!file_exists (path)) filter_url_mkdir (path);
  filter_url_file_put_contents (schema, contents);
}


string database_cache_get (string schema)
{
  schema = database_cache_clean_name (schema);
  schema = database_cache_split_file (schema);
  schema = database_cache_full_path (schema);
  return filter_url_file_get_contents (schema);
}


// Deletes items older than x days from the cache.
// It uses a Linux shell command. This can be done because it runs on the server only.
// On some clients, shell commands don't work.
void database_cache_trim ()
{
  Database_Logs::log ("trim cache"); // Todo
  string path = database_cache_full_path ("");
  string output, error;
  filter_shell_run (path, "find", {path, "-atime", "+5", "-delete"}, &output, &error);
  if (!output.empty ()) Database_Logs::log (output);
  if (!error.empty ()) Database_Logs::log (error);
}
