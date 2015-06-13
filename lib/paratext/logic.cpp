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


#include <paratext/logic.h>
#include <filter/url.h>
#include <filter/string.h>
#include <pwd.h>
#include <database/books.h>


string Paratext_Logic::searchProjectsFolder ()
{
  const char *homedir;

  // Try Linux.
  if ((homedir = getenv("HOME")) == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }
  if (homedir) {
    vector <string> files = filter_url_scandir (homedir);
    for (auto file : files) {
      if (file.find ("Paratext") != string::npos) {
        return filter_url_create_path (homedir, file);
      }
    }
  }
  
  // Try Windows.
  homedir = "C:\\";
  vector <string> files = filter_url_scandir (homedir);
  for (auto file : files) {
    if (file.find ("Paratext") != string::npos) {
      string path = filter_url_create_path (homedir, file);
      path = filter_string_str_replace ("\\/", "\\", path);
      return path;
    }
  }
  
  // No Paratext projects folder found.
  return "";
}


vector <string> Paratext_Logic::searchProjects (string projects_folder)
{
  vector <string> projects;
  vector <string> folders = filter_url_scandir (projects_folder);
  for (auto folder : folders) {
    string path = filter_url_create_path (projects_folder, folder);
    if (filter_url_is_dir (path)) {
      vector <string> books = searchBooks (path);
      if (!books.empty ()) projects.push_back (folder);
    }
  }
  return projects;
}


vector <string> Paratext_Logic::searchBooks (string project_path)
{
  vector <string> books;
  vector <string> files = filter_url_scandir (project_path);
  for (auto file : files) {
    if (file.find (".BAK") != string::npos) continue;
    if (file.find ("~") != string::npos) continue;
    string path = filter_url_create_path (project_path, file);
    int id = getBook (path);
    if (id) books.push_back (file);
  }
  return books;
}


int Paratext_Logic::getBook (string filename)
{
  // A USFM file should not be larger than 4 Mb and not be smaller than 7 bytes.
  int filesize = filter_url_filesize (filename);
  if (filesize < 7) return 0;
  if (filesize > 4000000) return 0;
  
  // Read a small portion of the file for higher speed.
  ifstream fin (filename);
  fin.seekg (0);
  char buffer [128];
  fin.read (buffer, 7);
  buffer [7] = 0;
  string fragment (buffer);

  // Check for "\id "
  if (fragment.find ("\\id ") == string::npos) return 0;
  fragment.erase (0, 4);
  
  // Get book from the USFM id.
  int id = Database_Books::getIdFromUsfm (fragment);
  return id;
}
