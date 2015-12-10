/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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


#include <sync/resources.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <resource/logic.h>


string sync_resources_url ()
{
  return "sync/resources";
}


bool sync_resources_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string sync_resources (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string resource = request->query ["r"];
  int book = convert_to_int (request->query ["b"]);
  int chapter = convert_to_int (request->query ["c"]);
  int verse = convert_to_int (request->query ["v"]);
  
  if (book) {
    return resource_logic_get_contents_server2client (resource, book, chapter, verse);
  }
  
  // Bad request. Delay flood of bad requests.
  this_thread::sleep_for (chrono::seconds (1));
  request->response_code = 400;
  return "";
}
