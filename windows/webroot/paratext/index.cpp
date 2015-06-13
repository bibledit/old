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


#include <paratext/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <dialog/entry.h>
#include <dialog/list.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <paratext/logic.h>
#include <database/config/bible.h>
#include <database/config/general.h>


string paratext_index_url ()
{
  return "paratext/index";
}


bool paratext_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string paratext_index (void * webserver_request) // Todo write the page.
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  

  string page;
  page = Assets_Page::header (translate ("Paratext"), webserver_request, "");
  Assets_View view = Assets_View ();
  string success;
  string error;

  
  string bible = request->query ["bible"];
  if (request->query.count ("select")) {
    string select = request->query["select"];
    if (select == "") {
      Dialog_List dialog_list = Dialog_List ("index", translate("Which Bible are you going to use?"), "", "");
      dialog_list.add_query ("bible", bible);
      vector <string> bibles = request->database_bibles()->getBibles();
      for (auto & value : bibles) {
        dialog_list.add_row (value, "select", value);
      }
      page += dialog_list.run ();
      return page;
    } else {
      bible = select;
    }
  }
  
  
  if (request->query.count ("disable")) {
    bible.clear ();
    // Todo remove relevant data
  }

  
  view.set_variable ("bible", bible);
  if (bible.empty ()) view.enable_zone ("bibleinactive");
  else view.enable_zone ("bibleactive");
  
  

  
  // Paratext Projects folder.
  string paratext_folder = Database_Config_General::getParatextProjectsFolder ();
  if (!file_exists (paratext_folder)) paratext_folder.clear ();
  if (paratext_folder.empty ()) paratext_folder = Paratext_Logic::searchParatextProjectsFolder ();
  Database_Config_General::setParatextProjectsFolder (paratext_folder);
  view.set_variable ("paratextfolder", paratext_folder);
  if (paratext_folder.empty ()) {
    view.set_variable ("paratextfolder", translate ("not found"));
  }

  

  

  view.set_variable ("success", success);
  view.set_variable ("error", error);
  
  page += view.render ("paratext", "index");
  
  page += Assets_Page::footer ();
  
  return page;
}
