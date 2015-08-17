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


#include <database/morphology.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>


// This is the database for the Bible text morphology.
// Resilience: It is never written to. 
// Chances of corruption are nearly zero.


// Get text and parsing.
vector <Database_Morphology_Item> Database_Morphology::get (int book, int chapter, int verse) // Todo
{
  vector <Database_Morphology_Item> items;
  bool greek = (book >= 40);
  SqliteSQL sql = SqliteSQL ();
  sql.add ("SELECT pos, parsing, word, lemma FROM");
  if (greek) {
    sql.add ("morphgnt");
  } else {
    sql.add ("morphhb");
  }
  sql.add ("WHERE book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("AND verse =");
  sql.add (verse);
  sql.add ("ORDER BY rowid;");
  sqlite3 * db;
  if (greek) {
    db = database_sqlite_connect ("morphgnt");
  } else {
    db = database_sqlite_connect ("morphhb");
  }
  map <string, vector <string> > results = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  vector <string> parts_of_speech = results ["pos"];
  vector <string> parsings = results ["parsing"];
  vector <string> words = results ["word"];
  vector <string> lemmas = results ["lemma"];
  for (size_t i = 0; i < parts_of_speech.size (); i++) {
    Database_Morphology_Item item;
    item.part_of_speech = parts_of_speech [i];
    item.parsing = parsings[i];
    item.word = words[i];
    item.lemma = lemmas[i];
    items.push_back (item);
  }
  return items;
}
