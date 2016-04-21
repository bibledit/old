/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <database/git.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>


// Database resilience: It contains statistical and non-essential data.
// It is checked and optionally recreated at least once a day.


void Database_Git::create () // Todo
{
  bool healthy_database = database_sqlite_healthy (name ());
  if (!healthy_database) {
    filter_url_unlink (database_sqlite_file (name ()));
  }

  SqliteDatabase sql = SqliteDatabase (name ());
  
  // On Android, this pragma prevents the following error: VACUUM; Unable to open database file.
  sql.add ("PRAGMA temp_store = MEMORY;");
  sql.execute ();

  sql.clear ();

  sql.add ("CREATE TABLE IF NOT EXISTS changes ("
           " bible text,"
           " book integer,"
           " chapter integer,"
           " oldusfm text,"
           " newusfm text"
           ");");
  sql.execute ();
  
  sql.clear ();

  sql.add ("VACUUM;");
  sql.execute ();
}


void Database_Git::store_chapter (string bible, int book, int chapter, string oldusfm, string newusfm)
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("INSERT INTO changes VALUES (");
  sql.add (bible);
  sql.add (",");
  sql.add (book);
  sql.add (",");
  sql.add (chapter);
  sql.add (",");
  sql.add (oldusfm);
  sql.add (",");
  sql.add (newusfm);
  sql.add (");");
  sql.execute ();
}


vector <int> Database_Git::get_rowids ()
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("SELECT rowid FROM changes;");
  vector <string> values = sql.query () ["rowid"];
  vector <int> rowids;
  for (auto value : values) {
    rowids.push_back (convert_to_int (value));
  }
  return rowids;
}


bool Database_Git::get_chapter (int rowid,
                                string & bible, int & book, int & chapter,
                                string & oldusfm, string & newusfm)
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("SELECT bible, book, chapter, oldusfm, newusfm FROM changes WHERE rowid =");
  sql.add (rowid);
  sql.add (";");
  map <string, vector <string> > result = sql.query ();
  vector <string> bibles   = result ["bible"];
  vector <string> books    = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> oldusfms = result ["oldusfm"];
  vector <string> newusfms = result ["newusfm"];
  if (bibles.empty ()) return false;
  if (!bibles.empty ())   bible = bibles [0];
  if (!books.empty ())    book = convert_to_int (books [0]);
  if (!chapters.empty ()) chapter = convert_to_int (chapters [0]);
  if (oldusfm.empty ())   oldusfm = oldusfms [0];
  if (newusfm.empty ())   newusfm = newusfms [0];
  return true;
}


// Flag export of $bible $book to $format.
void Database_Git::erase_rowid (int rowid)
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("DELETE FROM changes WHERE rowid =");
  sql.add (rowid);
  sql.add (";");
  sql.execute ();
}


const char * Database_Git::name ()
{
  return "git";
}
