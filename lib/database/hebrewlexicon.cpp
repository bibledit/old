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


#include <database/hebrewlexicon.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/sqlite.h>
#include <sqlite3.h>


// This is the database that contains Open Scriptures's Hebrew Lexicon.
// Resilience: It is not written to.
// Chances of corruption are nearly zero.


const char * Database_HebrewLexicon::filename ()
{
  return "hebrewlexicon";
}


void Database_HebrewLexicon::create ()
{
  filter_url_unlink (database_sqlite_file (filename ()));
  
  SqliteDatabase sql = SqliteDatabase (filename ());

  sql.clear ();
  sql.add ("CREATE TABLE IF NOT EXISTS strong (strong text, definition text);");
  sql.execute ();
}


void Database_HebrewLexicon::optimize ()
{
  SqliteDatabase sql = SqliteDatabase (filename ());
  sql.add ("VACUUM;");
  sql.execute ();
}


void Database_HebrewLexicon::storestrong (string strong, string definition)
{
  SqliteDatabase sql = SqliteDatabase (filename ());
  sql.add ("INSERT INTO strong VALUES (");
  sql.add (strong);
  sql.add (",");
  sql.add (definition);
  sql.add (");");
  sql.execute ();
}


/*
vector <int> Database_HebrewLexicon::rowids (int book, int chapter, int verse)
{
  SqliteDatabase sql = SqliteDatabase (filename ());
  sql.add ("SELECT rowid FROM morphgnt WHERE book =");
  sql.add (book);
  sql.add ("AND chapter =");
  sql.add (chapter);
  sql.add ("AND verse =");
  sql.add (verse);
  sql.add ("ORDER BY rowid;");
  vector <string> result = sql.query () ["rowid"];
  vector <int> rowids;
  for (auto rowid : result) rowids.push_back (convert_to_int (rowid));
  return rowids;
}

*/