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


#include <resource/organize.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>


string resource_organize_url ()
{
  return "resource/organize";
}


bool resource_organize_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string resource_organize (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  /* Todo
   
   
   
  @$add = request->query['add'];
  if (isset ($add)) {
    $resources = request->database_config_user()->getActiveResources ();
    $resources [] = $add;
    request->database_config_user()->setActiveResources ($resources);
  }
  
  
  @$remove = request->query['remove'];
  if (isset ($remove)) {
    $resources = request->database_config_user()->getActiveResources ();
    $key = array_search ($remove, $resources);
    unset ($resources[$key]);
    request->database_config_user()->setActiveResources ($resources);
  }
  
  
  @$resources = request->post ['resources'];
  if (isset ($resources)) {
    $resources = explode (",", $resources);
    request->database_config_user()->setActiveResources ($resources);
  }
  
  

  */
  
  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  header.jQueryUIOn ("sortable");
  page = header.run ();
  Assets_View view = Assets_View ();

  // Active resources.
  vector <string> active_resources = request->database_config_user()->getActiveResources ();
  string activesblock;
  for (auto& active : active_resources) {
    activesblock.append ("<p><a href=\"?remove=" + active + "\"> ✗ </a>" + active + "</p>\n");
  }
  view.set_variable ("activesblock", activesblock);
  
  // The selectable resources are the available ones minus the active ones.
  /* Todo
  $available_resources = Resource_Logic::getNames ();
  $selectable_resources = filter_string_array_diff ($available_resources, $active_resources);
  $view.set_variable ("selectables = $selectable_resources;
*/
   
  page += view.render ("resource", "organize");
  page += Assets_Page::footer ();
  return page;
}
