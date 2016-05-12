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


#include <sync/externalresources.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <config/logic.h>
#include <database/logs.h>
#include <database/offlineresources.h>
#include <client/logic.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <sync/logic.h>
#include <checksum/logic.h>
#include <access/bible.h>
#include <bible/logic.h>


string sync_externalresources_url ()
{
  return "sync/externalresources";
}


string sync_externalresources (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Sync_Logic sync_logic = Sync_Logic (webserver_request);
  Database_OfflineResources database_offlineresources;

  if (!sync_logic.security_okay ()) {
    // When the Cloud enforces https, inform the client to upgrade.
    request->response_code = 426;
    return "";
  }

  int action = convert_to_int (request->post ["a"]);
  string resource = request->post ["r"];
  string file = request->post ["f"];
  
  if (action == Sync_Logic::offlineresources_get_total_checksum) {
    return Sync_Logic::offline_resources_checksum ();
  }
  
  else if (action == Sync_Logic::offlineresources_get_resources) {
    vector <string> resources = database_offlineresources.names ();
    return filter_string_implode (resources, "\n");
  }
  
  else if (action == Sync_Logic::offlineresources_get_resource_checksum) {
    return Sync_Logic::offline_resource_checksum (resource);
  }
  
  else if (action == Sync_Logic::offlineresources_get_files) {
    vector <string> files = database_offlineresources.files (resource);
    return filter_string_implode (files, "\n");
  }
  
  else if (action == Sync_Logic::offlineresources_get_file_checksum) {
    return Sync_Logic::offline_resource_file_checksum (resource, file);
  }
  
  else if (action == Sync_Logic::offlineresources_get_file) {
    return bin2hex (database_offlineresources.load (resource, file));
  }
  
  else if (action == Sync_Logic::offlineresources_get_file_filename) {
    return database_offlineresources.httpget (filter_url_urlencode (resource), file);
  }
  
  // Bad request. Delay flood of bad requests.
  this_thread::sleep_for (chrono::seconds (1));
  request->response_code = 400;
  return "";
}

