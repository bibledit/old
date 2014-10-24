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


#pragma once


#include <cstdlib>
#include <string>


using namespace std;


class Webserver_Request
{
public:
  Webserver_Request ();
  ~Webserver_Request ();
  string remote_address; // The browser's or client's remote IPv4 address.
  string get; // The page the browser requests via GET.
  string query; // The query from the browser, e.g. foo=bar&baz=qux
  string user_agent; // The browser's user agent, e.g. Mozilla/x.0 (X11; Linux) ...
  string accept_language; // The browser's or client's Accept-Language header.
  string host; // The server's host as requested by the client.
  string header; // Extra header to be sent back to the browser.
  string reply; // Body to be sent back to the browser.
  int response_code; // Response code to be sent to the browser.
private:
};


