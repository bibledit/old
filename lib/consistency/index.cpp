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


#include <consistency/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>
#include <database/usfmresources.h>
#include <client/logic.h>
#include <demo/logic.h>
#include <sendreceive/logic.h>
#include <config/logic.h>
#include <dialog/list.h>
#include <resource/logic.h>


string consistency_index_url ()
{
  return "consistency/index";
}


bool consistency_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string consistency_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string page;
  page = Assets_Page::header (translate ("Consistency"), webserver_request);
  Assets_View view;

  
  if (request->query.count ("add")) {
    string add = request->query ["add"];
    if (add == "") {
      Dialog_List dialog_list = Dialog_List ("index", translate("Would you like to add a Resource?"), "", "");
      vector <string> resources = resource_logic_get_names (webserver_request);
      for (auto resource : resources) {
        dialog_list.add_row (resource, "add", resource);
      }
      page += dialog_list.run();
      return page;
    } else {
      vector <string> resources = request->database_config_user()->getConsistencyResources ();
      resources.push_back (add);
      request->database_config_user()->setConsistencyResources (resources);
    }
  }
  
  
  if (request->query.count ("remove")) {
    string remove = request->query ["remove"];
    vector <string> resources = request->database_config_user()->getConsistencyResources ();
    resources = filter_string_array_diff (resources, {remove});
    request->database_config_user()->setConsistencyResources (resources);
  }
  
  
  string resourceblock;
  vector <string> resources = request->database_config_user()->getConsistencyResources ();
  for (auto resource : resources) {
    resourceblock.append (resource);
    resourceblock.append ("\n");
    resourceblock.append ("<a href=\"?remove=" + resource + "\">[" + translate("remove") + "]</a>");
    resourceblock.append (" | ");
  }
  view.set_variable ("resourceblock", resourceblock);

  
  page += view.render ("consistency", "index");
  page += Assets_Page::footer ();
  return page;
}
