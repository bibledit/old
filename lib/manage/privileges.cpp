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


#include <manage/privileges.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/css.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/privileges.h>
#include <access/logic.h>
#include <menu/logic.h>
#include <manage/users.h>


string manage_privileges_url ()
{
  return "manage/privileges";
}


bool manage_privileges_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string manage_privileges (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string page;
  Assets_Header header = Assets_Header (translate("Read/write"), webserver_request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  header.addBreadCrumb (manage_users_url (), menu_logic_manage_users_text ());
  page = header.run ();
  Assets_View view;

  
  // Get the user and his/her level.
  string user = request->query["user"];
  view.set_variable ("user", user);
  int level;
  access_logic_user_level (webserver_request, user, level);

  
  bool state;

  
  // The privilege to view Resources.
  if (level < access_logic_view_resources_role ()) {
    view.enable_zone ("viewresourcesoff");
    if (request->query.count ("viewresources")) {
      state = Database_Privileges::getFeature (user, PRIVILEGE_VIEW_RESOURCES);
      Database_Privileges::setFeature (user, PRIVILEGE_VIEW_RESOURCES, !state);
    }
    state = Database_Privileges::getFeature (user, PRIVILEGE_VIEW_RESOURCES);
  } else {
    view.enable_zone ("viewresourceson");
    state = true;
  }
  view.set_variable ("viewresources", get_tick_box (state));

  
  view.set_variable ("grey", filter_css_grey_background ());
  
  page += view.render ("manage", "privileges");
  
  page += Assets_Page::footer ();
  
  return page;
}
