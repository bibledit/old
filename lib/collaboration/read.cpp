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


#include <collaboration/read.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/git.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <database/config/bible.h>
#include <dialog/list.h>


string collaboration_read_url ()
{
  return "collaboration/read";
}


bool collaboration_read_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::admin ());
}


string collaboration_read (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  string page;
  Assets_Header header = Assets_Header (gettext("Collaboration"), request);
  page = header.run ();
  Assets_View view = Assets_View ();
  

  // The Bible (object) to work with.
  string object = request->query ["object"];
  view.set_variable ("object", object);
  
  
  // Deal wih possibly setting or updating the URL of the remote repository.
  if (request->post.count ("url")) {
    string url = request->post["urlvalue"];
    Database_Config_Bible::setRemoteRepositoryUrl (object, url);
  }
  string url = Database_Config_Bible::getRemoteRepositoryUrl (object);
  view.set_variable ("url", url);

  
  // Create the git repository directory now since this is the most convenient moment to do it.
  if (!object.empty ()) {
    string directory = filter_git_directory (object);
    filter_url_rmdir (directory);
    filter_url_mkdir (directory);
  }
  

  string error;
  bool online = filter_git_remote_read (url, error); // Todo
  if (online) {
    string success_message = gettext("Read access to the repository is successful.");
    view.set_variable ("success_message", success_message);
  } else {
    string error_message = gettext("Read access failed. Please retry it, possibly with another URL.");
    view.set_variable ("error_message", error_message);
    view.set_variable ("error", error);
  }
  
  
  page += view.render ("collaboration", "read");
  page += Assets_Page::footer ();
  return page;
}
