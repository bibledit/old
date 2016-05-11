/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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
#include <sync/logic.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <resource/logic.h>


string sync_resources_url ()
{
  return "sync/resources";
}


// Serves general resource content to a client.
string sync_resources (void * webserver_request) // Todo consider security
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Sync_Logic sync_logic = Sync_Logic (webserver_request);

  // If the client's IP address very recently made a prioritized server call,
  // then delay the current call.
  // This is to give priority to the other calls from the same client:
  // Not clogging that client's internet connection.
  if (sync_logic.prioritized_ip_address_active ()) {
    this_thread::sleep_for (chrono::seconds (5));
  }

  string resource = request->query ["r"];
  int book = convert_to_int (request->query ["b"]);
  int chapter = convert_to_int (request->query ["c"]);
  int verse = convert_to_int (request->query ["v"]);
  
  bool request_ok = true;
  if (book <= 0) request_ok = false;
  if (chapter < 0) request_ok = false;
  if (chapter > 151) request_ok = false;
  if (verse < 0) request_ok = false;
  if (verse > 200) request_ok = false;
  
  if (request_ok) {
    return resource_logic_get_contents_for_client (resource, book, chapter, verse);
  }
  
  // Bad request. Delay flood of bad requests.
  this_thread::sleep_for (chrono::seconds (1));
  request->response_code = 400;
  return "";
}
