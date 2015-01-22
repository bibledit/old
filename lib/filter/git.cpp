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


#include <filter/git.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/logs.h>
#include <database/books.h>
#include <config.h>
#include <git2.h>


// This function returns the directory of the git repository belonging to $object.
string filter_git_directory (string object)
{
  return filter_url_create_root_path ("git", object);
}


void filter_git_check_error (int result)
{
  if (result != 0) {
    const git_error * error = giterr_last ();
    Database_Logs::log (error->message);
  }
}


// Runs the equivalent of "git init".
bool filter_git_init (string directory)
{
  git_repository *repo = NULL;
  int result = git_repository_init (&repo, directory.c_str(), false);
  filter_git_check_error (result);
  git_repository_free (repo);
  return (result == 0);
}


// This filter takes the Bible data as it is stored in Bibledit's database,
// and puts this information into the layout in books and chapters
// such as is used in Bibledit-Gtk into the $git folder.
// The $git is a git repository, and may contain other data as well.
// The filter focuses on reading the data in the git repository, and only writes to it if necessary,
// This speeds up the filter.
void filter_git_sync_bible_to_git (void *webserver_request, string bible, string repository) // Todo
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  // First stage.
  // Read the chapters in the git repository,
  // and check if they occur in the database.
  // If a chapter is not in the database, remove it from the repository.
  vector <int> books = request->database_bibles()->getBooks (bible);
  vector <string> bookfiles = filter_url_scandir (repository);
  for (auto & bookname : bookfiles) {
    string path = filter_url_create_path (repository, bookname);
    if (filter_url_is_dir (path)) {
      int book = Database_Books::getIdFromEnglish (bookname);
      if (book) {
        if (in_array (book, books)) {
          // Book exists in the database: Check the chapters.
          vector <int> chapters = request->database_bibles()->getChapters (bible, book);
          vector <string> chapterfiles = filter_url_scandir (filter_url_create_path (repository, bookname));
          for (auto & chaptername : chapterfiles) {
            string path = filter_url_create_path (repository, bookname, chaptername);
            if (filter_url_is_dir (path)) {
              if (filter_string_is_numeric (chaptername)) {
                int chapter = convert_to_int (chaptername);
                string filename = filter_url_create_path (repository, bookname, chaptername, "data");
                if (filter_url_file_exists (filename)) {
                  if (!in_array (chapter, chapters)) {
                    // Chapter does not exist in the database.
                    filter_url_rmdir (filter_url_create_path (repository, bookname, chaptername));
                  }
                }
              }
            }
          }
        } else {
          // Book does not exist in the database: Remove it from $git.
          filter_url_rmdir (filter_url_create_path (repository, bookname));
        }
      }
    }
  }
  
  // Second stage.
  // Read the books / chapters from the database,
  // and check if they occur in the repository, and the data matches.
  // If necessary, save the chapter to the repository.
  books = request->database_bibles()->getBooks (bible);
  for (auto & book : books) {
    string bookname = Database_Books::getEnglishFromId (book);
    string bookdir = filter_url_create_path (repository, bookname);
    if (!filter_url_file_exists (bookdir)) filter_url_mkdir (bookdir);
    vector <int> chapters = request->database_bibles()->getChapters (bible, book);
    for (auto & chapter : chapters) {
      string chapterdir = filter_url_create_path (bookdir, to_string (chapter));;
      if (!filter_url_file_exists (chapterdir)) filter_url_mkdir (chapterdir);
      string datafile = filter_url_create_path (chapterdir, "data");
      string contents = filter_url_file_get_contents (datafile);
      string usfm = request->database_bibles()->getChapter (bible, book, chapter);
      if (contents != usfm) filter_url_file_put_contents (datafile, usfm);
    }
  }
}
