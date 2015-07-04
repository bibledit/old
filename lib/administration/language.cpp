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


#include <administration/language.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <confirm/worker.h>
#include <locale/translate.h>
#include <locale/logic.h>
#include <dialog/list.h>
#include <config/logic.h>
#include <database/config/general.h>


string administration_language_url ()
{
  return "administration/language";
}


bool administration_language_acl (void * webserver_request)
{
  // Cloud: Manager can set the language.
  int role = Filter_Roles::manager ();
  // Client: Anyone can set the language.
  if (config_logic_client_prepared ()) role = Filter_Roles::member ();
  return Filter_Roles::access_control (webserver_request, role);
}


string administration_language (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;

  page = Assets_Page::header (translate ("Language"), webserver_request, "");

  Assets_View view = Assets_View ();

  map <string, string> localizations = locale_logic_localizations ();
  
  if (request->query.count ("language")) {
    string language = request->query ["language"];
    if (language == "select") {
      Dialog_List dialog_list = Dialog_List ("language", translate("Set the language for Bibledit"), "", "");
      for (auto element : localizations) {
        dialog_list.add_row (element.second, "language", element.first);
      }
      page += dialog_list.run ();
      return page;
    } else {
      Database_Config_General::setSiteLanguage (locale_logic_filter_default_language (language));
    }
  }

  string language = locale_logic_filter_default_language (Database_Config_General::getSiteLanguage ());
  language = localizations [language];
  view.set_variable ("language", language);
  
  page += view.render ("administration", "language");

  page += Assets_Page::footer ();

  return page;
}
