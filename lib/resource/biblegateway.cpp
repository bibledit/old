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


#include <resource/biblegateway.h>
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
#include <menu/logic.h>
#include <pugixml/pugixml.hpp>


using namespace pugi;


string resource_biblegateway_url ()
{
  return "resource/biblegateway";
}


bool resource_biblegateway_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string resource_biblegateway (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  page = header.run ();
  Assets_View view;

  
  // The path to the list of BibleGateway resources.
  // It is stored in the client files area.
  // Clients will download it from there.
  string path = resource_logic_bible_gateway_module_list_path ();
  
  
  if (request->query.count ("refresh")) {
    string error;
    string html = filter_url_http_get ("https://www.biblegateway.com/versions/", error, false);
    if (error.empty ()) {
      vector <string> resources;
      html = filter_text_html_get_element (html, "select");
      xml_document document;
      document.load_string (html.c_str());
      xml_node select_node = document.first_child ();
      for (xml_node option_node : select_node.children()) {
        string cls = option_node.attribute ("class").value ();
        if (cls == "lang") continue;
        if (cls == "spacer") continue;
        string name = option_node.text ().get ();
        resources.push_back (name);
      }
      view.set_variable ("success", translate ("The list was updated"));
      filter_url_file_put_contents (path, filter_string_implode (resources, "\n"));
    }
    view.set_variable ("error", error);
  }
  
  
  string moduleblock = filter_url_file_get_contents (path);
  vector <string> lines = filter_string_explode (moduleblock, '\n');
  moduleblock.clear ();
  for (auto line : lines) {
    moduleblock.append ("<p>");
    moduleblock.append (line);
    moduleblock.append ("</p>\n");
  }
  view.set_variable ("moduleblock", moduleblock);

  
  page += view.render ("resource", "biblegateway");
  page += Assets_Page::footer ();
  return page;
}
