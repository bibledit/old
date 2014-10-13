<?php
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


require_once ("bootstrap/bootstrap.php");


/*
This generates a start menu.

It is based on arrays of all possible menu entries.

It reads the access levels of those entries.
It removes the menu entries the currently logged-in user has no access to, and keeps the rest.

It originally self-organized the entries such that the ones used clicked often came earlier in the menu.
But menu entries moving around creates confusion.
Therefore it was removed again.
*/


class Menu_Main
{


  private function mainmenu ()
  {
    // This is the main menu.
    // It will be visible in the top bar.
    // The last element in the array is the submenu to display on expansion.
    $menu = array (
      array ("", Locale_Translate::_("Bible"), $this->biblemenu ()),
      array ("", Locale_Translate::_("Notes"), $this->notesmenu ()),
      array ("", Locale_Translate::_("Resources"), $this->resourcesmenu ()),
      array ("", Locale_Translate::_("Changes"), $this->changesmenu ()),
      array ("", Locale_Translate::_("Planning"), $this->planningmenu ()),
      array ("", Locale_Translate::_("Tools"), $this->toolsmenu ()),
      array ("", Locale_Translate::_("Settings"), $this->settingsmenu ()),
      array ("", Locale_Translate::_("Help"), $this->helpmenu ()),
    );
    return $menu;
  }


  private function biblemenu ()
  {
    $menu = array (
      array ("edit/index", Locale_Translate::_("Edit"), $this->bible_edit_menu ()),
      array ("search/index", Locale_Translate::_("Search"), NULL),
      array ("workbench/index", Locale_Translate::_("Workbench"), $this->bible_workbench_menu ()),
      array ("checks/index", Locale_Translate::_("Checks"), $this->bible_checks_menu ()),
      array ("bible/manage", Locale_Translate::_("Bibles"), NULL),
    );
    return $menu;
  }


  private function bible_edit_menu ()
  {
    $menu = array (
      array ("editusfm/index", Locale_Translate::_("USFM chapter"), NULL),
      array ("editverse/index", Locale_Translate::_("USFM verse"), NULL),
    );
    return $menu;
  }


  private function bible_workbench_menu ()
  {
    // Start building the Workbench menu.
    $menu = array ();

    // Add the available configured Workbenches to the menu.
    $workbenches = Workbench_Logic::getWorkbenches ();
    foreach ($workbenches as $offset => $workbench) {
      $menu [] = array ("workbench/index?bench=$offset", $workbench, NULL);
    }

    // Finally add the Workbench Organizer.    
    $menu [] = array ("workbench/organize", Locale_Translate::_("Organize"), NULL);
    
    // The result.
    return $menu;
  }


  private function bible_checks_menu ()
  {
    $menu = array (
      array ("checks/settings", Locale_Translate::_("Manage"), NULL),
      array ("consistency/index", Locale_Translate::_("Consistency"), NULL),
    );
    return $menu;
  }


  private function notesmenu ()
  {
    $menu = array (
      array ("notes/index", Locale_Translate::_("List"), NULL),
      array ("notes/editsource", Locale_Translate::_("Edit"), NULL),
      array ("notes/clean",  Locale_Translate::_("Checks"), NULL),
      array ("notes/import1", Locale_Translate::_("Import"), NULL),
    );
    return $menu;
  }


  private function resourcesmenu ()
  {
    $menu = array (
      array ("resource/index", Locale_Translate::_("View"), NULL),
      array ("resource/print", Locale_Translate::_("Print"), NULL),
      array ("resource/manage", Locale_Translate::_("USFM"), NULL),
      array ("resource/admin", Locale_Translate::_("External"), NULL),
    );
    return $menu;
  }


  private function changesmenu ()
  {
    $menu = array (
      array ("changes/changes", Locale_Translate::_("Notifications"), NULL),
      array ("changes/history", Locale_Translate::_("History"), NULL),
      array ("revisions", Locale_Translate::_("Download"), NULL),
      array ("changes/manage", Locale_Translate::_("Manage"), NULL),
      array ("journal/index", Locale_Translate::_("Journal"), NULL),
    );
    return $menu;
  }


  private function planningmenu ()
  {
    $menu = array (
      array ("sprint/index", Locale_Translate::_("Sprint"), NULL),
    );
    return $menu;
  }


  private function toolsmenu ()
  {
    $menu = array (
      array ("sendreceive/index", Locale_Translate::_("Sync"), NULL),
      array ("exports", Locale_Translate::_("Exports"), $this->exportssubmenu ()),
      array ("manage/hyphenation", Locale_Translate::_("Hyphenation"), NULL),
      array ("xrefs/index", Locale_Translate::_("Cross references"), NULL),
      array ("phpliteadmin/index", "phpLiteAdmin", NULL),
    );
    return $menu;
  }


  private function exportssubmenu ()
  {
    $menu = array (
      array ("manage/exports", Locale_Translate::_("Manage"), NULL),
    );
    return $menu;
  }


  private function settingsmenu ()
  {
    $menu = array (
      "users" => array ("manage/users", Locale_Translate::_("Users"), NULL),
      array ("manage/indexing", Locale_Translate::_("Indexing"), NULL),
      array ("administration/language", Locale_Translate::_("Language"), NULL),
      array ("administration/timezone", Locale_Translate::_("Timezone"), NULL),
      "mail" => array ("administration/mail", Locale_Translate::_("Mail"), NULL),
      array ("styles/indext", Locale_Translate::_("Styles"), $this->stylessubmenu ()),
      array ("versification/index", Locale_Translate::_("Versifications"), NULL),
      array ("mapping/index",  Locale_Translate::_("Verse mappings"), NULL),
      "collaboration" => array ("administration/collaboration", Locale_Translate::_("Collaboration"), NULL),
      "client" => array ("administration/client", Locale_Translate::_("Client"), NULL),
      array ("fonts/index", Locale_Translate::_("Fonts"), NULL),
    );
    // If the installation is not prepared for Client mode, disable the menu.
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
    return $menu;
  }


  private function stylessubmenu ()
  {
    $menu = array (
      array ("styles/indexm",  Locale_Translate::_("Manage"), NULL),
    );
    return $menu;
  }


  private function helpmenu ()
  {
    $menu = array (
      array ("help/index", Locale_Translate::_("Help"), NULL),
      array ("administration/phpinfo", Locale_Translate::_("PHPInfo"), NULL),
    );
    return $menu;
  }


  // Create the menu.
  public function create ()
  {
    // Modify the menu based on user access level.
    $mainmenu = $this->accesscontrol ($this->mainmenu ());

    // To create CSS menu the HTML structure needs to be like this:
    //   <ul id="menu" class="menu">
    //     <li>
    //       Menu entry
    //         <li>Subitem</li>
    //       </ul>
    //     </li>
    //     <li>Another entry</li>
    //   </ul>

    $document = new DOMDocument ("1.0", "UTF-8");
    $document->encoding = "UTF-8";
    $document->preserveWhiteSpace = false;
    $document->loadXML ('<ul id="mainmenu" class="menu"></ul>');
    $xpath = new DOMXpath ($document);
    $nodes = $xpath->query ("//ul");
    $mainul = $nodes->item (0);

    // Go through the main menu.
    foreach ($mainmenu as $mainitem) {

      // Build the main menu.
      $mainhref = $mainitem [0];
      $maintext = $mainitem [1];

      $mainli = $document->createElement ("li");
      $mainul->appendChild ($mainli);
      $attribute = $document->createAttribute ("class");
      $mainli->appendChild ($attribute);
      $attribute->value = "toggle";
      if ($mainhref == "") {
        $mainaspan = $document->createElement ("span");
      } else {
        $mainaspan = $document->createElement ("a");
        $attribute = $document->createAttribute ("href");
        $attribute->value = $mainhref;
        $mainaspan->appendChild ($attribute);
      }
      $mainli->appendChild ($mainaspan);
      $mainaspan->nodeValue = $maintext;

      // Build the submenu.
      $submenu = $mainitem [2];
      if (!$submenu) continue;
      $this->submenu ($document, $mainli, $submenu);
    }

    // Get the result.
    $document->formatOutput = true;
    $menu = $document->saveXML ($mainul);
    return $menu;
  }


  private function submenu ($document, $parent, $menu)
  {
    $ul = $document->createElement ("ul");
    $parent->appendChild ($ul);

    foreach ($menu as $item) {

      $href = $item [0];
      $text = $item [1];
      $submenu = $item [2];

      $li = $document->createElement ("li");
      $ul->appendChild ($li);
      $this->subsubmenu ($document, $li, $submenu);
      if ($href == "") {
        $subaspan = $document->createElement ("span");
      } else {
        $subaspan = $document->createElement ("a");
        $attribute = $document->createAttribute ("href");
        $subaspan->appendChild ($attribute);
        $attribute->value = Menu_Logic::href ($href);
      }
      $li->appendChild ($subaspan);
      $subaspan->nodeValue = $text;
    }
  }


  private function subsubmenu ($document, $parent, $menu)
  {
    if (!$menu) return;

    $ul = $document->createElement ("ul");
    $parent->appendChild ($ul);

    foreach ($menu as $item) {

      $href = $item [0];
      $text = $item [1];
      $submenu = $item [2];

      $li = $document->createElement ("li");
      $ul->appendChild ($li);
      $suba = $document->createElement ("a");
      $li->appendChild ($suba);
      $attribute = $document->createAttribute ("href");
      $suba->appendChild ($attribute);
      $attribute->value = Menu_Logic::href ($href);
      $suba->nodeValue = $text;
    }
  }


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


}


?>
