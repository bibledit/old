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


#ifndef INCLUDED_DATABASE_OFFLINERESOURCES_H
#define INCLUDED_DATABASE_OFFLINERESOURCES_H


#include <config/libraries.h>
#include <sqlite3.h>


class Database_OfflineResources
{
public:
  void erase (string name);
  void store (string name, int book, int chapter, int verse, string html);
  void healthy (string file);
  bool exists (string name, int book, int chapter, int verse);
  bool exists (string name);
  int count (string name);
  string get (string name, int book, int chapter, int verse);
  vector <string> names ();
  vector <string> files (string name);
  int size (const string & name, string file);
  void unlink (const string & name, string file);
  void save (const string & name, string file, const string & contents);
  string load (const string & name, const string & file);
  static string databases ();
  static string offlineresources ();
  static string httpget (const string & name, const string & file);
  string filepath (const string & name, const string & file);
  void link_to_central_cache (const string & name);
private:
  string mainFolder ();
  string resourceFolder (string name);
  string databaseFile (string name, int book);
  bool central_cache_linked (string name);
  sqlite3 * connect (string file);
};


#endif
