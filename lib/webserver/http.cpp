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
#include <config/globals.h>
#include <filter/string.h>
#include <parsewebdata/ParseWebData.h>


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
void http_parse_headers (string headers, Webserver_Request * request)
{
  // Defaults for the headers.
  request->get = "/index";
  request->user_agent = "Browser/1.0";
  request->accept_language = "en-US";

  // Flags.
  bool post_request = false;
  bool reading_post_data = false;
  
  // Query data.
  string query_data;
    
  // Split the headers up into separate lines and process them.
  vector <string> lines = filter_string_explode (headers, '\n');
  for (unsigned int i = 0; i < lines.size (); i++) {
    string line = filter_string_trim (lines [i]);

    // Deal with a header like this: GET /css/stylesheet.css?1.0.1 HTTP/1.1
    // Or like this: POST /session/login?request= HTTP/1.1
    bool get = false;
    if (line.substr (0, 3) == "GET") get = true;
    if (line.substr (0, 4) == "POST") {
      get = true;
      post_request = true;
    }
    if (get) {
      vector <string> get = filter_string_explode (line, ' ');
      if (get.size () >= 2) {
        request->get = get [1];
        // The GET or POST value may be, for example: stylesheet.css?1.0.1.
        // Split it up into two parts: The part before the ?, and the part after the ?.
        istringstream issquery (request->get);
        int counter = 0;
        string s;
        while (getline (issquery, s, '?')) {
          if (counter == 0) request->get = s;
          if (counter == 1) query_data = s;
          counter++;
        }
      }
    }
    
    // Extract the User-Agent from a header like this:
    // User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.104 Safari/537.36
    if (line.substr (0, 10) == "User-Agent") {
      request->user_agent = line.substr (12);
    }
    
    // Extract the Accept-Language from a header like this:
    // Accept-Language: sn,en-US;q=0.8,en;q=0.6
    if (line.substr (0, 15) == "Accept-Language") {
      request->accept_language = line.substr (17);
    }
    
    // Extract the Host from a header like this:
    // Host: 192.168.1.139:8080
    if (line.substr (0, 4) == "Host") {
      request->host = line.substr (6);
      vector <string> bits = filter_string_explode (request->host, ':');
      if (!bits.empty ()) request->host = bits [0];
    }
    
    // Extract the Content-Type from a header like this:
    // Content-Type: application/x-www-form-urlencoded
    if (line.substr (0, 12) == "Content-Type") {
      request->content_type = line.substr (14);
    }
    
    // Read and parse the POST data.
    if (reading_post_data && post_request) {
   	  ParseWebData::WebDataMap dataMap;
      ParseWebData::parse_post_data (line, request->content_type, dataMap);
      for (ParseWebData::WebDataMap::const_iterator iter = dataMap.begin(); iter != dataMap.end(); ++iter) {
        request->post [(*iter).first] = filter_url_urldecode ((*iter).second.value);
      }
    }
    
    // The POST data comes after a blank line.
    if (line == "") reading_post_data = true;
  }

  // Read and parse the GET data.
  if (!query_data.empty ()) {
 	  ParseWebData::WebDataMap dataMap;
    ParseWebData::parse_get_data (query_data, dataMap);
    for (ParseWebData::WebDataMap::const_iterator iter = dataMap.begin(); iter != dataMap.end(); ++iter) {
      request->query [(*iter).first] = filter_url_urldecode ((*iter).second.value);
    }
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

  // Assemble the Content-Type.
  string extension = filter_url_get_extension (request->get);
  string content_type;
  if (extension == "js") content_type = "application/javascript";
  else if (extension == "css") content_type = "text/css";
  else if (extension == "ico") content_type = "image/vnd.microsoft.icon";
  else content_type = "text/html";

  // Assemble the complete response for the browser.
  vector <string> response;
  response.push_back ("HTTP/1.1 " + http_response_code_fragment);
  response.push_back ("Accept-Ranges: bytes");
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


// This function serves a file.
void http_serve_file (Webserver_Request * request)
{
  // Full path to the file.
  string filename = filter_url_create_root_path (request->get);
  
  // Get file's contents.
  request->reply = filter_url_file_get_contents (filename);
}
