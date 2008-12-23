/*
 ** Copyright (©) 2003-2008 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "tiny_utilities.h"
#include <glib.h>

// The tiny utilities as listed below are mainly intended to be linked with 
// small executables, such as for example bibledit-git.

unsigned int convert_to_int(const ustring & str)
{
  unsigned int i = 0;
  istringstream r(str);
  r >> i;
  return i;
}

ustring convert_to_string(int i)
{
  ostringstream r;
  r << i;
  return r.str();
}

ustring convert_to_string(unsigned int i)
{
  ostringstream r;
  r << i;
  return r.str();
}

ustring convert_to_string(bool b)
{
  if (b)
    return "1";
  else
    return "0";
}

ustring convert_to_string(double d)
{
  ostringstream r;
  r << d;
  return r.str();
}

ustring convert_to_string(long unsigned int i)
{
  ostringstream r;
  r << i;
  return r.str();
}

bool convert_to_bool(const ustring & s)
{
  if (s == "")
    return false;
  if (s == "0")
    return false;
  if (s == "F")
    return false;
  else
    return true;
}

double convert_to_double(const ustring & s)
{
  double result;
  istringstream i(s);
  i >> result;
  return result;
}

ustring convert_bool_to_yes_no(bool b)
{
  if (b)
    return "yes";
  else
    return "no";
}

ustring trim(const ustring & s)
{
  if (s.length() == 0)
    return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t beg = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  // No non-spaces  
  if (beg == string::npos)
    return "";
  return ustring(s, beg, end - beg + 1);
}

string trim(const string & s)
{
  if (s.length() == 0)
    return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t beg = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  // No non-spaces  
  if (beg == string::npos)
    return "";
  return string(s, beg, end - beg + 1);
}

ustring tiny_gw_build_filename(const ustring & part1, const ustring & part2)
// Wrapper for g_build_filename, to make programming easier.
{
  ustring filename;
  gchar *name;
  name = g_build_filename(part1.c_str(), part2.c_str(), NULL);
  filename = name;
  g_free(name);
  return filename;
}

ustring tiny_gw_build_filename(const ustring & part1, const ustring & part2, const ustring & part3)
{
  ustring filename;
  gchar *name;
  name = g_build_filename(part1.c_str(), part2.c_str(), part3.c_str(), NULL);
  filename = name;
  g_free(name);
  return filename;
}

ustring tiny_directories_get_root()
/*
 Returns the root directory of all data, using some logic to get there.
 */
{
  // Static variables to use.
  static bool root_directory_initialized = false;
  static ustring root_directory;

  if (!root_directory_initialized) {
    root_directory = tiny_gw_build_filename(g_get_home_dir(), ".bibledit");
  }
  root_directory_initialized = true;

  // Return the root of all data, <home>/.bibledit
  return root_directory;
}

ustring tiny_directories_get_projects()
{
  // This returns the directory with all the projects.
  return tiny_gw_build_filename(tiny_directories_get_root(), "projects");
}

ustring tiny_project_data_directory_part()
{
  return "data";
}

ustring tiny_project_data_directory_project(const ustring & project)
// Returns the data directory for the project, e.g.:
// ~/.bibledit/projects/testproject/data
{
  ustring directory = tiny_gw_build_filename(tiny_directories_get_projects(), project, tiny_project_data_directory_part());
  return directory;
}
