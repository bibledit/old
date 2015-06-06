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


#include <resource/get.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <resource/logic.h>


string resource_get_url ()
{
  return "resource/get";
}


bool resource_get_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string resource_get (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  string page;

  
  string resource = request->query["resource"];
  string book = request->query["book"];
  string chapter = request->query["chapter"];
  string verse = request->query["verse"];

  
  if (!resource.empty () && !book.empty () && !chapter.empty () && !verse.empty ()) {


    unsigned int iresource = convert_to_int (resource);
    int ibook = convert_to_int (book);
    int ichapter = convert_to_int (chapter);
    int iverse = convert_to_int (verse);

    
    // In JavaScript the resource identifier starts at 1 instead of at 0.
    iresource--;
    
    
    vector <string> resources = request->database_config_user()->getActiveResources ();
    if (iresource <= resources.size ()) {
      resource = resources [iresource];
      page = Resource_Logic::getHtml (request, resource, ibook, ichapter, iverse);
    }
  }
  
  
  return page;
}
