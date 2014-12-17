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
#include <session/logout.h>
#include <session/password.h>
#include <session/signup.h>
#include <database/config/general.h>
#include <setup/index.h>
#include <journal/index.h>
#include <config/logic.h>
#include <help/index.h>
#include <email/index.h>
#include <user/notifications.h>
#include <user/account.h>


// This function is the first function to be called when a client requests a page or file.
// Based on the request from the client, it decides what other functions to call to assemble the response.
void bootstrap_index (Webserver_Request * request)
{
  string extension = filter_url_get_extension (request->get);
  string url = request->get.substr (1);
  
  // Serve graphics, stylesheets, JavaScript.
  if ((extension  == "ico") || (extension  == "png") || (extension == "css") || (extension == "js")) http_serve_file (request);
  
  // Force setup.
  else if (config_logic_version () != Database_Config_General::getInstalledVersion ()) request->reply = setup_index (request);

  // Home page.
  else if ((url == index_index_url ()) && index_index_acl (request)) request->reply = index_index (request);
  
  // Login and logout.
  else if ((url == session_login_url ()) && session_login_acl (request)) request->reply = session_login (request);
  else if ((url == session_logout_url ()) && session_logout_acl (request)) request->reply = session_logout (request);
  else if ((url == session_password_url ()) && session_password_acl (request)) request->reply = session_password (request);
  else if ((url == session_signup_url ()) && session_signup_acl (request)) request->reply = session_signup (request);
  
  // Changes menu.
  else if ((url == journal_index_url ()) && journal_index_acl (request)) request->reply = journal_index (request);

  // Settings menu.
  else if ((url == email_index_url ()) && email_index_acl (request)) request->reply = email_index (request);
  
  // Help menu.
  else if ((help_index_url (url)) && help_index_acl (request, url)) request->reply = help_index (request, url);

  // User menu.  
  else if ((url == user_notifications_url ()) && user_notifications_acl (request)) request->reply = user_notifications (request);
  else if ((url == user_account_url ()) && user_account_acl (request)) request->reply = user_account (request);

  // Forward the browser to the default home page.
  else {
    redirect_browser ("/index/index", request);
  }
}


