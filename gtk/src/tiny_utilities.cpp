/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#include "tiny_utilities.h"
#include <glib.h>


// The tiny utilities as listed below are mainly intended to be linked with 
// small executables


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
    // Default root folder.
    ustring default_root;
    default_root = tiny_gw_build_filename (g_get_home_dir(), ".bibledit");
    ustring new_root_pointer_file;
    new_root_pointer_file = tiny_gw_build_filename (g_get_home_dir(), ".bibledit-datafolder-pointer");
    if (g_file_test (new_root_pointer_file.c_str(), G_FILE_TEST_IS_REGULAR)) {
      // File exists: Read the root directory it contains.
      gchar *contents;
      g_file_get_contents (new_root_pointer_file.c_str(), &contents, NULL, NULL);
      root_directory = contents;
      g_free (contents);
      root_directory = trim (root_directory);
      cout << "Using non-standard datafolder " << root_directory << endl;
      // If it contains nothing, proceed with defaults.
      if (root_directory.empty()) {
        root_directory = default_root;
        cout << "This data folder has no name. Resetting to " << default_root << endl;
      }
      // If the new directory is not accessible, defaults.
      int result = access (root_directory.c_str(), W_OK);
      if (result != 0) {
        root_directory = default_root;
        cout << "There are not enough access permissions for this data folder. Resetting to "<< default_root << endl;
      }
    } else {
      // Ok, default situation.
      root_directory = default_root;
    }
    root_directory_initialized = true;
  }

  // Return the root of all data, usually $HOME/.bibledit
  return root_directory;
}


ustring tiny_directories_get_projects()
{
  // This returns the directory with all the projects.
  return tiny_gw_build_filename(tiny_directories_get_root(), "projects");
}


ustring tiny_project_directory(const ustring& project)
// Returns the project directory.
{
  return tiny_gw_build_filename (tiny_directories_get_projects(), project);
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


void tiny_spawn_write(int fd, const ustring & text)
{
  if (write(fd, text.c_str(), strlen(text.c_str()))) ;
}


TinySpawn::TinySpawn(const char *program)
{
  myprogram = program;
  myread = false;
  exitstatus = 0;
}


TinySpawn::~TinySpawn()
{
}


void TinySpawn::workingdirectory(ustring directory)
// The process' working directory.
{
  myworkingdirectory = directory;
}


void TinySpawn::arg(ustring value)
// Add one arguments to the arguments for running the program.
// This function can be repeated as many times as desired.
{
#ifdef WIN32
  // TODO(dennison): Find a portable way to quote the argument.
#else
  // Escape the '.
  // --- Why isn't this covered in shell_quote_space?
  size_t offposition = value.find("'");
  while (offposition != string::npos) {
    value.replace(offposition, 1, "\\'");
    offposition = value.find("'", offposition + 2);
  }
#endif
  // Save argument.
  arguments.push_back(value);
}


void TinySpawn::read()
// Make cout and cerr of the program available for later reading.
{
  myread = true;
}


void TinySpawn::run()
{
  // Working directory.
  const gchar *workingdirectory = NULL;
  if (!myworkingdirectory.empty())
    workingdirectory = myworkingdirectory.c_str();
  // Store arguments in argv.
  char *argv[arguments.size() + 2];
  argv[0] = (char *)myprogram;
  for (unsigned int i = 0; i < arguments.size(); i++) {
    argv[i + 1] = (char *)arguments[i].c_str();
  }
  // Terminate argv.
  argv[arguments.size() + 1] = NULL;
  // Spawn flags.
  int flags = G_SPAWN_SEARCH_PATH;
  // Possible pipes.
  gchar *standard_output = NULL;
  gchar *standard_error = NULL;
  gchar **standard_output_pointer = NULL;
  gchar **standard_error_pointer = NULL;
  if (myread) {
    standard_output_pointer = &standard_output;
    standard_error_pointer = &standard_error;
  }
  // Spawn process.
  result = g_spawn_sync(workingdirectory, argv, NULL, (GSpawnFlags) flags, NULL, NULL, standard_output_pointer, standard_error_pointer, &exitstatus, NULL);
  // Handle case we didn't spawn the process.
  if (!result) {
    exitstatus = -1;
    ustring message = myprogram;
    message.append(" didn't spawn");
    g_critical("%s", message.c_str());
    return;
  }
  // Handle reading the output.
  if (myread) {
    // In sync mode we have gchar * output.
    ParseLine parse_output (standard_output);
    standardout = parse_output.lines;
    ParseLine parse_error (standard_error);
    standarderr = parse_error.lines;
    // Free data.
    if (standard_output)
      g_free(standard_output);
    if (standard_error)
      g_free(standard_error);
  }
}


ParseLine::ParseLine(const ustring & text)
// Parses text in its separate lines.
{
  ustring processed_line;
  processed_line = trim(text);
  size_t newlineposition;
  newlineposition = processed_line.find("\n");
  while (newlineposition != string::npos) {
    ustring word = processed_line.substr(0, newlineposition);
    lines.push_back(trim(word));
    processed_line.erase(0, newlineposition + 1);
    processed_line = trim(processed_line);
    newlineposition = processed_line.find("\n");
  }
  if (!processed_line.empty())
    lines.push_back(trim(processed_line));
}


ParseLine::~ParseLine()
{
}
