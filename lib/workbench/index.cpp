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


#include <workbench/index.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>
#include <database/notes.h>
#include <workbench/logic.h>
#include <menu/logic.h>
#include <ipc/focus.h>
#include <navigation/passage.h>


string workbench_index_url ()
{
  return "workbench/index";
}


bool workbench_index_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string workbench_index (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  vector <string> desktops = workbench_get_names (request);

  
  // Set the requested desktop as the active one.
  if (request->query.count ("bench")) {
    unsigned int bench = convert_to_int (request->query ["bench"]);
    if (bench < desktops.size ()) {
      string workbench = desktops [bench];
      request->database_config_user()->setActiveWorkbench (workbench);
    }
  }
  
  
  // Check that the active desktop exists, else set the first available desktop as the active one.
  {
    string desktop = request->database_config_user ()->getActiveWorkbench ();
    if (!in_array (desktop, desktops)) {
      if (!desktops.empty ()) {
        request->database_config_user ()->setActiveWorkbench (desktops [0]);
      }
    }
  }
  
  
  // Create default set of desktops if there are none.
  bool create = desktops.empty ();
  if (!create) {
    create = (desktops [0] == workbench_get_default_name ());
  }
  if (create) {
    workbench_create_defaults (webserver_request);
  }

  
  // In case the desktop is opened from a consultation note email,
  // read the note, and set the active passage to the passage the note refers to.
  int noteid = convert_to_int (request->query ["note"]);
  if (noteid) {
    Database_Notes database_notes (webserver_request);
    vector <Passage> passages = database_notes.getPassages (noteid);
    if (!passages.empty ()) {
      Ipc_Focus::set (webserver_request, passages[0].book, passages[0].chapter, convert_to_int (passages[0].verse));
      Navigation_Passage::recordHistory (webserver_request, passages[0].book, passages[0].chapter, convert_to_int (passages[0].verse));
    }
  }
  
  
  string page;
  Assets_Header header = Assets_Header (translate("Desktop"), request);
  header.setNavigator ();
  header.setFadingMenu (menu_logic_desktop_category (webserver_request));
  page = header.run ();
  Assets_View view; // TodojQuery

  
  map <int, string> urls = workbench_get_urls (request, true);
  map <int, string> widths = workbench_get_widths (request);
  for (unsigned int key = 0; key < 15; key++) {
    string url = urls [key];
    string width = widths [key];
    int row = round (key / 5) + 1;
    int column = key % 5 + 1;
    string variable = "url" + convert_to_string (row) + convert_to_string (column);
    view.set_variable (variable, url);
    variable = "width" + convert_to_string (row) + convert_to_string (column);
    view.set_variable (variable, width);
    if (convert_to_int (width) > 0) view.enable_zone (variable);
  }
  
  
  map <int, string> heights = workbench_get_heights (request);
  for (unsigned int key = 0; key < 3; key++) {
    string height = heights [key];
    int row = key + 1;
    string variable = "height" + convert_to_string (row);
    view.set_variable (variable, height);
    if (convert_to_int (height) > 0) view.enable_zone (variable);
  }
  
  
  string workbenchwidth = workbench_get_entire_width (request);
  if (!workbenchwidth.empty ()) {
    workbenchwidth.insert (0, "width: ");
    workbenchwidth.append (";");
  }
  view.set_variable ("workbenchwidth", workbenchwidth);
  
  
  // The rendered template disables framekillers through the "sandbox" attribute on the iframe elements.
  page += view.render ("workbench", "index");
  page += Assets_Page::footer ();
  return page;
}
