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


#include <database/kjv.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/globals.h>
#include <database/sqlite.h>


// This is the database for the Strong's numbers and English glosses.
// Resilience: It is never written to. 
// Chances of corruption are nearly zero.


sqlite3 * Database_Kjv::connect ()
{
  return database_sqlite_connect ("kjv");
}


// Get Strong's numbers and English snippets for book / chapter / verse.
vector <Database_Kjv_Item> Database_Kjv::getVerse (int book, int chapter, int verse)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT strong, english FROM kjv WHERE book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("AND verse =");
  sql.add (verse);
  sql.add (";");
  vector <Database_Kjv_Item> hits;
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  vector <string> strongs = result ["strong"];
  vector <string> english = result ["english"];
  for (unsigned int i = 0; i < strongs.size (); i++) {
    Database_Kjv_Item item = Database_Kjv_Item ();
    item.strong = strongs [i];
    item.english = english [i];
    hits.push_back (item);
  }
  return hits;
}


// Get all passages that contain a strong's number.
vector <Passage> Database_Kjv::searchStrong (string strong)
{
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT DISTINCT book, chapter, verse FROM kjv WHERE strong =");
  sql.add (strong);
  sql.add (";");
  vector <Passage> hits;
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  vector <string> books = result ["book"];
  vector <string> chapters = result ["chapter"];
  vector <string> verses = result ["verse"];
  for (unsigned int i = 0; i < books.size (); i++) {
    Passage passage = Passage ();
    passage.book = convert_to_int (books [i]);
    passage.chapter = convert_to_int (chapters [i]);
    passage.verse = verses [i];
    hits.push_back (passage);
  }
  return hits;
}


