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


#include <workbench/organize.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <database/config/general.h>
#include <workbench/logic.h>
#include <dialog/yes.h>
#include <dialog/entry.h>
#include <menu/logic.h>


string workbench_organize_url ()
{
  return "workbench/organize";
}


bool workbench_organize_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::consultant ());
}


string workbench_organize (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  
  if (request->post.count ("add")) {
    string add = request->post["add"];
    request->database_config_user()->setActiveWorkbench (add);
    workbench_set_urls    (request, workbench_get_default_urls (0));
    workbench_set_widths  (request, workbench_get_default_widths (0));
    workbench_set_heights (request, workbench_get_default_heights (0));
  }
  
  
  // Re-ordering desktops.
  if (request->query.count ("up")) {
    size_t item = convert_to_int (request->query ["up"]);
    vector <string> desktops = workbench_get_names (request);
    array_move_up_down (desktops, item, true);
    workbench_reorder (request, desktops);
  }
  if (request->query.count ("down")) {
    size_t item = convert_to_int (request->query ["down"]);
    vector <string> desktops = workbench_get_names (request);
    array_move_up_down (desktops, item, false);
    workbench_reorder (request, desktops);
  }
  
  
  // Create and reset all default desktops.
  if (request->query.count ("defaults")) {
    workbench_create_defaults (webserver_request);
  }
  
  
  string page;
  
  
  Assets_Header header = Assets_Header (translate("Desktops"), request);
  header.addBreadCrumb (menu_logic_settings_menu (), menu_logic_settings_text ());
  page = header.run ();
  
  
  if (request->query.count ("remove")) {
    string remove = request->query["remove"];
    string confirm = request->query["confirm"];
    if (confirm.empty ()) {
      Dialog_Yes dialog_yes = Dialog_Yes ("organize", translate("Would you like to delete this desktop configuration?"));
      dialog_yes.add_query ("remove", remove);
      page += dialog_yes.run ();
      return page;
    }
    if (confirm == "yes") {
      workbench_delete (request, remove);
    }
  }
  
  
  // Copy desktop.
  if (request->query.count ("copy")) {
    string desktop = request->query ["copy"];
    Dialog_Entry dialog_entry ("organize", translate("Please enter a name for the new desktop"), "", "destination", "");
    dialog_entry.add_query ("source", desktop);
    page.append (dialog_entry.run ());
    return page;
  }
  if (request->query.count ("source")) {
    string source = request->query ["source"];
    string destination = request->post ["entry"];
    workbench_copy (webserver_request, source, destination);
  }

  
  // Send desktop to all users.
  string send = request->query ["send"];
  if (!send.empty ()) {
    string me = request->session_logic ()->currentUser ();
    vector <string> users = request->database_users ()->getUsers ();
    for (auto user : users) {
      if (user != me) {
        workbench_send (webserver_request, send, user);
      }
    }
  }

  
  Assets_View view;
  
  
  vector <string> desktopblock;
  vector <string> desktops = workbench_get_names (request);
  for (size_t i = 0; i < desktops.size (); i++) {
    string desktop = desktops [i];
    desktopblock.push_back ("<p>");
    desktopblock.push_back ("<a href=\"?remove=" + desktop + "\" title=\"" + translate("Delete desktop") + "\"> ✗ </a>");
    desktopblock.push_back ("|");
    desktopblock.push_back ("<a href=\"?up=" + convert_to_string (i) + "\" title=\"" + translate("Move desktop up") + "\"> " + unicode_black_up_pointing_triangle () + " </a>");
    desktopblock.push_back ("|");
    desktopblock.push_back ("<a href=\"?down=" + convert_to_string (i) + "\" title=\"" + translate("Move desktop down") + "\"> " + unicode_black_down_pointing_triangle () + " </a>");
    desktopblock.push_back ("|");
    desktopblock.push_back ("<a href=\"settings?name=" + desktop + "\" title=\"" + translate("Edit desktop") + "\"> ✎ </a>");
    desktopblock.push_back ("|");
    desktopblock.push_back ("<a href=\"?copy=" + desktop + "\" title=\"" + translate("Copy desktop") + "\"> ⎘ </a>");
#ifndef CLIENT_PREPARED
    // In the Cloud, one can send the desktop configuration to other users.
    // On a client, sending a desktop to other users does not work.
    desktopblock.push_back ("|");
    desktopblock.push_back ("<a href=\"?send=" + desktop + "\" title=\"" + translate("Send desktop to all users") + "\"> ✉ </a>");
#endif
    desktopblock.push_back ("|");
    desktopblock.push_back ("<span>" + desktop + "</span>");
    desktopblock.push_back ("</p>");
  }
  view.set_variable ("desktopblock", filter_string_implode (desktopblock, "\n"));

  
#ifndef CLIENT_PREPARED
    view.enable_zone ("cloud");
#endif
  

  page += view.render ("workbench", "organize");
  
  
  page += Assets_Page::footer ();
  
  
  return page;
}
