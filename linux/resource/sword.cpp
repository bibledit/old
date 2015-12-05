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


#include <resource/sword.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/shell.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <resource/logic.h>
#include <tasks/logic.h>
#include <journal/index.h>
#include <sword/logic.h>
#include <config/logic.h>
#include <menu/logic.h>


string resource_sword_url ()
{
  return "resource/sword";
}


bool resource_sword_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string resource_sword (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  if (request->query.count ("refresh")) {
    tasks_logic_queue (REFRESHSWORDMODULES);
    redirect_browser (request, journal_index_url ());
  }

  
  string source = request->query ["source"];
  string module = request->query ["module"];

  
  if (request->query.count ("install")) {
    tasks_logic_queue (INSTALLSWORDMODULE, {source, module});
    redirect_browser (request, journal_index_url ());
  }
  
  
  if (request->query.count ("update")) {
    tasks_logic_queue (UPDATESWORDMODULE, {source, module});
    redirect_browser (request, journal_index_url ());
  }
  
  
  if (request->query.count ("updateall")) {
    tasks_logic_queue (UPDATEALLSWORDMODULES, {});
    redirect_browser (request, journal_index_url ());
  }
  
  
  if (request->query.count ("uninstall")) {
    tasks_logic_queue (UNINSTALLSWORDMODULE, {module});
    redirect_browser (request, journal_index_url ());
  }

  
  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  page = header.run ();
  Assets_View view;

  
  map <string, string> installed_modules;
  {
    vector <string> modules = sword_logic_get_installed ();
    for (auto module : modules) {
      string name = sword_logic_get_installed_module (module);
      string version = sword_logic_get_version (module);
      installed_modules [name] = version;
    }
  }
  
  vector <string> available_modules = sword_logic_get_available ();
  string moduleblock;
  for (auto & available_module : available_modules) {
    string source = sword_logic_get_source (available_module);
    string module = sword_logic_get_remote_module (available_module);
    moduleblock.append ("<p>");
    string source_module = "&source=" + source + "&module=" + module;
    if (installed_modules [module].empty ()) {
      moduleblock.append ("<a href=\"?install=" + source_module + "\">" + translate ("install") + "</a>");
      moduleblock.append (" | ");
    } else {
      string version = sword_logic_get_version (available_module);
      if (version != installed_modules[module]) {
        moduleblock.append ("<a href=\"?update=" + source_module + "\">" + translate ("update") + "</a>");
        moduleblock.append (" | ");
      }
      moduleblock.append ("<a href=\"?uninstall=" + source_module + "\">" + translate ("uninstall") + "</a>");
      moduleblock.append (" | ");
    }
    moduleblock.append (available_module);
    moduleblock.append ("</p>\n");
  }
  view.set_variable ("moduleblock", moduleblock);

  
  if (config_logic_client_prepared ()) {
    view.enable_zone ("client");
  } else {
    view.enable_zone ("server");
  }
  
  
  page += view.render ("resource", "sword");
  page += Assets_Page::footer ();
  return page;
}
