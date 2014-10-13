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
#include <libgen.h>


using namespace std;



// This function redirects the browser to "path".
// "path" is an absolute value.
string filter_url_redirect (string path) // Todo porting this.
{
  // E.g. http or https: Always use http for just now.
  string scheme = "http";  
  
  // E.g. localhost
  string host = "localhost";

  // Port
  string port = "8080"; 
  
  // A location header needs to contain an absolute url, like http://localhost/some.php.
  // See 14.30 in the specification http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html.
  string location = scheme + "://" + host + ":" + port + path;
  return "Location: " + location;
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
