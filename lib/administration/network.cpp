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


#include <administration/network.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <confirm/worker.h>
#include <database/config/general.h>
#include <locale/translate.h>
#include <assets/header.h>
#include <menu/logic.h>


string administration_network_url ()
{
  return "administration/network";
}


bool administration_network_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::admin ());
}


string administration_network (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  Assets_Header header = Assets_Header (translate("network"), webserver_request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  page = header.run ();

  Assets_View view;

  string success;
  string error;

  if (request->post.count ("url")) {
    string url = request->post ["url"];
    Database_Config_General::setManualUserFacingUrl (url);
  }

  string url = Database_Config_General::getManualUserFacingUrl ();
  view.set_variable ("url", url);
  
  string siteurl = Database_Config_General::getSiteURL ();
  view.set_variable ("siteurl", siteurl);

  view.set_variable ("success", success);
  view.set_variable ("error", error);
  
  page += view.render ("administration", "network");

  page += Assets_Page::footer ();

  return page;
}
