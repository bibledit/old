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


#include <sync/setup.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <database/config/general.h>
#include <database/users.h>


string sync_setup_url ()
{
  return "sync/setup";
}


bool sync_setup_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string sync_setup (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  
  string username = request->query ["user"];
  username = hex2bin (username);
  string password = request->query ["pass"];

  if (request->database_users ()->usernameExists (username)) {
    string md5 = request->database_users ()->getmd5 (username);
    if (password == md5) {
      // The credentials of the client have been accepted.
      // Return the level to the client.
      return convert_to_string (request->database_users ()->getUserLevel (username));
    }
  }
  
  
  // The credentials were not accepted.
  this_thread::sleep_for (chrono::seconds (1));
  return "Server does not recognize the credentials";
}
