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


#ifndef INCLUDED_DATABASE_SEARCH_H
#define INCLUDED_DATABASE_SEARCH_H


#include <config/libraries.h>
#include <sqlite3.h>
#include <filter/passage.h>


class Database_Search
{
public:
  Database_Search ();
  ~Database_Search ();
  void create ();
  void optimize ();
  bool checkup (bool recreate = false);
  void updateSearchFields (string name, int book, int chapter);
  vector <int> searchText (string search, vector <string> bibles);
  vector <int> searchBibleText (string bible, string search);
  vector <int> searchBibleTextCaseSensitive (string bible, string search);
  vector <int> searchBibleUsfm (string bible, string search);
  vector <int> searchBibleUsfmCaseSensitive (string bible, string search);
  Passage getBiblePassage (int rowid);
  string getBibleVerseText (string bible, int book, int chapter, int verse);
  string getBibleVerseUsfm (string bible, int book, int chapter, int verse);
  vector <string> getBibles ();
  void deleteBible (string bible);
  vector <int> getBooks (string bible);
  void deleteBook (string bible, int book);
  vector <int> getChapters (string bible, int book);
  void deleteChapter (string bible, int book, int chapter);
  vector <int> getVerses (string bible, int book, int chapter);
  int getVerseCount (string bible);
private:
  sqlite3 * connect ();
};


#endif
