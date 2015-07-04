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


#include <locale/logic.h>
#include <locale/translate.h>
#include <filter/string.h>
#include <filter/url.h>


// Filters out the default language.
// Returns the $localization, or "" in case of the default language.
string locale_logic_filter_default_language (string localization)
{
  if (localization == "default") localization.clear ();
  return localization;
}


// Takes a month from 1 to 12, and returns its localized name.
string locale_logic_month (int month)
{
  switch (month) {
    case 1:  return translate ("January");
    case 2:  return translate ("February");
    case 3:  return translate ("March");
    case 4:  return translate ("April");
    case 5:  return translate ("May");
    case 6:  return translate ("June");
    case 7:  return translate ("July");
    case 8:  return translate ("August");
    case 9:  return translate ("September");
    case 10: return translate ("October");
    case 11: return translate ("November");
    case 12: return translate ("December");
  }
  return translate ("Month") + " " + convert_to_string (month);
}


// Return the available localizations.
map <string, string> locale_logic_localizations ()
{
  string directory = filter_url_create_root_path ("locale");
  vector <string> files = filter_url_scandir (directory);
  map <string, string> localizations = {make_pair ("", "Default")};
  for (auto file : files) {
    string suffix = filter_url_get_extension (file);
    if (suffix == "po") {
      string basename = filter_string_str_replace ("." + suffix, "", file);
      string path = filter_url_create_path (directory, file);
      string contents = filter_url_file_get_contents (path);
      string language = "Unknown";
      vector <string> lines = filter_string_explode (contents, '\n');
      for (auto line : lines) {
        if (line.find ("translation for bibledit") != string::npos) {
          line.erase (0, 2);
          line.erase (line.length () - 25);
          language = line;
        }
      }
      localizations.insert (make_pair (basename, language));
    }
  }
  return localizations;
}


map <string, string> locale_logic_read_po (string file) // Todo implement and test.
{
  map <string, string> translations;
  string contents = filter_url_file_get_contents (file);
  vector <string> lines = filter_string_explode (contents, '\n');
  for (auto & line : lines) {
    cout << line << endl; // Todo
  }
  return translations;
}
