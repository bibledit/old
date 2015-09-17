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


#include <resource/organize.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/passage.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <resource/logic.h>
#include <resource/admin.h>
#include <sync/logic.h>
#include <dialog/entry.h>
#include <config/logic.h>
#include <client/logic.h>


string resource_organize_url ()
{
  return "resource/organize";
}


bool resource_organize_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string resource_organize (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  if (request->query.count ("add")) {
    string add = request->query["add"];
    vector <string> resources = request->database_config_user()->getActiveResources ();
    resources.push_back (add);
    request->database_config_user()->setActiveResources (resources);
    request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_resources_organization);
  }
  
  
  if (request->query.count ("remove")) {
    size_t remove = convert_to_int (request->query["remove"]);
    vector <string> resources = request->database_config_user()->getActiveResources ();
    if (remove < resources.size ()) {
      resources.erase (resources.begin () + remove);
    }
    request->database_config_user()->setActiveResources (resources);
    request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_resources_organization);
  }
  
  
  if (request->post.count ("resources")) {
    string resources = request->post ["resources"];
    vector <string> v_resources = filter_string_explode (resources, ',');
    request->database_config_user()->setActiveResources (v_resources);
    request->database_config_user()->addUpdatedSetting (Sync_Logic::settings_send_resources_organization);
    return "";
  }
  

  string page;
  Assets_Header header = Assets_Header (translate("Resources"), request);
  header.jQueryUIOn ();
  page = header.run ();
  Assets_View view = Assets_View ();

  
  // Active resources.
  vector <string> active_resources = request->database_config_user()->getActiveResources ();
  string activesblock;
  for (size_t i = 0; i < active_resources.size (); i++) {
    activesblock.append ("<p><a href=\"?remove=" + convert_to_string (i) + "\"> ✗ </a>" + active_resources [i] + "</p>\n");
  }
  view.set_variable ("activesblock", activesblock);
  
  
  // The selectable resources are the available ones minus the active ones.
  vector <string> available_resources = resource_logic_get_names (webserver_request);
  vector <string> selectable_resources = filter_string_array_diff (available_resources, active_resources);
  string selectablesblock;
  for (unsigned int i = 0; i < selectable_resources.size (); i++) {
    string selectable = selectable_resources [i];
    if (i) selectablesblock.append (" | ");
    selectablesblock.append ("<a href=\"?add=" + selectable + "\">" + selectable + "</a>");
  }
  view.set_variable ("selectablesblock", selectablesblock);
  
  
  // The dividers.
  vector <string> dividers = {
    resource_logic_yellow_divider (),
    resource_logic_green_divider (),
    resource_logic_blue_divider (),
    resource_logic_violet_divider (),
    resource_logic_red_divider (),
    resource_logic_orange_divider ()
  };
  string dividersblock;
  for (unsigned int i = 0; i < dividers.size (); i++) {
    string divider = dividers [i];
    if (i) dividersblock.append (" | ");
    dividersblock.append ("<a href=\"?add=" + divider + "\">" + divider + "</a>");
  }
  view.set_variable ("dividersblock", dividersblock);
  
  
  // Context before.
  if (request->query.count ("before")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("organize", translate("Please enter the number of verses"), convert_to_string (request->database_config_user ()->getResourceVersesBefore ()), "before", translate ("How many verses of context to display before the focused verse."));
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("before")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 0) && (value <= 100)) {
      request->database_config_user ()->setResourceVersesBefore (value);
    }
  }
  view.set_variable ("before", convert_to_string (request->database_config_user ()->getResourceVersesBefore ()));

  
  // Context after.
  if (request->query.count ("after")) {
    Dialog_Entry dialog_entry = Dialog_Entry ("organize", translate("Please enter the number of verses"), convert_to_string (request->database_config_user ()->getResourceVersesAfter ()), "after", translate ("How many verses of context to display after the focused verse."));
    page += dialog_entry.run ();
    return page;
  }
  if (request->post.count ("after")) {
    int value = convert_to_int (request->post["entry"]);
    if ((value >= 0) && (value <= 100)) {
      request->database_config_user ()->setResourceVersesAfter (value);
    }
  }
  view.set_variable ("after", convert_to_string (request->database_config_user ()->getResourceVersesAfter ()));
  
  
  page += view.render ("resource", "organize");
  page += Assets_Page::footer ();
  return page;
}
