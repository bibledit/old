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


#ifndef INCLUDED_DATABASE_VERSIFICATIONS_H
#define INCLUDED_DATABASE_VERSIFICATIONS_H


#include <config/libraries.h>
#include <sqlite3.h>
#include <filter/passage.h>


class Database_Versifications
{
public:
  Database_Versifications ();
  ~Database_Versifications ();
  void create ();
  void defaults ();
  void optimize ();
  void input (const string& contents, const string& name);
  string output (const string& name);
  void erase (const string& name);
  int getID (const string& name);
  int createSystem (const string& name);
  vector <string> getSystems ();
  vector <Passage> getBooksChaptersVerses (const string& name);
  vector <int> getBooks (const string& name);
  vector <int> getChapters (const string& name, int book, bool include0 = false);
  vector <int> getVerses (const string& name, int book, int chapter);
private:
  sqlite3 * connect ();
  void RussianCanonical (sqlite3 * db, const string& sql);
  void DutchTraditional (sqlite3 * db, const string& sql);
  void Vulgate (sqlite3 * db, const string& sql);
  void RussianProtestant (sqlite3 * db, const string& sql);
  void RussianOrthodox (sqlite3 * db, const string& sql);
  void Septuagint (sqlite3 * db, const string& sql);
  void English (sqlite3 * db, const string& sql);
  void Spanish (sqlite3 * db, const string& sql);
  void Original (sqlite3 * db, const string& sql);
  void StatenBible (sqlite3 * db, const string& sql);
};


#endif
