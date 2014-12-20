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


#include <styles/indext.h>
#include <assets/view.h>
#include <assets/page.h>
#include <dialog/entry.h>
#include <dialog/list.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <tasks/logic.h>
#include <webserver/request.h>
#include <journal/index.h>
#include <database/config/user.h>
#include <database/logs.h>
#include <access/user.h>
#include <locale/translate.h>


string styles_indext_url ()
{
  return "styles/indext";
}


bool styles_indext_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string styles_indext (void * webserver_request) // Todo
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  page = Assets_Page::header (gettext ("Styles"), webserver_request, "");

  Assets_View view = Assets_View ();

  Database_Config_User database_config_user = Database_Config_User (webserver_request);
   
  if (request->query.count ("sheet")) {
  string sheet = request->query["sheet"];
    
    /* Todo  if (sheet == "") {
   $dialog_list = new Dialog_List2 (gettext("Would you like to switch to another stylesheet?"));
   $database_styles = Database_Styles::getInstance();
   $sheets = $database_styles->getSheets();
   foreach ($sheets as $sheet) {
   $parameter = "&sheet=$sheet";
   $dialog_list->add_row ($sheet, $parameter);
   }
   $dialog_list->run ();
   } else {
   $database_config_user->setStylesheet ($sheet);
   }
   }*/
   
  string stylesheet = database_config_user.getStylesheet();
  view.set_variable ("stylesheet",  = Filter_Html::sanitize (stylesheet);
  
  page += view.render ("styles", "indext");

  page += Assets_Page::footer ();

  return page;
}
