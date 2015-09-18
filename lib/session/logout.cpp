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


#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <session/login.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <filter/roles.h>


const char * session_logout_url ()
{
  return "session/logout";
}


bool session_logout_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string session_logout (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  request->session_logic ()->logout ();
  string page;
  page += Assets_Page::header (translate ("Logout"), webserver_request);
  Assets_View view = Assets_View ();
  page += view.render ("session", "logout");
  page += Assets_Page::footer ();
  return page;
}


