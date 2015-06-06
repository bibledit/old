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


string administration_language_url ()
{
  return "administration/language";
}


bool administration_language_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::admin ());
}


string administration_language (void * webserver_request)
{
  //Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;

  page = Assets_Page::header (translate ("Language"), webserver_request, "");

  Assets_View view = Assets_View ();

/*
  database_config_general = Database_Config_General::getInstance ();

  @language =  _GET['language'];
  if (isset (language)) {
    if (language == "") {
      dialog_list = new Dialog_List2 (translate("Set the default language for the site"));
      languages = Locale_Logic::getLocalizations ();
      for (languages as language) {
        dialog_list.add_row (Locale_Logic::getLanguage (language), "&language=language");
      }
      dialog_list.run ();
    } else {
      database_config_general.setSiteLanguage (Locale_Logic::filterDefault (language));
    }
  }

  view.view.language = Locale_Logic::getLanguage (database_config_general.getSiteLanguage());
*/

  view.set_variable ("language", "English");
  
  page += view.render ("administration", "language");

  page += Assets_Page::footer ();

  return page;
}
