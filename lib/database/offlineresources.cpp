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


#include <database/offlineresources.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>
#include <config/logic.h>


// This database is not very resilient.
// It contains external data that can be replaced if it gets corrupted.
// It stores one book in a database file.
// This provides some measure of robustness.
// When downloading an external resource, there is one script per book, 
// so that only one script writes to the database at any time.


void Database_OfflineResources::erase (string name)
{
  string path = resourceFolder (name);
  // If a file: Delete it.
  filter_url_unlink (path);
  // If a folder: Delete it.
  filter_url_rmdir (path);
}


void Database_OfflineResources::store (string name, int book, int chapter, int verse, string html)
{
  string folder = resourceFolder (name);
  if (!file_exists (folder)) filter_url_mkdir (folder);

  string file = databaseFile (name, book);

  healthy (file);

  sqlite3 *db = NULL;

  if (!file_exists (file)) {
    db = connect (file);
    string sql = ""
                 "CREATE TABLE IF NOT EXISTS offlineresources ("
                 "chapter integer,"
                 "verse integer,"
                 "html text"
                 ");";
    database_sqlite_exec (db, sql);
  }

  if (db == NULL) db = connect (file);

  SqliteSQL sql1 = SqliteSQL ();
  sql1.add ("DELETE FROM offlineresources WHERE chapter =");
  sql1.add (chapter);
  sql1.add ("AND verse =");
  sql1.add (verse);
  sql1.add (";");
  database_sqlite_exec (db, sql1.sql);

  SqliteSQL sql2 = SqliteSQL ();
  sql2.add ("INSERT INTO offlineresources VALUES (");
  sql2.add (chapter);
  sql2.add (",");
  sql2.add (verse);
  sql2.add (",");
  sql2.add (html);
  sql2.add (");");
  database_sqlite_exec (db, sql2.sql);

  database_sqlite_disconnect (db);
}


void Database_OfflineResources::healthy (string file)
{
  bool ok = database_sqlite_healthy (file);
  if (!ok) filter_url_unlink (file);
}


bool Database_OfflineResources::exists (string name, int book, int chapter, int verse)
{
  string file = databaseFile (name, book);
  if (file_exists (file)) {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("SELECT html FROM offlineresources WHERE chapter =");
    sql.add (chapter);
    sql.add ("AND verse =");
    sql.add (verse);
    sql.add (";");
    sqlite3 * db = connect (file);
    vector <string> result = database_sqlite_query (db, sql.sql) ["html"];
    database_sqlite_disconnect (db);
    return !result.empty ();
  }
  return false;
}


// Counts the number of verses in the offline resource name.
int Database_OfflineResources::count (string name)
{
  int count = 0;
  string resource_folder = resourceFolder (name);
  vector <string> databases = filter_url_scandir (resource_folder);
  for (auto & database : databases) {
    if (database.find ("sqlite") != string::npos) {
      string file = filter_url_create_path (resource_folder, database);
      sqlite3 * db = connect (file);
      vector <string> result = database_sqlite_query (db, "SELECT count(*) FROM offlineresources;") ["count(*)"];
      database_sqlite_disconnect (db);
      for (auto & cnt : result) {
        count += convert_to_int (cnt);
      }
    }
  }
  return count;
}


string Database_OfflineResources::get (string name, int book, int chapter, int verse)
{
  string html = "";
  string file = databaseFile (name, book);
  if (file_exists (file)) {
    SqliteSQL sql = SqliteSQL ();
    sql.add ("SELECT html FROM offlineresources WHERE chapter =");
    sql.add (chapter);
    sql.add ("AND verse =");
    sql.add (verse);
    sql.add (";");
    sqlite3 * db = connect (file);
    vector <string> result = database_sqlite_query (db, sql.sql) ["html"];
    database_sqlite_disconnect (db);
    if (!result.empty ()) html = result [0];
  }
  return html;
}


// Return the names of the available offline resources.
vector <string> Database_OfflineResources::names ()
{
  // It looks for the same folder for both the normal cache, and for the central cache,
  // because the central cache is indicated by the presence of this folder also.
  return filter_url_scandir (mainFolder ());
}


// Returns the files in offline resource name.
vector <string> Database_OfflineResources::files (string name)
{
  return filter_url_scandir (resourceFolder (name));
}


// Returns the size of the file that contains resource name and file.
int Database_OfflineResources::size (const string & name, string file)
{
  // Normal file.
  file = filter_url_create_path (resourceFolder (name), file);
  int size = 0;
  if (file_exists (file)) size = filter_url_filesize (file);
  return size;
}


void Database_OfflineResources::unlink (const string & name, string file)
{
  string path = filter_url_create_path (resourceFolder (name), file);
  // If a folder: Delete it.
  filter_url_rmdir (path);
  // If a file: Delete it.
  filter_url_unlink (path);
}


// Saves an entire database in resource $name as file $file.
void Database_OfflineResources::save (const string & name, string file, const string & contents)
{
  string path = resourceFolder (name);
  if (!file_exists (path)) filter_url_mkdir (path);
  file = filter_url_create_path (path, file);
  filter_url_file_put_contents (file, contents);
}


// Returns the raw contents of an entire database in resource $name file $file.
string Database_OfflineResources::load (const string & name, const string & file)
{
  string path = filter_url_create_path (resourceFolder (name), file);
  string contents = filter_url_file_get_contents (path);
  return contents;
}


// Returns "databases": part of path.
string Database_OfflineResources::databases ()
{
  return "databases";
}


// Returns "offlineresources": part of path.
string Database_OfflineResources::offlineresources ()
{
  return "offlineresources";
}


// Returns the filename to get via http in order to get the $file.
string Database_OfflineResources::httpget (const string & name, const string & file)
{
  return "/" + filter_url_create_path (databases (), offlineresources (), name, file);
}


// Returns the filename of resource $name and file $file in local storage.
string Database_OfflineResources::filepath (const string & name, const string & file)
{
  return filter_url_create_path (resourceFolder (name), file);
}


// Link resource $name to the central cache.
void Database_OfflineResources::link_to_central_cache (const string & name)
{
  erase (name);
  string ourname = resourceFolder (name);
  string centralname = filter_url_create_path (config_logic_external_resources_cache_path (), name);
  symlink (centralname.c_str (), ourname.c_str());
}


string Database_OfflineResources::mainFolder ()
{
  return filter_url_create_root_path (databases (), offlineresources ());
}


string Database_OfflineResources::resourceFolder (string name)
{
  return filter_url_create_path (mainFolder (), name);
}


string Database_OfflineResources::databaseFile (string name, int book)
{
  return filter_url_create_path (resourceFolder (name), convert_to_string (book) + ".sqlite");
}


sqlite3 * Database_OfflineResources::connect (string file)
{
  return database_sqlite_connect (file);
}


// Whether the resource $name is linked to the central cache.
bool Database_OfflineResources::central_cache_linked (string name)
{
  return file_exists (resourceFolder (name));
}
