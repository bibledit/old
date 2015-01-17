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


#include <workbench/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>


string workbench_index_url ()
{
  return "workbench/index";
}


bool workbench_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string workbench_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  

  if (request->query.count ("bench")) {
    string bench = request->query ["bench"];
    /* Todo
    $workbenches = Workbench_Logic::getWorkbenches ();
    $workbench = $workbenches [$bench];
    request->database_config_user()->setActiveWorkbench ($workbench);
    */
  }
  
  
  string page;

  
  Assets_Header header = Assets_Header (gettext("Workbench"), request);
  header.setNavigator ();
  page = header.run ();
  
  
  Assets_View view = Assets_View ();
  

  /* Todo
  $urls = Workbench_Logic::getURLs (true);
  $widths = Workbench_Logic::getWidths ();
  for ($urls as $key => $url) {
    $row = intval ($key / 5) + 1;
    $column = $key % 5 + 1;
    $variable = "url" . $row . $column;
    $view->view->$variable = $url;
    $variable = "width" . $row . $column;
    $view->view->$variable = $widths [$key];
  }
  
  
  $heights = Workbench_Logic::getHeights ();
  for ($heights as $key => $height) {
    $row = $key + 1;
    $variable = "height" . $row;
    $view->view->$variable = $height;
  }
   */
  
  
  page += view.render ("workbench", "index");
  
  
  page += Assets_Page::footer ();
  
  
  return page;
}
