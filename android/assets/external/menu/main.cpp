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


#include <menu/main.h>
#include <locale/translate.h>
#include <libxml/xmlwriter.h>
#include <menu/logic.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <webserver/request.h>
#include <email/index.h>
#include <config/logic.h>
#include <manage/index.h>
#include <manage/users.h>
#include <administration/language.h>
#include <administration/timezone.h>
#include <styles/indext.h>
#include <styles/indexm.h>
#include <versification/index.h>
#include <bible/manage.h>
#include <edit/index.h>
#include <editusfm/index.h>
#include <editverse/index.h>
#include <workbench/logic.h>
#include <workbench/index.h>
#include <workbench/organize.h>
#include <collaboration/index.h>
#include <client/index.h>
#include <client/logic.h>
#include <sendreceive/index.h>
#include <search/index.h>
#include <config/globals.h>
#include <resource/index.h>
#include <resource/manage.h>
#include <resource/admin.h>
#include <resource/print.h>
#include <mapping/index.h>
#include <notes/index.h>
#include <changes/changes.h>
#include <journal/index.h>
#include <changes/manage.h>
#include <index/listing.h>
#include <sprint/index.h>
#include <checks/index.h>
#include <checks/settings.h>
#include <consistency/index.h>
#include <manage/exports.h>
#include <manage/hyphenation.h>
#include <xrefs/index.h>
#include <editone/index.h>
#include <debug/index.h>
#include <paratext/index.h>
#include <personalize/index.h>
#include <resource/images.h>


/*
This generates a start menu.

It is based on arrays of all possible menu entries.

It reads the access levels of those entries.
It removes the menu entries the currently logged-in user has no access to, and keeps the rest.

It originally self-organized the entries such that the ones used clicked often came earlier in the menu.
But menu entries moving around creates confusion.
Therefore it was removed again.
*/


Menu_Main::Menu_Main (void * webserver_request_in)
{
  webserver_request = webserver_request_in;
}


Menu_Main::~Menu_Main ()
{
}


vector <Menu_Main_Item> * Menu_Main::mainmenu ()
{
  // This is the main menu.
  // It will be visible in the top bar.
  // The last element in the array is the submenu to display on expansion.
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (biblemenu ()) menu->push_back ( { "", "", translate ("Bible"), biblemenu () } );
  if (notesmenu ()) menu->push_back ( { "", "", translate ("Notes"), notesmenu () } );
  if (resourcesmenu ()) menu->push_back ( { "", "", translate ("Resources"), resourcesmenu () } );
  if (changesmenu ()) menu->push_back ( { "", "", translate ("Changes"), changesmenu () } );
  if (planningmenu ()) menu->push_back ( { "", "", translate ("Planning"),  planningmenu () } );
  if (toolsmenu ()) menu->push_back ( { "", "", translate ("Tools"), toolsmenu () } );
  if (settingsmenu ()) menu->push_back ( { "", "", translate ("Settings"), settingsmenu () } );
  if (helpmenu ()) menu->push_back ( { "", "", translate ("Help"), helpmenu () } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::biblemenu ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (bible_edit_menu ()) menu->push_back ( { "", "",  translate ("Edit"), bible_edit_menu () } );
  if (search_index_acl (request)) menu->push_back ( { "", search_index_url (), translate ("Search"), NULL } );
  if (bible_workbench_menu ()) menu->push_back ( { "", "", translate ("Workbench"), bible_workbench_menu () } );
  if (checks_index_acl (request)) menu->push_back ( { "", checks_index_url (), translate ("Checks"), bible_checks_menu () } );
  if (bible_manage_acl (webserver_request)) menu->push_back ( { "", bible_manage_url (), translate ("Bibles"), NULL} );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::bible_edit_menu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (edit_index_acl (webserver_request)) menu->push_back ( { "", edit_index_url (), translate ("Visual chapter"), NULL } );
  if (editone_index_acl (webserver_request)) menu->push_back ( { "", editone_index_url (), translate ("Visual verse"), NULL } );
  if (editusfm_index_acl (webserver_request)) menu->push_back ( { "", editusfm_index_url (),  translate ("USFM chapter"), NULL } );
  if (editverse_index_acl (webserver_request)) menu->push_back ( { "", editverse_index_url (), translate ("USFM verse"),   NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::bible_workbench_menu ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  // Start building the Workbench menu.
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  // Add the available configured Workbenches to the menu.
  // The user's role should be sufficiently high.
  if (workbench_organize_acl (request)) {
    vector <string> workbenches = workbenchGetWorkbenches (request);
    for (size_t i = 0; i < workbenches.size(); i++) {
      menu->push_back ( {"", workbench_index_url () + "?bench=" + convert_to_string (i), workbenches[i], NULL});
    }
  }
  // Finally add the Workbench Organizer.
  if (workbench_organize_acl (request)) menu->push_back ( { "", workbench_organize_url (), translate ("Organize"), NULL } );
  // The result.
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::bible_checks_menu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (checks_settings_acl (webserver_request)) menu->push_back ( { "", checks_settings_url (), translate ("Manage"), NULL } );
  if (consistency_index_acl (webserver_request)) menu->push_back ( { "", consistency_index_url (), translate ("Consistency"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::notesmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  if (notes_index_acl (request)) menu->push_back ( { "", notes_index_url (), translate ("List"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::resourcesmenu ()
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (resource_index_acl (request)) menu->push_back ( { "", resource_index_url (), translate ("View"), NULL } );
  if (resource_print_acl (request)) menu->push_back ( { "", resource_print_url (), translate ("Print"), NULL } );
  if (!config_logic_client_prepared ()) {
    if (resource_manage_acl (request)) menu->push_back ( { "", resource_manage_url (), translate ("USFM"), NULL } );
    if (resource_admin_acl (request)) menu->push_back ( { "", resource_admin_url (), translate ("External"), NULL } );
    if (resource_images_acl (request)) menu->push_back ( { "", resource_images_url (), translate ("Images"), NULL } );
  }
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::changesmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (changes_changes_acl (webserver_request)) menu->push_back ( { "", changes_changes_url (), translate ("Notifications"), NULL } );
  // Downloading revisions only on server, not on client.
  if (!config_logic_client_prepared ())
    if (index_listing_acl (webserver_request, "revisions"))
      menu->push_back ( { "", index_listing_url ("revisions"), translate ("Download"), NULL } );
  // Managing change notifications only on server, not on client.
  if (!config_logic_client_prepared ())
    if (changes_manage_acl (webserver_request))
      menu->push_back ( { "", changes_manage_url (), translate ("Manage"), NULL } );
  if (journal_index_acl (webserver_request)) menu->push_back ( { "", journal_index_url (), translate ("Journal"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::planningmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (!config_logic_client_prepared ()) {
    if (sprint_index_acl (webserver_request)) menu->push_back ( { "", sprint_index_url (), translate ("Sprint"), NULL } );
  }
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::toolsmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (sendreceive_index_acl (webserver_request)) menu->push_back ( { "", sendreceive_index_url (), translate ("Sync"), NULL } );
  if (index_listing_acl (webserver_request, "exports")) menu->push_back ( { "", index_listing_url ("exports"), translate ("Exports"), exportssubmenu () } );
  if (manage_hyphenation_acl (webserver_request)) menu->push_back ( { "", manage_hyphenation_url (), translate ("Hyphenation"), NULL } );
  if (xrefs_index_acl (webserver_request)) menu->push_back ( { "", xrefs_index_url (), translate ("Cross references"), NULL } );
  if (debug_index_acl (webserver_request)) menu->push_back ( { "", debug_index_url (), translate ("Debug"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::exportssubmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (manage_exports_acl (webserver_request)) menu->push_back ( { "", manage_exports_url (), translate ("Manage"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::settingsmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (personalize_index_acl (webserver_request)) {
    menu->push_back ( { "", personalize_index_url (), translate ("Personalize"), NULL } );
  }
  if (manage_users_acl (webserver_request) && !config_logic_client_prepared ()) {
    menu->push_back ( { "", manage_users_url (), translate ("Users"), NULL } );
  }
  if (manage_index_acl (webserver_request)) {
    menu->push_back ( { "", manage_index_url (), translate ("Manage"), NULL } );
  }
  if (administration_language_acl (webserver_request)) {
    menu->push_back ( { "", administration_language_url (), translate ("Language"), NULL } );
  }
  if (administration_timezone_acl (webserver_request)) {
    // Display menu to set the site's timezone only in case the calling program has not yet set this zone in the library.
    // So for example the app for iOS can set the timezone from the device, and in case this has been done,
    // then the user no longer can set it through Bibledit.
    if    ((config_globals_timezone_offset_utc < MINIMUM_TIMEZONE)
        || (config_globals_timezone_offset_utc > MAXIMUM_TIMEZONE)) {
        menu->push_back ( { "", administration_timezone_url (), translate ("Timezone"), NULL } );
    }
  }
  if (email_index_acl (webserver_request) && !config_logic_client_prepared ()) {
    menu->push_back ( { "", email_index_url (), translate ("Mail"), NULL } );
  }
  if (styles_indext_acl (webserver_request)) {
    menu->push_back ( { "", styles_indext_url (), translate ("Styles"), stylessubmenu () } );
  }
  if (versification_index_acl (webserver_request)) {
    menu->push_back ( { "", versification_index_url (), translate ("Versifications"), NULL } );
  }
  if (mapping_index_acl (webserver_request)) {
    menu->push_back ( { "", mapping_index_url (), translate ("Verse mappings"), NULL } );
  }
  if (collaboration_index_acl (webserver_request)) {
    menu->push_back ( { "", collaboration_index_url (), translate ("Collaboration"), NULL } );
  }
  // If the installation is not prepared for Client mode, disable the client menu.
  // But keep the menu item in an open installation.
  bool client_menu = client_index_acl (webserver_request);
  if (!config_logic_client_prepared ()) client_menu = false;
  if (config_logic_demo_enabled ()) client_menu = true;
  if (client_menu) menu->push_back ( { "", client_index_url (), translate ("Cloud"), NULL } );
  // Paratext can be enabled through ./configure --enable-paratext.
  if (config_logic_paratext_enabled ()) {
    if (paratext_index_acl (webserver_request)) {
      menu->push_back ( { "", paratext_index_url (), translate ("Paratext"), NULL } );
    }
  }
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::stylessubmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  if (styles_indexm_acl (webserver_request)) menu->push_back ( { "", styles_indexm_url (), translate ("Manage"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


vector <Menu_Main_Item> * Menu_Main::helpmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "help/index", translate ("Help"), NULL } );
  if (menu->size ()) return menu;
  delete menu;
  return NULL;
}


// Create the menu.
string Menu_Main::create ()
{
  vector <Menu_Main_Item> * main_menu = mainmenu ();

  // To HTML for the CSS menu needs to be like this:
  //   <ul id="menu" class="menu">
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
  xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "id", "%s", "mainmenu");
  xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "class", "%s", "menu");

  // Go through the main menu.
  for (unsigned int i = 0; i < main_menu->size(); i++) {
    
    Menu_Main_Item item = main_menu->at (i);

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


void Menu_Main::submenu (xmlTextWriterPtr xmlwriter, vector <Menu_Main_Item> * menu)
{
  if (menu->size()) {
    xmlTextWriterStartElement (xmlwriter, BAD_CAST "ul");
    for (unsigned int i = 0; i < menu->size(); i++) {
      Menu_Main_Item item = menu->at (i);
      xmlTextWriterStartElement (xmlwriter, BAD_CAST "li");
      subsubmenu (xmlwriter, item.submenu);
      if (item.href == "") {
        xmlTextWriterStartElement (xmlwriter, BAD_CAST "span");
      } else {
        xmlTextWriterStartElement (xmlwriter, BAD_CAST "a");
        xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "href", "%s", menu_logic_href (item.href).c_str());
      }
      xmlTextWriterWriteFormatString (xmlwriter, "%s", item.text.c_str());
      xmlTextWriterEndElement (xmlwriter); // span
      xmlTextWriterEndElement (xmlwriter); // li
      if (item.submenu) delete item.submenu;
    }
    xmlTextWriterEndElement (xmlwriter); // ul
  }
}


void Menu_Main::subsubmenu (xmlTextWriterPtr xmlwriter, vector <Menu_Main_Item> * menu)
{
  if (!menu) return;
  xmlTextWriterStartElement (xmlwriter, BAD_CAST "ul");
  for (unsigned int i = 0; i < menu->size(); i++) {
    Menu_Main_Item item = menu->at (i);
    xmlTextWriterStartElement (xmlwriter, BAD_CAST "li");
    xmlTextWriterStartElement (xmlwriter, BAD_CAST "a");
    xmlTextWriterWriteFormatAttribute (xmlwriter, BAD_CAST "href", "%s", menu_logic_href (item.href).c_str());
    xmlTextWriterWriteFormatString (xmlwriter, "%s", item.text.c_str());
    xmlTextWriterEndElement (xmlwriter); // a
    xmlTextWriterEndElement (xmlwriter); // li
    if (item.submenu) delete item.submenu;
  }
  xmlTextWriterEndElement (xmlwriter); // ul
}
