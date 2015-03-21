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


#include <edit/edit.h>
#include <edit/index.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <filter/url.h>
#include <filter/date.h>
#include <webserver/request.h>
#include <ipc/focus.h>
#include <navigation/passage.h>
#include <locale/translate.h>


string edit_edit_url ()
{
  return "edit/edit";
}


bool edit_edit_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::translator ());
}


string edit_edit (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  
  // Optionally handle a call to the page to set the passage.
  if (request->query.count ("switchbook")) {
    int switchbook = convert_to_int (request->query ["switchbook"]);
    int switchchapter = 1;
    if (request->query.count ("switchchapter")) {
      switchchapter = convert_to_int (request->query ["switchchapter"]);
    }
    int switchverse = 1;
    if (request->query.count ("switchverse")) {
      switchverse = convert_to_int (request->query ["switchverse"]);
    }
    Ipc_Focus::set (request, switchbook, switchchapter, switchverse);
    Navigation_Passage::recordHistory (request, switchbook, switchchapter, switchverse);
  }
  
  
  // Check whether a Bible editor is alive.
  int timestamp = request->database_ipc()->getBibleAlive ();
  bool alive = (timestamp > (filter_date_seconds_since_epoch () - 5));
  
  
  if (alive) {
    // If a Bible editor is alive, send javascript to the browser to close this new window.
    string message = translate("The passage has been opened in the existing Bible editor in another browser tab.");
    string script =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "</head>\n"
    "<body>\n"
    "<script>\n"
    "setTimeout (closeself, 1000);\n"
    "function closeself () {\n"
    "  var win = window.open (\"\" ,\"_self\", \"\");\n"
    "  win.close();\n"
    "}\n"
    "</script>\n"
    "<div>" + message + "</div>\n"
    "</body>\n"
    "</html>\n";
    return script;
  } else {
    // If no Bible editor is alive, forward the browser to the Bible editor.
    redirect_browser (request, edit_index_url ());
  }
  
  return "";
}

