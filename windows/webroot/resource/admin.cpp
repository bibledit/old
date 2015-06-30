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


#include <resource/admin.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <resource/logic.h>
#include <resource/external.h>


string resource_admin_url ()
{
  return "resource/admin";
}


bool resource_admin_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string resource_admin (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  page = header.run ();
  Assets_View view = Assets_View ();

  
  vector <string> resources = resource_external_names ();
  string resourceblock;
  for (auto & resource : resources) {
    resourceblock.append ("<p>");
    resourceblock.append ("<a href=\"download?name=" + resource + "\" title=\"" + translate("Download resource") + "\">" + resource + "</a>");
    resourceblock.append ("</p>\n");
  }
  view.set_variable ("resourceblock", resourceblock);

  
  page += view.render ("resource", "admin");
  page += Assets_Page::footer ();
  return page;
}
