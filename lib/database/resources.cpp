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


#include <database/resources.h>
#include <filter/url.h>
#include <filter/string.h>


// Robustness: Data is stored on the filesystem.


Database_Resources::Database_Resources ()
{
}


Database_Resources::~Database_Resources ()
{
}


string Database_Resources::folder ()
{
  return filter_url_create_root_path ("resources");
}


string Database_Resources::getFile (string name)
{
  name = filter_string_str_replace (" ", "_", name);
  string file = filter_url_create_path (folder (), name + "php");
  return file;
}


string Database_Resources::getInclude (string name)
{
  name = filter_string_str_replace (" ", "_", name);
  string file = filter_url_create_path (folder (), name + ".php");
  return file;
}


string Database_Resources::getMetaFile (string name)
{
  string file = getFile (name);
  file = filter_string_str_replace (".php", "_meta.php", file);
  return file;
}


string Database_Resources::getVersification (const string& name)
{
  string versification = "English";
  // C++Port @include ...
  getMetaFile (name);
  return versification;
}


string Database_Resources::getMapping (const string& name)
{
  string mapping = "English";
  // C++Port @include ...
  getMetaFile (name);
  return mapping;
}


void Database_Resources::save (const string& name, const string& code)
{
 string file = getFile (name);
 filter_url_file_put_contents (file, code);
}


// Gets the names of the available external resource scripts.
// The names are derived from the filenames of the resource scripts, as follows:
// * An underscore in the filenames becomes a space in the resource name.
// * The .php extension is removed.
// * Scripts ending on _meta.php are not considered. They contain meta information for that resource.
vector <string> Database_Resources::getNames ()
{
  vector <string> names;
  vector <string> files = filter_url_scandir (folder ());
  for (auto file : files) {
    if (file == unicode_string_casefold (file)) continue;
    if (file.find ("_meta.php") != string::npos) continue;
    string suffix = filter_url_get_extension (file);
    if (suffix != "php") continue;
    string name = file.substr (0, strlen (file.c_str()) -1 - strlen (suffix.c_str()));
    name = filter_string_str_replace ("_", " ", name);
    names.push_back (name);
  }
  return names;
}


void Database_Resources::erase (const string& name)
{
  filter_url_unlink (getFile (name));
}


