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


string Paratext_Logic::searchParatextProjectsFolder ()
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
  
  // Try Windows. Todo test it on real Windows on Windows XP with Paratext installed.
  homedir = "C:\\";
  vector <string> files = filter_url_scandir (homedir);
  for (auto file : files) {
    if (file.find ("Paratext") != string::npos) {
      string path = filter_url_create_path (homedir, file);
      path = filter_string_str_replace ("\\/", "\\", path); // Todo test it.
      return path;
    }
  }
  
  // No Paratext projects folder found.
  return "";
}


