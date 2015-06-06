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


#include <bible/import_task.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/archive.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/logs.h>
#include <database/books.h>
#include <bible/logic.h>


void recursive_directory_scan_for_files (string folder, vector <string> & paths)
{
  vector <string> files = filter_url_scandir (folder);
  for (auto & file : files) {
    string path = filter_url_create_path (folder, file);
    if (filter_url_is_dir (path)) {
      recursive_directory_scan_for_files (path, paths);
    } else {
      paths.push_back (path);
    }
  }
}


void bible_import_task (string location, string bible)
{
  Database_Logs::log ("Importing Bible data from location " + location + " into Bible " + bible);

  string folder = filter_archive_uncompress (location);
  if (folder != "") location = folder;
  vector <string> files;
  if (filter_url_is_dir (location)) {
    recursive_directory_scan_for_files (location, files);
  } else {
    files.push_back (location);
  }
  
  Webserver_Request request;
  string stylesheet = request.database_config_user ()->getStylesheet();
  
  for (auto & file : files) {
    Database_Logs::log ("Examining file for import: " + file);
    string success_message = "";
    string error_message = "";
    string data = filter_url_file_get_contents (file);
    if (data != "") {
      if (unicode_string_is_valid (data)) {
        vector <BookChapterData> book_chapter_text = usfm_import (data, stylesheet);
        for (auto & data : book_chapter_text) {
          int book_number = data.book;
          int chapter_number = data.chapter;
          string chapter_data = data.data;
          if (book_number > 0) {
            Bible_Logic::storeChapter (bible, book_number, chapter_number, chapter_data);
            string book_name = Database_Books::getUsfmFromId (book_number);
            Database_Logs::log ("Imported " + book_name + " " + convert_to_string (chapter_number));
          } else {
            Database_Logs::log ("Could not import this data: " + chapter_data.substr (0, 1000));
          }
        }
      } else {
        Database_Logs::log ("The file does not contain valid Unicode UTF-8 text.", true);
      }
    } else {
      Database_Logs::log ("Nothing was imported.", true);
    }
  }
  
  Database_Logs::log ("Import Bible data has finished");
}