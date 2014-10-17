/*
Copyright (©) 2003-2014 Teus Benschop.

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
#include <string>
#include <vector>
#include <locale/translate.h>
#include <libxml/xmlwriter.h>
#include <menu/logic.h>


using namespace std;


/*
This generates a start menu.

It is based on arrays of all possible menu entries.

It reads the access levels of those entries.
It removes the menu entries the currently logged-in user has no access to, and keeps the rest.

It originally self-organized the entries such that the ones used clicked often came earlier in the menu.
But menu entries moving around creates confusion.
Therefore it was removed again.
*/


Menu_Main::Menu_Main ()
{
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
  menu->push_back ( { "", "", gettext ("Bible"),     biblemenu ()     } );
  menu->push_back ( { "", "", gettext ("Notes"),     notesmenu ()     } );
  menu->push_back ( { "", "", gettext ("Resources"), resourcesmenu () } );
  menu->push_back ( { "", "", gettext ("Changes"),   changesmenu ()   } );
  menu->push_back ( { "", "", gettext ("Planning"),  planningmenu ()  } );
  menu->push_back ( { "", "", gettext ("Tools"),     toolsmenu ()     } );
  menu->push_back ( { "", "", gettext ("Settings"),  settingsmenu ()  } );
  menu->push_back ( { "", "", gettext ("Help"),      helpmenu ()      } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::biblemenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "edit/index",      gettext ("Edit"),      bible_edit_menu ()      } );
  menu->push_back ( { "", "search/index",    gettext ("Search"),    NULL                    } );
  menu->push_back ( { "", "workbench/index", gettext ("Workbench"), bible_workbench_menu () } );
  menu->push_back ( { "", "checks/index",    gettext ("Checks"),    bible_checks_menu ()    } );
  menu->push_back ( { "", "bible/manage",    gettext ("Bibles"),    NULL                    } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::bible_edit_menu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "editusfm/index",  gettext ("USFM chapter"), NULL } );
  menu->push_back ( { "", "editverse/index", gettext ("USFM verse"),   NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::bible_workbench_menu ()
{
  // Start building the Workbench menu.
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;

  /* C++Port
  // Add the available configured Workbenches to the menu. 
  $workbenches = Workbench_Logic::getWorkbenches ();
  foreach ($workbenches as $offset => $workbench) {
    $menu [] = array ("workbench/index?bench=$offset", $workbench, NULL);
  }
  */

  // Finally add the Workbench Organizer.    
  menu->push_back ( { "", "workbench/organize", gettext ("Organize"), NULL } );
  
  // The result.
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::bible_checks_menu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "checks/settings", gettext ("Manage"), NULL } );
  menu->push_back ( { "", "consistency/index", gettext ("Consistency"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::notesmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "notes/index", gettext ("List"), NULL } );
  menu->push_back ( { "", "notes/editsource", gettext ("Edit"), NULL } );
  menu->push_back ( { "", "notes/clean", gettext ("Checks"), NULL } );
  menu->push_back ( { "", "notes/import1", gettext ("Import"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::resourcesmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "resource/index", gettext ("View"), NULL } );
  menu->push_back ( { "", "resource/print", gettext ("Print"), NULL } );
  menu->push_back ( { "", "resource/manage", gettext ("USFM"), NULL } );
  menu->push_back ( { "", "resource/admin", gettext ("External"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::changesmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "changes/changes", gettext ("Notifications"), NULL } );
  menu->push_back ( { "", "changes/history", gettext ("History"), NULL } );
  menu->push_back ( { "", "revisions", gettext ("Download"), NULL } );
  menu->push_back ( { "", "changes/manage", gettext ("Manage"), NULL } );
  menu->push_back ( { "", "journal/index", gettext ("Journal"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::planningmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "sprint/index", gettext ("Sprint"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::toolsmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "sendreceive/index", gettext ("Sync"), NULL } );
  menu->push_back ( { "", "exports", gettext ("Exports"), exportssubmenu () } );
  menu->push_back ( { "", "manage/hyphenation", gettext ("Hyphenation"), NULL } );
  menu->push_back ( { "", "xrefs/index", gettext ("Cross references"), NULL } );
  menu->push_back ( { "", "phpliteadmin/index", gettext ("phpLiteAdmin"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::exportssubmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "manage/exports", gettext ("Manage"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::settingsmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "users", "manage/users", gettext ("Users"), NULL } );
  menu->push_back ( { "", "manage/indexing", gettext ("Indexing"), NULL } );
  menu->push_back ( { "", "administration/language", gettext ("Language"), NULL } );
  menu->push_back ( { "", "administration/timezone", gettext ("Timezone"), NULL } );
  menu->push_back ( { "mail", "administration/mail", gettext ("Mail"), NULL } );
  menu->push_back ( { "", "styles/indext", gettext ("Styles"), stylessubmenu () } );
  menu->push_back ( { "", "versification/index", gettext ("Versifications"), NULL } );
  menu->push_back ( { "", "mapping/index", gettext ("Verse mappings"), NULL } );
  menu->push_back ( { "collaboration", "administration/collaboration", gettext ("Collaboration"), NULL } );
  menu->push_back ( { "client", "administration/client", gettext ("Client"), NULL } );
  menu->push_back ( { "", "fonts/index", gettext ("Fonts"), NULL } );
/* C++Port
    // If the installation is not prepared for Client mode, disable the client menu.
    // But keep the menu item in an open installation.
    include ("config/open.php");
    if (!$open_installation) {
      if (!Filter_Client::prepared ()) {
        unset ($menu ["client"]);
      }
    }
    // If Client mode is enabled, disable certain menu entries.
    if (Filter_Client::enabled ()) {
      unset ($menu ["mail"]);
      unset ($menu ["users"]);
      unset ($menu ["collaboration"]);
    }
*/
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::stylessubmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "styles/indexm", gettext ("Manage"), NULL } );
  return menu;
}


vector <Menu_Main_Item> * Menu_Main::helpmenu ()
{
  vector <Menu_Main_Item> * menu = new vector <Menu_Main_Item>;
  menu->push_back ( { "", "help/index", gettext ("Help"), NULL } );
  menu->push_back ( { "", "administration/phpinfo", gettext ("PHPInfo"), NULL } );
  return menu;
}

/* C++Port
  private function accesscontrol ($menu)
  {
    // Go through the main menu items.
    foreach ($menu as $offset1 => $dummy) {

      // Go through the sub menu items.
      foreach ($menu [$offset1] [2] as $offset2 => $dummy) {

        // Go through the sub sub menu items, if there are any.
        // Remove items the user has no access to.
        if ($menu [$offset1] [2] [$offset2] [2]) {
          foreach ($menu [$offset1] [2] [$offset2] [2] as $offset3 => $dummy) {
            $url = $menu [$offset1] [2] [$offset2] [2] [$offset3] [0];
            $access = Menu_Logic::check_user_access ($url);
            if (!$access) {
              unset ($menu [$offset1] [2] [$offset2] [2] [$offset3]);
            }
          }
        }

        // Remove sub menu item itself if the user has no access to it,
        $url = $menu [$offset1] [2] [$offset2] [0];
        $access = Menu_Logic::check_user_access ($url);
        if (!$access) {
          unset ($menu [$offset1] [2] [$offset2]);
        }

        // Remove the sub menu item if it has no children left anymore.
        if (isset ($menu [$offset1] [2] [$offset2] [2])) {
          $children = $menu [$offset1] [2] [$offset2] [2];
          if (count ($children) == 0) {
            unset ($menu [$offset1] [2] [$offset2]);
          }
        }
      }

      // Remove the main menu item if it has no children left anymore.
      $children = $menu [$offset1] [2];
      if (count ($children) == 0) {
        unset ($menu [$offset1]);
      }
    }

    // The updated menu.
    return $menu;
  }


*/


// Create the menu.
string Menu_Main::create ()
{
  // Modify the menu based on user access level.
/* C++Port
  $mainmenu = $this->accesscontrol ($this->mainmenu ());
*/
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


