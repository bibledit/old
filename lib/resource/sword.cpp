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
#include <database/logs.h>


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

  
  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  page = header.run ();
  Assets_View view = Assets_View ();

  
  vector <string> resources;
  string resourceblock;
  for (auto & resource : resources) {
    resourceblock.append ("<p>");
    resourceblock.append ("<a href=\"download?name=" + resource + "\" title=\"" + translate("Download resource") + "\">" + resource + "</a>");
    resourceblock.append ("</p>\n");
  }
  view.set_variable ("resourceblock", resourceblock);

  
  page += view.render ("resource", "sword");
  page += Assets_Page::footer ();
  return page;
}


void resource_sword_refresh_module_list () // Todo
{
  Database_Logs::log ("Refreshing SWORD module list");

  string out_err;
  vector <string> lines;
  
  // Initialize basic user configuration
  filter_shell_run ("echo yes | installmgr -init", out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    Database_Logs::log (line);
  }
  
  // Sync the configuration with the online known remote repository list.
  filter_shell_run ("echo yes | installmgr -sc", out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    Database_Logs::log (line);
  }
  
  // List the remote sources.
  vector <string> remote_sources;
  filter_shell_run ("installmgr -s", out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    Database_Logs::log (line);
    line = filter_string_trim (line);
    if (line.find ("[") != string::npos) {
      line.erase (0, 1);
      if (line.find ("]") != string::npos) {
        line.erase (line.length () - 1, 1);
        remote_sources.push_back (line);
      }
    }
  }
  
  vector <string> sword_modules;
  
  for (auto remote_source : remote_sources) {
    
    Database_Logs::log ("Reading modules from remote resource: " + remote_source);
    
    filter_shell_run ("echo yes | installmgr -r \"" + remote_source + "\"", out_err);
    filter_string_replace_between (out_err, "WARNING", "type yes at the prompt", "");
    Database_Logs::log (out_err);

    filter_shell_run ("installmgr -rl \"" + remote_source + "\"", out_err);
    lines = filter_string_explode (out_err, '\n');
    for (auto line : lines) {
      line = filter_string_trim (line);
      if (line.empty ()) continue;
      Database_Logs::log (line);
      if (line.find ("[") == string::npos) continue;
      if (line.find ("]") == string::npos) continue;
      sword_modules.push_back ("[" + remote_source + "]" + " " + line);
    }
    
  }
  
  // Store the list of remote sources and their modules.
  // It is stored in the client files area.
  // Clients can access it from there too.
  string path = resource_sword_module_list_path ();
  filter_url_file_put_contents (path, filter_string_implode (sword_modules, "\n"));
  
  Database_Logs::log ("Ready refreshing SWORD module list");
}


string resource_sword_module_list_path ()
{
  return filter_url_create_root_path ("databases", "client", "sword_modules.txt");
}


