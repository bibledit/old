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


#include <webserver/http.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/UriCodec.cpp>
#ifdef WIN32
#include <direct.h>
#endif
#include <filter/string.h>
#include <config.h>


// Gets the base URL of current Bibledit installation.
string get_base_url (Webserver_Request * request)
{
  // E.g. http or https: Always use http for just now.
  string scheme = "http";  
  // Port
  string port = "8080"; 
  // Full URL.  
  string url = scheme + "://" + request->host + ":" + port + "/";
  return url;
}


// This function redirects the browser to "path".
// "path" is an absolute value.
void redirect_browser (string path, Webserver_Request * request)
{
  // A location header needs to contain an absolute url, like http://localhost/some.php.
  // See 14.30 in the specification http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html.
  string location = get_base_url (request) + path;
  request->header = "Location: " + location;
  request->response_code = 302;
}


// C++ replacement for the dirname function, see http://linux.die.net/man/3/dirname.
string filter_url_dirname (string url)
{
  if (!url.empty ()) {
    if (url.find_last_of (DIRECTORY_SEPARATOR) == url.length () - 1) {
      // Remove trailing slash.
      url = url.substr (0, url.length () - 1);
    }
    size_t pos = url.find_last_of (DIRECTORY_SEPARATOR);
    if (pos != string::npos) url = url.substr (0, pos);
    else url = "";
  }
  if (url.empty ()) url = ".";
  return url;
}


// C++ replacement for the basename function, see http://linux.die.net/man/3/basename.
string filter_url_basename (string url)
{
  if (!url.empty ()) {
    if (url.find_last_of (DIRECTORY_SEPARATOR) == url.length () - 1) {
      // Remove trailing slash.
      url = url.substr (0, url.length () - 1);
    }
    size_t pos = url.find_last_of (DIRECTORY_SEPARATOR);
    if (pos != string::npos) url = url.substr (pos + 1);
  }
  return url;
}


void filter_url_unlink (string filename)
{
  unlink (filename.c_str ());
}


void filter_url_rename (const string& oldfilename, const string& newfilename)
{
  rename (oldfilename.c_str(), newfilename.c_str());
}


// Creates a file path out of the components.
string filter_url_create_path (string part1, string part2, string part3, string part4, string part5, string part6)
{
  string path (part1);
  if (part2.length()) path += DIRECTORY_SEPARATOR + part2;
  if (part3.length()) path += DIRECTORY_SEPARATOR + part3;
  if (part4.length()) path += DIRECTORY_SEPARATOR + part4;
  if (part5.length()) path += DIRECTORY_SEPARATOR + part5;
  if (part6.length()) path += DIRECTORY_SEPARATOR + part6;
  return path;
}


// Creates a file path out of the variable list of components, relative to the server's document root.
string filter_url_create_root_path (string part1, string part2, string part3, string part4, string part5)
{
  string path = config_globals_document_root;
  if (part1.length()) path += DIRECTORY_SEPARATOR + part1;
  if (part2.length()) path += DIRECTORY_SEPARATOR + part2;
  if (part3.length()) path += DIRECTORY_SEPARATOR + part3;
  if (part4.length()) path += DIRECTORY_SEPARATOR + part4;
  if (part5.length()) path += DIRECTORY_SEPARATOR + part5;
  return path;
}


// Gets the file / url extension, e.g. /home/joe/file.txt returns "txt".
string filter_url_get_extension (string url)
{
  string extension;
  size_t pos = url.find_last_of (".");
  if (pos != string::npos) {
    extension = url.substr (pos + 1);
  }
  return extension;
}


// Returns true if the file in "url" exists.
bool filter_url_file_exists (string url)
{
  struct stat buffer;   
  return (stat (url.c_str(), &buffer) == 0);
}


// Wrapper for the mkdir function: make a directory.
// Creates parents where needed.
void filter_url_mkdir (string directory)
{
  int status;
#ifdef WIN32
	status = _mkdir(directory.c_str());
#else
  status = mkdir(directory.c_str(), 0777);
#endif
  if (status != 0) {
    vector <string> paths;
    paths.push_back (directory);
    directory = filter_url_dirname (directory);
    while (directory.length () > 2) {
      paths.push_back (directory);
      directory = filter_url_dirname (directory);
    }
    reverse (paths.begin (), paths.end ());
    for (unsigned int i = 0; i < paths.size (); i++) {
#ifdef WIN32
		_mkdir(paths[i].c_str());
#else
		mkdir(paths[i].c_str(), 0777);
#endif
    }
  }
}


// Removes directory recursively.
void filter_url_rmdir (string directory)
{
  DIR *dir;
  struct dirent *entry;
  char path[PATH_MAX];
  dir = opendir(directory.c_str());
  if (dir == NULL) return;
  while ((entry = readdir (dir)) != NULL) {
    if (strcmp (entry->d_name, ".") && strcmp (entry->d_name, "..")) {
      snprintf (path, (size_t) PATH_MAX, "%s/%s", directory.c_str(), entry->d_name);
      if (entry->d_type == DT_DIR) {
        filter_url_rmdir (path);
      }
      remove (path);
    }
  }
  closedir(dir);
  remove (directory.c_str());
}


// Returns true is $path points to a directory.
bool filter_url_is_dir (string path)
{
  struct stat sb;
  stat (path.c_str(), &sb);
  return (sb.st_mode & S_IFMT) == S_IFDIR;
}


bool filter_url_get_write_permission (string path)
{
  int result = access (path.c_str(), W_OK);
  if (result == 0) return true;
  return false;
}


void filter_url_set_write_permission (string path)
{
  chmod (path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
}


// C++ rough equivalent for PHP'sfilter_url_file_get_contents.
string filter_url_file_get_contents (string filename)
{
  if (!filter_url_file_exists (filename)) return "";
  try {
    ifstream ifs (filename.c_str(), ios::in | ios::binary | ios::ate);
    streamoff filesize = ifs.tellg();
    ifs.seekg (0, ios::beg);
    vector <char> bytes ((int)filesize);
    ifs.read (&bytes[0], (int)filesize);
    return string (&bytes[0], (int)filesize);
  } catch (...) {
    return "";
  }
}


// C++ rough equivalent for PHP'sfilter_url_file_put_contents.
void filter_url_file_put_contents (string filename, string contents)
{
  try {
    ofstream file;  
    file.open (filename, ios::binary | ios::trunc);
    file << contents;
    file.close ();
  } catch (...) {
  }
}


// C++ rough equivalent for PHP'sfilter_url_file_put_contents.
// Appends the data if the file exists.
void filter_url_file_put_contents_append (string filename, string contents)
{
  try {
    ofstream file;  
    file.open (filename, ios::binary | ios::app);  
    file << contents;
    file.close ();
  } catch (...) {
  }
}


// A C++ equivalent for PHP's filesize function.
int filter_url_filesize (string filename)
{
  struct stat stat_buf;
  int rc = stat (filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : 0;
}


// A C++ near equivalent for PHP's scandir function.
vector <string> filter_url_scandir (string folder)
{
  vector <string> files;
  DIR * dir = opendir (folder.c_str());
  if (dir) {
    struct dirent * direntry;
    while ((direntry = readdir (dir)) != NULL) {
      string name = direntry->d_name;
      if (name != "." && name != ".." && name != "gitflag" && name != ".deps" && name != ".dirstamp") {
        files.push_back (name);
      }
    }
    closedir (dir);
  }
  sort (files.begin(), files.end());
  return files;
}


// A C++ near equivalent for PHP's filemtime function.
int filter_url_filemtime (string filename)
{
  struct stat attributes;
  stat (filename.c_str(), &attributes);
  return (int) attributes.st_mtime;
}


// A C++ near equivalent for PHP's urldecode function.
string filter_url_urldecode (string url)
{
  url = UriDecode (url);
  replace (url.begin (), url.end (), '+', ' ');
  return url;
}


// Returns the name of a temporary file.
string filter_url_tempfile ()
{
  string filename = filter_url_create_root_path ("tmp", convert_to_string (filter_string_date_seconds_since_epoch ()) + convert_to_string (filter_string_date_numerical_microseconds ()));
  return filename;
}


// C++ equivalent for PHP's escapeshellarg function.
string filter_url_escape_shell_argument (string argument)
{
  argument = filter_string_str_replace ("'", "\\'", argument);
  argument.insert (0, "'");
  argument.append ("'");
  return argument;
}


// The function accepts a $path.
// The function may add a numerical suffix 
// to ensure that the $path does not yet exist in the filesystem.
string filter_url_unique_path (string path)
{
  if (!filter_url_file_exists (path)) return path;
  for (unsigned int i = 1; i < 100; i++) {
    string uniquepath = path + "." + convert_to_string (i);
    if (!filter_url_file_exists (uniquepath)) return uniquepath;
  }
  return path + "." + convert_to_string (filter_string_rand (100, 1000));
}


// Returns true if the email address is valid.
bool filter_url_email_is_valid (string email)
{
  const string valid_set ("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._-");
  // The @ character should appear only once.
  vector <string> atbits = filter_string_explode (email, '@');
  if (atbits.size() != 2) return false;
  // The characters on the left of @ should be from the valid set.
  string left = atbits [0];
  for (unsigned int i = 0; i < left.size(); i++) {
    char c = left [i];
    if (valid_set.find (c) == string::npos) return false;
  }
  // The characters on the right of @ should be from the valid set.
  string right = atbits [1];
  for (unsigned int i = 0; i < right.size(); i++) {
    char c = right [i];
    if (valid_set.find (c) == string::npos) return false;
  }
  // The character . should appear at least once to the right of @.
  vector <string> dotbits = filter_string_explode (right, '.');
  if (dotbits.size () < 2) return false;
  // The email address is valid.
  return true;
}


string filter_url_build_http_query (string url, const string& parameter, const string& value)
{
  size_t pos = url.find ("?");
  if (pos == string::npos) url.append ("?");
  else url.append ("&");
  url.append (parameter);
  url.append ("=");
  url.append (value);
  return url;
}
