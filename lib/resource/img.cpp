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


#include <resource/img.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <resource/logic.h>
#include <database/imageresources.h>
#include <config/logic.h>
#include <journal/index.h>
#include <dialog/yes.h>
#include <dialog/entry.h>


string resource_img_url ()
{
  return "resource/img";
}


bool resource_img_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::manager ());
}


string resource_img (void * webserver_request) // Todo
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_ImageResources database_imageresources;

  
  string page;
  Assets_Header header = Assets_Header (translate("Image resources"), request);
  page = header.run ();
  Assets_View view = Assets_View ();
  string error, success;
  
  
  string name = request->query ["name"];
  view.set_variable ("name", name);

  
  string image = request->query ["image"];
  view.set_variable ("image", image);


  
  // Retrieve passage range for this image.
  int book1, chapter1, verse1, book2, chapter2, verse2;
  database_imageresources.get (name, image, book1, chapter1, verse1, book2, chapter2, verse2);
  
  

  view.set_variable ("success", success);
  view.set_variable ("error", error);
  page += view.render ("resource", "img");
  page += Assets_Page::footer ();
  return page;
}
