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


#ifndef INCLUDED_SYNC_LOGIC_H
#define INCLUDED_SYNC_LOGIC_H


#include <config/libraries.h>
#include <sqlite3.h>
#include <filter/passage.h>


class Sync_Logic_Range
{
public:
  int low;
  int high;
};


class Sync_Logic
{
public:
  Sync_Logic (void * webserver_request_in);
  ~Sync_Logic ();
  static const int settings_get_total_checksum = 1;
  static const int settings_send_workbench_urls = 2;
  static const int settings_get_workbench_urls = 3;
  static const int settings_send_workbench_widths = 4;
  static const int settings_get_workbench_widths = 5;
  static const int settings_send_workbench_heights = 6;
  static const int settings_get_workbench_heights = 7;
  static const int bibles_get_total_checksum = 1;
  static const int bibles_get_bibles = 2;
  static const int bibles_get_bible_checksum = 3;
  static const int bibles_get_books = 4;
  static const int bibles_get_book_checksum = 5;
  static const int bibles_get_chapters = 6;
  static const int bibles_get_chapter_checksum = 7;
  static const int bibles_send_chapter = 8;
  static const int bibles_get_chapter = 9;
  static const int usfmresources_get_total_checksum = 1;
  static const int usfmresources_get_resources = 2;
  static const int usfmresources_get_resource_checksum = 3;
  static const int usfmresources_get_books = 4;
  static const int usfmresources_get_book_checksum = 5;
  static const int usfmresources_get_chapters = 6;
  static const int usfmresources_get_chapter_checksum = 7;
  static const int usfmresources_get_chapter = 8;
  bool credentials_okay ();
  string settings_checksum ();
  string checksum (const vector <int> & identifiers);
  vector <Sync_Logic_Range> create_range (int start, int end);
  string post (map <string, string> & post, const string& url, string & error);
  static string usfm_resources_checksum ();
  static string usfm_resource_checksum (const string& name);
  static string usfm_resource_book_checksum (const string& name, int book);
  static string usfm_resource_chapter_checksum (const string& name, int book, int chapter);
private:
  void * webserver_request;
};


#endif
