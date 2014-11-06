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


#include <database/bibles.h>
#include <filter/url.h>
#include <filter/string.h>


using namespace std;


// This database stores its data in files in the filesystem.
// This is a rugged and reliable system of storing data.
// Because no real database is used, no database can get corrupted.


Database_Bibles::Database_Bibles ()
{
}


Database_Bibles::~Database_Bibles ()
{
}


string Database_Bibles::mainFolder ()
{
  return filter_url_create_root_path ("bibles");
}


string Database_Bibles::bibleFolder (string bible)
{
  return filter_url_create_path (mainFolder (), bible);
}


string Database_Bibles::bookFolder (string bible, int book)
{
  return filter_url_create_path (bibleFolder (bible), convert_to_string (book));
}


string Database_Bibles::chapterFolder (string bible, int book, int chapter)
{
  return filter_url_create_path (bookFolder (bible, book), convert_to_string (chapter));
}


// Returns an array with the available Bibles.
vector <string> Database_Bibles::getBibles ()
{
  vector <string> bibles = filter_url_scandir (mainFolder ());
  return bibles;
}


// Returns the ID for a named Bible.
int Database_Bibles::getID (string name)
{
  string file = filter_url_create_path (bibleFolder (name), "id");
  if (filter_url_file_exists (file)) {
    return convert_to_int (filter_url_file_get_contents (file));
  }
  return 0;
}


// Returns the Bible name for a Bible ID.
string Database_Bibles::getName (int id)
{
  vector <string> bibles = getBibles ();
  for (string bible : bibles) {
    int bibleID = getID (bible);
    if (id == bibleID) return bible;
  }
  return "Unknown";
}


// Creates a new empty Bible. Returns its ID.
int Database_Bibles::createBible (string name)
{
  // If the Bible already exists, return its ID.
  int id = getID (name);
  if (id > 0) return id;

  // Get IDs in use.
  vector <int> ids;
  vector <string> bibles = getBibles ();
  for (auto bible : bibles) {
    ids.push_back (getID (bible));
  }
  
  // Get the first free ID.
  id = 1;
  while (find (ids.begin (), ids.end (), id) != ids.end ()) id++;

  // Create the empty system.
  string folder = bibleFolder (name);
  filter_url_mkdir (folder);
  filter_url_file_put_contents (filter_url_create_path (folder, "id"), convert_to_string (id));

  // Return new ID.
  return id;
}


// Deletes a Bible.
void Database_Bibles::deleteBible (string name)
{
  filter_url_rmdir (bibleFolder (name));
}


// Stores data of one chapter in Bible $name,
void Database_Bibles::storeChapter (string name, int book, int chapter_number, string chapter_text)
{
  string folder = chapterFolder (name, book, chapter_number);
  if (!filter_url_file_exists (folder)) filter_url_mkdir (folder);

  // Increase the chapter identifier, and store the chapter data.
  int id = getChapterId (name, book, chapter_number);
  id++;
  string file = filter_url_create_path (folder, convert_to_string (id));
  filter_url_file_put_contents (file, chapter_text);

  // Update search fields.
  updateSearchFields (name, book, chapter_number);
}


void Database_Bibles::updateSearchFields (string name, int book, int chapter)
{
  if (book) name.append (name); // Todo temporal: OUT.
  if (chapter) {};
  /* C++Port
  $database_search = Database_Search::getInstance ();
  $database_search->updateSearchFields ($name, $book, $chapter);
  */
}


// Returns an array with the available books in a Bible.
vector <int> Database_Bibles::getBooks (string bible)
{
  // Read the books from the database.
  string folder = bibleFolder (bible);
  vector <int> books;
  vector <string> files = filter_url_scandir (folder);
  for (string book : files) {
    if (filter_string_is_numeric (book)) books.push_back (convert_to_int (book));
  }

  // Sort the books according to the sequence defined in the books database.
  /* C++Port
  $sequence = array ();
  $database_books = Database_Books::getInstance ();
  foreach ($books as $book) {
    $sequence [] = $database_books->getSequenceFromId ($book);
  }
  array_multisort ($sequence, $books);
  */

  // Result.
  return books;
}


void Database_Bibles::deleteBook (string bible, int book)
{
  string folder = bookFolder (bible, book);
  filter_url_rmdir (folder);
}


// Returns an array with the available chapters in a $book in a Bible.
vector <int> Database_Bibles::getChapters (string bible, int book)
{
  // Read the chapters from the database.
  string folder = bookFolder (bible, book);
  vector <int> chapters;
  vector <string> files = filter_url_scandir (folder);
  for (string file : files) {
    if (filter_string_is_numeric (file)) chapters.push_back (convert_to_int (file));
  }
  sort (chapters.begin (), chapters.end ());
  return chapters;
}


void Database_Bibles::deleteChapter (string bible, int book, int chapter)
{
  string folder = chapterFolder (bible, book, chapter);
  filter_url_rmdir (folder);
}


// Gets the chapter data as a string.
string Database_Bibles::getChapter (string bible, int book, int chapter)
{
  string folder = chapterFolder (bible, book, chapter);
  vector <string> files = filter_url_scandir (folder);
  if (!files.empty ()) {
    string file = files [files.size () - 1];
    string data = filter_url_file_get_contents (filter_url_create_path (folder, file));
    return data;
  }
  return "";
}


// Gets the chapter id.
int Database_Bibles::getChapterId (string bible, int book, int chapter)
{
  string folder = chapterFolder (bible, book, chapter);
  vector <string> files = filter_url_scandir (folder);
  if (!files.empty ()) {
    string file = files [files.size() - 1];
    return convert_to_int (file);
  }
  return 100000000;
}


void Database_Bibles::optimize ()
{
  /* C++Port
  // Go through all chapters in all books and all Ḃibles.
  $bibles = $this->getBibles ();
  foreach ($bibles as $bible) {
    $books = $this->getBooks ($bible);
    foreach ($books as $book) {
      $chapters = $this->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        $folder = $this->chapterFolder ($bible, $book, $chapter);
        // Read the files in the folder.
        $files = scandir ($folder);
        // Remove the "." and ".." entries.
        // Because scandir sorts the files, those entries are at the start of the array.
        array_shift ($files);
        array_shift ($files);
        // Remove files with 0 size. so that in case a chapter was emptied by accident, 
        // it is removed now, effectually reverting the chapter to an earlier version.
        foreach ($files as $offset => $file) {
          if (filesize ("$folder/$file") == 0) {
            unlink ("$folder/$file");
            unset ($files [$offset]);
          }
        }
        // Remove the three most recent files from the array, so they don't get deleted.
        // Because scandir sorts the files, the files to be removed are at the end.
        array_pop ($files);
        array_pop ($files);
        array_pop ($files);
        // Remove the remaining files. These are the older versions.
        foreach ($files as $file) {
          unlink ("$folder/$file");
        }
      }
    }
  }
  */
}

