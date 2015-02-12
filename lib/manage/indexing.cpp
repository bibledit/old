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


#include <manage/indexing.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <tasks/logic.h>
#include <webserver/request.h>
#include <journal/index.h>
#include <database/config/general.h>
#include <locale/translate.h>


string manage_indexing_url ()
{
  return "manage/indexing";
}


bool manage_indexing_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string manage_indexing (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  page = Assets_Page::header (translate ("Indexing"), webserver_request, "");

  Assets_View view = Assets_View ();

  string refresh = request->query ["refresh"];
  
  if (refresh == "bibles") {
    tasks_logic_queue (REINDEXBIBLES);
    redirect_browser (request, journal_index_url ());
    return "";
  }
  
  if (refresh == "notes") {
    tasks_logic_queue (REINDEXNOTES);
    redirect_browser (request, journal_index_url ());
    return "";
  }

  page += view.render ("manage", "indexing");

  page += Assets_Page::footer ();

  return page;
}
