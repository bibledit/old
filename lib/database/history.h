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


#ifndef INCLUDED_DATABASE_HISTORY_H
#define INCLUDED_DATABASE_HISTORY_H


#include <config/libraries.h>
#include <sqlite3.h>


class Database_History_Item
{
public:
  int timestamp;
  string author;
  string bible;
  int book;
  int chapter;
  int verse;
  string oldtext;
  string modification;
  string newtext;
};


class Database_History
{
public:
  Database_History ();
  ~Database_History ();
  void create ();
  void optimize ();
  void trim ();
  void record (int timestamp, string author, string bible, int book, int chapter, int verse, string oldtext, string modification, string newtext);
  int count (string author, vector <string> bibles, int book, int chapter, int verse);
  vector <Database_History_Item> get (string author, string bible, int book, int chapter, int verse, int start);
  vector <string> authors (vector <string> bibles);
private:
  string query (string author, vector <string> bibles, int book, int chapter, int verse, bool count, int start);
  sqlite3 * connect ();
};


#endif
