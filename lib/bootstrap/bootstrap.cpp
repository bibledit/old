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


#include <filter/url.h>
#include <index/index.h>
#include <bootstrap/bootstrap.h>
#include <webserver/http.h>
#include <session/login.h>


using namespace std;


// This function is the first function to be called when a client requests a page or file.
// Based on the request from the client, it decides what other functions to call to assemble the response.
void bootstrap_index (Webserver_Request * request)
{
  string extension = filter_url_get_extension (request->get);
  
  // Serve icons.
  if (extension  == "ico") {
    http_serve_file (request);
  }
  
  // Home page.
  else if (request->get == "/index/index") {
    request->reply = index_index (request);
  }
  
  // Login and logout.
  else if (request->get == "/session/login") {
    request->reply = session_login (request);
  }
  
  // Serve stylesheets.
  else if (extension == "css") {
    http_serve_file (request);
  }
  
  // Serve javascript.
  else if (extension == "js") {
    http_serve_file (request);
  }

  // Forward the browser to the default home page.
  else {
    filter_url_redirect ("/index/index", request);
  }
}


