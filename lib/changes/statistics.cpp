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


#include <changes/statistics.h>
#include <assets/header.h>
#include <assets/view.h>
#include <assets/page.h>
#include <filter/roles.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <client/logic.h>
#include <demo/logic.h>
#include <database/modifications.h>
#include <database/notes.h>
#include <trash/handler.h>
#include <ipc/focus.h>
#include <navigation/passage.h>
#include <changes/logic.h>
#include <menu/logic.h>


string changes_statistics_url ()
{
  return "changes/statistics";
}


bool changes_statistics_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string changes_statistics (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  

  string page;
  Assets_Header header = Assets_Header (translate("Change statistics"), request);
  page += header.run ();
  Assets_View view;
  
  
  view.set_variable ("interlinks", changes_interlinks (webserver_request, changes_statistics_url ())); // Todo put anywhere.
  
  
  page += view.render ("changes", "statistics");
  
  
  page += Assets_Page::footer ();
  return page;
}
