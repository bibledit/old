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


#include <menu/user.h>
#include <locale/translate.h>
#include <libxml/xmlwriter.h>
#include <menu/logic.h>
#include <webserver/request.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <session/logout.h>
#include <session/login.h>
#include <user/notifications.h>
#include <user/account.h>
#include <client/logic.h>
#include <config/logic.h>


// This generates a user menu.
// It is based on all possible menu entries.
// It reads the access levels of those entries.
// It only keeps the menu entries the currently logged-in user has access to.
// It considers whether the user is logged in.



Menu_User::Menu_User (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Menu_User::~Menu_User ()
{
}


vector <Menu_User_Item> * Menu_User::mainmenu (string request)
{
  // This is the main menu.
  // It will be visible in the top bar.
  // The last element in the array is the submenu to display on expansion.
  string username = ((Webserver_Request *) webserver_request)->session_logic ()->currentUser ();
  vector <Menu_User_Item> * menu = new vector <Menu_User_Item>;
  if (username.empty ()) {
    menu->push_back ( { "", convert_to_string (session_login_url ()) + "?request=" + request, translate ("Login"), NULL } );
  } else {
    // Gear: "⚙"
    menu->push_back ( { "", "",  username, usermenu () } );
  }
  return menu;
}


vector <Menu_User_Item> * Menu_User::usermenu ()
{
  // Generate the user menu.
  // Take access control into account.

  bool client = client_logic_client_enabled ();
  bool demo = config_logic_demo_enabled ();

  vector <Menu_User_Item> * menu = new vector <Menu_User_Item>;
  if (!(client || demo)) if (session_logout_acl (webserver_request)) menu->push_back ( { "", session_logout_url (), translate ("Logout"), NULL } );
  if (user_notifications_acl (webserver_request)) menu->push_back ( { "", user_notifications_url (), translate ("Notifications"), NULL } );
  if (!(client || demo)) if (user_account_acl (webserver_request)) menu->push_back ( { "", user_account_url (), translate ("Account"), NULL } );
  return menu;
}


// Create the menu.
string Menu_User::create (string request)
{
  vector <Menu_User_Item> * main_menu = mainmenu (request);

  // To create CSS menu the HTML structure needs to be like this:
  //   <ul id="usermenu" class="menu">
  //     <li>
  //       Menu entry
  //         <li>Subitem</li>
  //       </ul>
  //     </li>
  //     <li>Another entry</li>
  //   </ul>

  // XML writer.
  xmlBufferPtr xmlbuffer = xmlBufferCreate ();
  xmlTextWriterPtr xmlwriter = xmlNewTextWriterMemory (xmlbuffer, 0);

  // Setup and open document.
  xmlTextWriterStartDocument (xmlwriter, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent (xmlwriter, 1);
  xmlTextWriterStartElement (xmlwriter, BAD_CAST "ul");
  xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "id", "%s", "usermenu");
  xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "class", "%s", "menu");

  // Go through the main menu.
  for (unsigned int i = 0; i < main_menu->size(); i++) {
    
    Menu_User_Item item = main_menu->at (i);

    // Build the main menu.
    xmlTextWriterStartElement (xmlwriter, BAD_CAST "li");
    xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "class", "%s", "toggle");
    if (item.href == "") {
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "span");
    } else {
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "a");
      xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "href", "%s", menu_logic_href (item.href).c_str());
    }
    xmlTextWriterWriteFormatString (xmlwriter, "%s", item.text.c_str());
    xmlTextWriterEndElement (xmlwriter); // span
   
    // Build the submenu.
    if (item.submenu) submenu (xmlwriter, item.submenu);
    
    // Free dynamic memory submenu.
    if (item.submenu) delete (item.submenu);

    xmlTextWriterEndElement (xmlwriter); // li
  }
  xmlTextWriterEndElement (xmlwriter); // ul

  // Close document and get its contents.
  xmlTextWriterEndDocument (xmlwriter);
  xmlTextWriterFlush (xmlwriter);
  string html = (char *) xmlbuffer->content;

  // Free memory.
  if (xmlwriter) xmlFreeTextWriter(xmlwriter);
  if (xmlbuffer) xmlBufferFree(xmlbuffer);
  xmlCleanupCharEncodingHandlers ();
  delete main_menu;

  // Remove the <?xml version="1.0" encoding="UTF-8"?>
  html.erase (0, html.find (">") + 1);

  // The resulting html.  
  return html;
}


void Menu_User::submenu (xmlTextWriterPtr xmlwriter, vector <Menu_User_Item> * menu)
{
  if (menu->size()) {
    xmlTextWriterStartElement (xmlwriter, BAD_CAST "ul");
    for (unsigned int i = 0; i < menu->size(); i++) {
      Menu_User_Item item = menu->at (i);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "li");
      if (item.href == "") {
        xmlTextWriterStartElement (xmlwriter, BAD_CAST "span");
      } else {
        xmlTextWriterStartElement (xmlwriter, BAD_CAST "a");
        xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "href", "%s", menu_logic_href (item.href).c_str());
      }
      xmlTextWriterWriteFormatString (xmlwriter, "%s", item.text.c_str());
      xmlTextWriterEndElement (xmlwriter); // span or a
      xmlTextWriterEndElement (xmlwriter); // li
      if (item.submenu) delete item.submenu;
    }
    xmlTextWriterEndElement (xmlwriter); // ul
  }
}


