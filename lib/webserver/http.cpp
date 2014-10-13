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


using namespace std;



/*
The http headers from a browser could look as follows:
 
GET /index/page HTTP/1.1
Host: localhost:8080
Connection: keep-alive
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.101 Safari/537.36
Accept-Language: sn,en-US;q=0.8,en;q=0.6

The function extracts the page request:

/index/page

*/
string http_get_header_get (string headers)
{
  string get = "/index";
  istringstream iss(headers);
  string s;
  bool bingo = false;
  while (getline (iss, s, ' ')) 
  {
    if (bingo) {
      get = s;
      bingo = false;
    }
    if (s == "GET") bingo = true;
  }
  return get;
}


/*

The function assembles the response to be given to the browser.
code: an integer response code, normally this is 200.
header: An extra header to be sent with the response. May be empty.
contents: the response body to be sent.

The function inserts the correct headers,
and returns the entire result to be sent back to the browser.

*/
string http_assemble_response (int code, string header, string contents)
{
  ostringstream length;
  length << contents.size ();

  // Assemble the http response code fragment.
  string http_response_code_fragment;
  if (code == 0) {
    // Default.
    http_response_code_fragment = "200 OK";
    
    if (header.find ("Location") != string::npos) {
      http_response_code_fragment = "302 Found";
    }
  }

  vector <string> response;
  response.push_back ("HTTP/1.1 " + http_response_code_fragment);
  response.push_back ("Content-Length: " + length.str());
  response.push_back ("Content-Type: text/html;charset=UTF-8");
  if (!header.empty ()) response.push_back (header);
  response.push_back ("");
  response.push_back (contents);

  string assembly;
  for (unsigned int i = 0; i < response.size (); i++) {
    if (i > 0) assembly += "\n";
    assembly += response [i];
  }
  return assembly;
}
