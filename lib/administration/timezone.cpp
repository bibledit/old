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


#include <administration/timezone.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <confirm/worker.h>
#include <database/config/general.h>
#include <locale/translate.h>


string administration_timezone_url ()
{
  return "administration/timezone";
}


bool administration_timezone_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::admin ());
}


string administration_timezone (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  page = Assets_Page::header (gettext ("Timezone"), webserver_request, "");

  Assets_View view = Assets_View ();

  string success;
  string error;
  if (request->post.count ("submit")) {
    string input = request->post ["timezone"];
    input = filter_string_str_replace ("UTC", "", input);
    int timezone = convert_to_int (input);
    bool clipped = false;
    if (timezone < -12) {
      timezone = -12;
      clipped = true;
    }
    if (timezone > +14) {
      timezone = +14;
      clipped = true;
    }
    if (clipped) error = gettext ("The timezone was clipped");
    Database_Config_General::setTimezone (timezone);
    success = gettext ("The timezone was saved");
  }

  int timezone = Database_Config_General::getTimezone();
  view.set_variable ("timezone", convert_to_string (timezone));
  view.set_variable ("success", success);
  view.set_variable ("error", error);
  
  page += view.render ("administration", "timezone");

  page += Assets_Page::footer ();

  return page;
}
