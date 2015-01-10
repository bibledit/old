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


#include <edit/styles.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <webserver/request.h>
#include <ipc/focus.h>


string edit_styles_url ()
{
  return "edit/styles";
}


bool edit_styles_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string edit_styles (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  return "";
}

/* Todo
 
 require_once ("../bootstrap/bootstrap.php");
 page_access_level (Filter_Roles::translator ());
 
 
 @$style = request->query['style'];
 if (isset ($style)) {
 Editor_Styles::recordUsage ($style);
 $action = Editor_Styles::getAction ($style);
 $data = array ('style' => $style, 'action' => $action);
 echo json_encode ($data);
 die;
 }
 
 
 @$all = request->query ['all'];
 if (isset ($all)) {
 echo Editor_Styles::getAll ();
 die;
 }
 
 
 echo Editor_Styles::getRecentlyUsed ();
 

 
 */