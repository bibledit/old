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


#include <database/rss.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/date.h>
#include <database/sqlite.h>


// Database resilience: It contains statistical and non-essential data.
// It is checked and optionally recreated at least once a day.


void Database_Rss::create ()
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("CREATE TABLE IF NOT EXISTS changes ("
           " timestamp integer,"
           " user text,"
           " bible text,"
           " book integer,"
           " chapter integer,"
           " oldusfm text,"
           " newusfm text"
           ");");
  sql.execute ();
}


void Database_Rss::optimize ()
{
  bool healthy_database = database_sqlite_healthy (name ());
  if (!healthy_database) {
    filter_url_unlink (database_sqlite_file (name ()));
    create ();
  }
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("VACUUM;");
  sql.execute ();
}


void Database_Rss::store (string user, string bible, int book, int chapter,
                          string oldusfm, string newusfm)
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("INSERT INTO changes VALUES (");
  sql.add (filter_date_seconds_since_epoch ());
  sql.add (",");
  sql.add (user);
  sql.add (",");
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


int Database_Rss::size ()
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("SELECT count(*) FROM changes;");
  vector <string> result = sql.query () ["count(*)"];
  int count = 0;
  if (!result.empty ()) count = convert_to_int (result [0]);
  return count;
}


void Database_Rss::fetch (string & user, string & bible, int & book, int & chapter,
                          string & oldusfm, string & newusfm)
{
  SqliteDatabase sql = SqliteDatabase (name ());
  sql.add ("SELECT * FROM changes ORDER BY rowid LIMIT 1;");
  map <string, vector <string> > result = sql.query ();
  vector <string> users    = result ["user"];
  vector <string> bibles   = result ["bible"];
  vector <string> books    = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> oldusfms = result ["oldusfm"];
  vector <string> newusfms = result ["newusfm"];
  if (!users.empty ())    user    = users [0];
  if (!bibles.empty ())   bible   = bibles [0];
  if (!books.empty ())    book    = convert_to_int (books [0]);
  if (!chapters.empty ()) chapter = convert_to_int (chapters [0]);
  if (oldusfm.empty ())   oldusfm = oldusfms [0];
  if (newusfm.empty ())   newusfm = newusfms [0];
  sql.clear ();
  sql.add ("DELETE FROM changes WHERE rowid = (SELECT rowid FROM changes ORDER BY rowid LIMIT 1);");
  sql.execute ();
}


const char * Database_Rss::name ()
{
  return "rss";
}
