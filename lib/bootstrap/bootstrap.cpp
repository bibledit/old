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


#include <vector>
#include <sstream>
#include <filter/url.h>
#include <index/index.h>
#include <bootstrap/bootstrap.h>


using namespace std;


// This function is the first function to be called when a client requests a page or file.
// Based on the request from the client, it decides what other functions to call to assemble the response.
void bootstrap_index (Webserver_Request * request)
{
  //  cout << request->get << endl; // Todo
  string extension = filter_url_get_extension (request->get);
  
  // /favicon.ico: Not yet implemented.
  if (request->get == "/favicon.ico") {
  }
  
  // Home page.
  else if (request->get == "/index/index") {
    request->reply = index_index ();
  }
  
  // Serve stylesheets.
  else if (extension == "css") {
    cout << request->get << endl; // Todo
  }

  // Forward the browser to the default home page.
  else {
    filter_url_redirect ("/index/index", request);
  }
}


