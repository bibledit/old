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


#include <bible/order.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <database/config/bible.h>
#include <database/books.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <menu/logic.h>
#include <bible/manage.h>


string bible_order_url ()
{
  return "bible/order";
}


bool bible_order_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string bible_order (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;

  Assets_Header header = Assets_Header (translate("Order"), request);
  header.jQueryUIOn (); // Todo
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  header.addBreadCrumb (bible_manage_url (), menu_logic_bible_manage_text ());
  page = header.run ();
  
  Assets_View view;
  
  string success_message;
  string error_message;
  
  // The name of the Bible.
  string bible = access_bible_clamp (request, request->query ["bible"]);
  view.set_variable ("bible", filter_string_sanitize_html (bible));

  if (request->query.count ("reset")) {
    Database_Config_Bible::setBookOrder (bible, "");
  }

  if (request->post.count ("order")) {
    string order = request->post ["order"];
    vector <string> v_order = filter_string_explode (order, ',');
    vector <string> ids;
    for (auto english : v_order) {
      int id = Database_Books::getIdFromEnglish (english);
      ids.push_back (convert_to_string (id));
    }
    order = filter_string_implode (ids, " ");
    Database_Config_Bible::setBookOrder (bible, order);
    return "";
  }
  
  string namesblock;
  vector <int> books = filter_passage_get_ordered_books (bible);
  for (auto book : books) {
    string name = Database_Books::getEnglishFromId (book);
    namesblock.append ("<p class=\"ui-state-default\"> ⇕ <span class=\"drag\">" + name + "</span> ⇕ </p>\n");
  }
  view.set_variable ("namesblock", namesblock);
  
  string script = "var orderBible = '" + bible + "'";
  view.set_variable ("script", script);

  view.set_variable ("success_message", success_message);
  view.set_variable ("error_message", error_message);
  
  page += view.render ("bible", "order");
  
  page += Assets_Page::footer ();
  
  return page;
}
