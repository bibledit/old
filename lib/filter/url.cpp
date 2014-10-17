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
string filter_url_create_path (vector <string> components)
{
  string path;
  for (unsigned int i = 0; i < components.size(); i++) {
    if (i > 0) path += "/";
    path += components [i];
  }
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


// C++ rough equivalent for PHP's file_get_contents.
string filter_url_get_file_contents (string filename)
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
