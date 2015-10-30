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


#include <public/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <locale/logic.h>
#include <access/bible.h>
#include <dialog/list.h>
#include <public/login.h>


string public_index_url ()
{
  return "public/index";
}


bool public_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string public_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;


  // If the person providing public feedback is not known, foward to the page for entering details.
  if (!request->session_logic ()->loggedIn ()) {
    redirect_browser (request, public_login_url ());
    return "";
  }

  
  string page;
  Assets_Header header = Assets_Header (translate ("Public feedback"), request);
  page = header.run ();
  Assets_View view = Assets_View ();
  
  
  if (request->query.count ("previouspublic")) {
  }
  
  
  string bible = access_bible_clamp (webserver_request, request->database_config_user()->getBible ());
  
  
  if (request->post.count ("add")) {
  }
  
  
  if (request->query.count ("bible")) {
    bible = request->query ["bible"];
    if (bible == "") {
      Dialog_List dialog_list = Dialog_List ("index", translate("Select which Bible to display"), "", "");
      vector <string> bibles = access_bible_bibles (request);
      for (auto & bible : bibles) {
        dialog_list.add_row (bible, "bible", bible);
      }
      page += dialog_list.run();
      return page;
    } else {
      request->database_config_user()->setBible (bible);
    }
  }
  
  
  bible = access_bible_clamp (webserver_request, request->database_config_user()->getBible ());
  
  
  view.set_variable ("bible", bible);

  
  page += view.render ("public", "index");
  page += Assets_Page::footer ();
  return page;
}
