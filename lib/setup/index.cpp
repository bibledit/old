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


#include <string>
#include <assets/view.h>
#include <assets/page.h>
#include <webserver/request.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <database/config/general.h>


using namespace std;


string setup_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  // Create or upgrade the quick and small databases.
  request->database_users ()->create ();

  Assets_View view = Assets_View (__FILE__);

  if (!request->post ["Submit"].empty ()) {
  
    string admin_username = request->post ["admin_username"];
    string admin_password = request->post ["admin_password"];
    string admin_email = request->post ["admin_email"];
  
    vector <string> errors;
    
    if (admin_username.length() < 5) {
      errors.push_back ("Choose a longer username.");
    }
    
    if (admin_password.length() < 7) {
      errors.push_back ("Choose a longer password.");
    }
    
    if (admin_email.length() < 5) {
      errors.push_back ("Enter a valid email address.");
    }
  
    if (errors.empty()) {
      request->database_users ()->removeUser (admin_username);
      request->database_users ()->addNewUser (admin_username, admin_password, Filter_Roles::admin (), admin_email);
      Database_Config_General::setInstalledVersion (VERSION);
      filter_url_redirect ("/index/index", request);
    } else {
      view.enable_zone ("errors");
      view.set_variable ("error", filter_string_implode (errors, " "));  
    }

  }

  return view.render ("");
}


