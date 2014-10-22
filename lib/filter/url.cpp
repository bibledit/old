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
#include <vector>
#include <sstream>
#include <fstream>
#include <libgen.h>
#include <sys/stat.h>
#include <cstring>
#include <config/globals.h>
#include <stdio.h>
#include <dirent.h>
#include <algorithm>


using namespace std;


// This function redirects the browser to "path".
// "path" is an absolute value.
void filter_url_redirect (string path, Webserver_Request * request)
{
  // E.g. http or https: Always use http for just now.
  string scheme = "http";  
  
  // E.g. localhost
  string host = "localhost"; // Todo get it from the browser's headers.

  // Port
  string port = "8080"; 
  
  // A location header needs to contain an absolute url, like http://localhost/some.php.
  // See 14.30 in the specification http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html.
  string location = scheme + "://" + host + ":" + port + path;
  request->header = "Location: " + location;
  request->response_code = 302;
}


// Wraps the dirname function, see http://linux.die.net/man/3/dirname.
string filter_url_dirname (string url)
{
  char * writable = new char [url.size() + 1];
  copy (url.begin(), url.end(), writable);
  writable[url.size()] = '\0';
  string directoryname = dirname (writable);
  delete [] writable;
  return directoryname;
}


// Wraps the basename function, see http://linux.die.net/man/3/basename.
string filter_url_basename (string url)
{
  char * writable = new char [url.size() + 1];
  copy (url.begin(), url.end(), writable);
  writable[url.size()] = '\0';
  string base_name = basename (writable);
  delete [] writable;
  return base_name;
}


// Creates a file path out of the components.
string filter_url_create_path (string part1 = "", string part2 = "", string part3 = "", string part4 = "", string part5 = "")
{
  string path (part1);
  if (part2.length()) path += "/" + part2;
  if (part3.length()) path += "/" + part3;
  if (part4.length()) path += "/" + part4;
  if (part5.length()) path += "/" + part5;
  return path;
}


// Creates a file path out of the variable list of components, relative to the server's document root.
string filter_url_create_root_path (string part1 = "", string part2 = "", string part3 = "", string part4 = "", string part5 = "")
{
  string path = config_globals_document_root;
  if (part1.length()) path += "/" + part1;
  if (part2.length()) path += "/" + part2;
  if (part3.length()) path += "/" + part3;
  if (part4.length()) path += "/" + part4;
  if (part5.length()) path += "/" + part5;
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
void filter_url_mkdir (string directory)
{
  mkdir (directory.c_str(), 0777);
}


// C++ rough equivalent for PHP's file_get_contents.
string filter_url_file_get_contents (string filename)
{
  try {
    ifstream ifs (filename.c_str(), ios::in | ios::binary | ios::ate);
    ifstream::pos_type filesize = ifs.tellg();
    ifs.seekg (0, ios::beg);
    vector <char> bytes (filesize);
    ifs.read (&bytes[0], filesize);
    return string (&bytes[0], filesize);
  } catch (...) {
    return "";
  }
}


// C++ rough equivalent for PHP's file_put_contents.
void filter_url_put_file_contents (string filename, string contents)
{
  try {
    ofstream file;  
    file.open (filename, ios::binary);  
    file << contents;
    file.close ();
  } catch (...) {
  }
}


// C++ rough equivalent for PHP's file_put_contents.
// Appends the data if the file exists.
void filter_url_put_file_contents_apend (string filename, string contents)
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
      if (name != "." && name != "..") {
        files.push_back (name);
      }
    }
  }
  closedir (dir);
  sort (files.begin(), files.end());
  return files;
}
