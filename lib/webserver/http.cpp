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
#include <filter/url.h>


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
void http_get_header_get (string headers, Webserver_Request * request)
{
  // Default page to get.
  request->get = "/index";
  
  // Split header on spaces, and extract the bit after the "GET".
  istringstream issget (headers);
  string s;
  bool bingo = false;
  while (getline (issget, s, ' ')) 
  {
    if (bingo) {
      request->get = s;
      bingo = false;
    }
    if (s == "GET") bingo = true;
  }
  
  // The GET value may be, for example: stylesheet.css?1.0.1.
  // Split it up into two parts: The part before the ?, and the part after the ?.
  istringstream issquery (request->get);
  int counter = 0;
  while (getline (issquery, s, '?')) {
    if (counter == 0) request->get = s;
    if (counter == 1) request->query = s;
    counter++;
  }
}


/*

The function assembles the response to be given to the browser.
code: an integer response code, normally this is 200.
header: An extra header to be sent with the response. May be empty.
contents: the response body to be sent.

The function inserts the correct headers,
and creates the entire result to be sent back to the browser.

*/
void http_assemble_response (Webserver_Request * request)
{
  ostringstream length;
  length << request->reply.size ();

  // Assemble the HTTP response code fragment.
  string http_response_code_fragment;
  if (request->response_code == 200) http_response_code_fragment = "200 OK";
  if (request->response_code == 302) http_response_code_fragment = "302 Found";

  // Assemble the Content-Type. Todo
  string extension = filter_url_get_extension (request->get);
  string content_type;
  if (extension == "js") content_type = "application/javascript";
  else if (extension == "css") content_type = "text/css"; // Todo this requires separation of the bits after the ? in the GET request: Update the request object for that.
  else content_type = "text/html";

  // Assemble the complete response for the browser.
  vector <string> response;
  response.push_back ("HTTP/1.1 " + http_response_code_fragment);
  response.push_back ("Content-Length: " + length.str());
  response.push_back ("Content-Type: " + content_type);
  if (!request->header.empty ()) response.push_back (request->header);
  response.push_back ("");
  if (!request->reply.empty ()) response.push_back (request->reply);

  string assembly;
  for (unsigned int i = 0; i < response.size (); i++) {
    if (i > 0) assembly += "\n";
    assembly += response [i];
  }
  request->reply = assembly;
}
