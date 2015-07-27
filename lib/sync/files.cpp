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


#include <sync/files.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <config/logic.h>
#include <database/logs.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <checksum/logic.h>


string sync_files_url ()
{
  return "sync/files";
}


bool sync_files_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string sync_files (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Sync_Logic sync_logic = Sync_Logic (webserver_request);

  int action = convert_to_int (request->post ["a"]);
  int version = convert_to_int (request->post ["v"]);
  size_t d = convert_to_int (request->post ["d"]);
  //string file = request->post ["f"];
  
  if (action == Sync_Logic::files_total_checksum) {
    return convert_to_string (Sync_Logic::files_get_total_checksum (version));
  }
  
  else if (action == Sync_Logic::files_directory_checksum) {
    vector <string> directories = Sync_Logic::files_get_directories (version);
    if (d >= directories.size ()) {
      request->response_code = 400;
      return "";
    }
    string directory = directories [d];
    int checksum = Sync_Logic::files_get_directory_checksum (directory);
    return convert_to_string (checksum);
  }

  else if (action == Sync_Logic::files_directory_files) {
    vector <string> directories = Sync_Logic::files_get_directories (version);
    if (d >= directories.size ()) {
      request->response_code = 400;
      return "";
    }
    string directory = directories [d];
    vector <string> paths = Sync_Logic::files_get_files (directory);
    return filter_string_implode (paths, "\n");
  }
  
  // Bad request. Delay flood of bad requests.
  this_thread::sleep_for (chrono::seconds (1));
  request->response_code = 400;
  return "";
}

