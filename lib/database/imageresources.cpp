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


#include <database/imageresources.h>
#include <filter/url.h>
#include <filter/string.h>


// Database resilience: 
// The data is stored as images and files in the filesystem.
// That should be resilient enough.


string Database_ImageResources::mainFolder ()
{
  return filter_url_create_root_path ("databases", "imageresources");
}


string Database_ImageResources::resourceFolder (const string& name)
{
  return filter_url_create_path (mainFolder (), name);
}


vector <string> Database_ImageResources::names ()
{
  return filter_url_scandir (mainFolder ());
}


void Database_ImageResources::create (string name)
{
  string path = resourceFolder (name);
  filter_url_unlink (path);
  filter_url_mkdir (path);
}


void Database_ImageResources::erase (string name)
{
  string path = resourceFolder (name);
  // If a folder: Delete it.
  filter_url_rmdir (path);
  // If a file: Delete it.
  filter_url_unlink (path);
}


void Database_ImageResources::erase (string name, string image) // Todo test
{
  string path = resourceFolder (name);
  path = filter_url_create_path (path, image);
  filter_url_unlink (path);
}


// Moves $file (path to an image file) into the database.
void Database_ImageResources::store (string name, string file) // Todo test
{
  string folder = resourceFolder (name);
  string basename = filter_url_basename (file);
  string path;
  bool exists = false;
  do {
    path = filter_url_create_path (path, basename);
    exists = file_exists (path);
    if (exists) basename = filter_string_str_replace (".", "0.", basename);
  } while (exists);
  filter_url_rename (file, path);
}


// Assign a passage range to the $image.
// It means that this image contains text for the passage range.
void Database_ImageResources::assign (string name, string image,
                                      int book1, int chapter1, int verse1,
                                      int book2, int chapter2, int verse2) // Todo test.
{
  
}


string Database_ImageResources::get (string name, int book, int chapter, int verse) // Todo write and test
{
  return "";
}


vector <string> Database_ImageResources::get (string name) // Todo test
{
  return filter_url_scandir (resourceFolder (name));
}
