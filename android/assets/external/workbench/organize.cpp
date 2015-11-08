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


#include <workbench/organize.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>
#include <workbench/logic.h>
#include <dialog/yes.h>


string workbench_organize_url ()
{
  return "workbench/organize";
}


bool workbench_organize_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string workbench_organize (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  if (request->post.count ("add")) {
    string add = request->post["add"];
    request->database_config_user()->setActiveWorkbench (add);
    workbenchSetURLs    (request, workbenchDefaultURLs (0));
    workbenchSetWidths  (request, workbenchDefaultWidths (0));
    workbenchSetHeights (request, workbenchDefaultHeights (0));
  }
  
  
  if (request->post.count ("workbenches")) {
    string s_workbenches = request->post ["workbenches"];
    vector <string> workbenches = filter_string_explode (s_workbenches, ',');
    workbenchOrderWorkbenches (request, workbenches);
    return "";
  }
  
  
  string page;
  
  
  Assets_Header header = Assets_Header (translate("Workbenches"), request);
  header.jQueryUIOn ();
  page = header.run ();
  
  
  if (request->query.count ("remove")) {
    string remove = request->query["remove"];
    string confirm = request->query["confirm"];
    if (confirm.empty ()) {
      Dialog_Yes dialog_yes = Dialog_Yes ("organize", translate("Would you like to delete this workbench configuration?"));
      dialog_yes.add_query ("remove", remove);
      page += dialog_yes.run ();
      return page;
    }
    if (confirm == "yes") {
      workbenchDeleteWorkbench (request, remove);
    }
  }
  
  
  Assets_View view;
  
  
  vector <string> workbenchblock;
  vector <string> workbenches = workbenchGetWorkbenches (request);
  for (auto & workbench : workbenches) {
    workbenchblock.push_back ("<p>");
    workbenchblock.push_back ("<a href=\"?remove=" + workbench + "\" title=\"" + translate("Delete workbench") + "\"> ✗ </a>");
    workbenchblock.push_back ("|");
    workbenchblock.push_back ("<a href=\"settings?name=" + workbench + "\" title=\"" + translate("Edit workbench") + "\"> ✎ </a>");
    workbenchblock.push_back ("|");
    workbenchblock.push_back ("<span class=\"drag\">" + workbench + "</span>");
    workbenchblock.push_back ("</p>");
  }
  view.set_variable ("workbenchblock", filter_string_implode (workbenchblock, "\n"));

  
  page += view.render ("workbench", "organize");
  
  
  page += Assets_Page::footer ();
  
  
  return page;
}
