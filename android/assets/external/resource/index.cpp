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


#include <resource/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <sword/logic.h>
#include <demo/logic.h>
#include <resource/external.h>


string resource_index_url ()
{
  return "resource/index";
}


bool resource_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string resource_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  header.setNavigator ();
  header.setStylesheet ();
  page = header.run ();
  Assets_View view = Assets_View ();
  
  
  vector <string> resources = request->database_config_user()->getActiveResources ();
  
  
  // If no resources are displayed, set a default selection of them.
  if (resources.empty ()) {
    resources = {
      demo_sample_bible_name (),
      "Biblehub Interlinear",
      resource_external_net_bible_name (),
      "Yellow Divider",
      "Hebrew (Open Scriptures)",
      "Greek (SBL)"
    };
    request->database_config_user()->setActiveResources (resources);
  }

  
  string resourceblock;
  for (size_t i = 1; i <= resources.size (); i++) {
    resourceblock.append ("<div id=\"line" + convert_to_string (i) + "\" style=\"clear:both\">\n");
    string resource = resources[i - 1];
    if (!sword_logic_get_remote_module (resource).empty ()) {
      if (!sword_logic_get_installed_module (resource).empty ()) {
        resource = sword_logic_get_name (resource);
      }
    }
    resourceblock.append ("<span id=\"name" + convert_to_string (i) + "\" class=\"small\">" + resource + "</span>\n");
    resourceblock.append ("<span id=\"loading" + convert_to_string (i) + "\"><img src=\"/pix/loading.gif\"></span>\n");
    resourceblock.append ("<span id=\"content" + convert_to_string (i) + "\"></span>\n");
    resourceblock.append ("<hr style=\"clear:both\">");
    resourceblock.append ("</div>\n");
  }
  view.set_variable ("resourceblock", resourceblock);
  
  
  int resource_count = resources.size ();
  string script = "var resourceCount = " + convert_to_string (resource_count) + ";";
  view.set_variable ("script", script);
  
  
  page += view.render ("resource", "index");
  page += Assets_Page::footer ();
  return page;
}
