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
This generates a user menu.

It is based on arrays of all possible menu entries.

It reads the access levels of those entries.
It removes the menu entries the currently logged-in user has no access to, and keeps the rest.

It also considers whether any user is logged in at all.
It updates the menu accordingly.

*/


class Menu_User
{


  private function mainmenu ()
  {
    // This is the main menu.
    // It will be visible in the top bar.
    // The last element in the array is the submenu to display on expansion.
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    $menu = array (
      array ("", $username, $this->usermenu ()),
    );
    return $menu;
  }


  private function usermenu ()
  {
    $menu = array (
      array ("session/logout", Locale_Translate::_("Logout"), NULL),
      array ("user/notifications", Locale_Translate::_("Notifications"), NULL),
      array ("user/account", Locale_Translate::_("Account"), NULL),
    );
    return $menu;
  }


  // Create the menu.
  public function create ()
  {
    // No user menu in client mode.
     if (Filter_Client::enabled ()) return "";
    
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
    $document->loadXML ('<ul id="usermenu" class="menu"></ul>');
    $xpath = new DOMXpath ($document);
    $nodes = $xpath->query ("//ul");
    $mainul = $nodes->item (0);

    // Go through the main menu.
    foreach ($mainmenu as $mainitem) {

      // Build the main menu.
      $mainhref = $mainitem [0];
      $maintext = $mainitem [1];

      $mainli = $document->createElement ("li");
      $attribute = $document->createAttribute ("class");
      $mainli->appendChild ($attribute);
      $attribute->value = "toggle";
      $mainul->appendChild ($mainli);
      if ($mainhref == "") {
        $mainaspan = $document->createElement ("span");
      } else {
        $mainaspan = $document->createElement ("a");
        $attribute = $document->createAttribute ("href");
        $mainaspan->appendChild ($attribute);
        $attribute->value = $mainhref;
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
      }
      $li->appendChild ($subaspan);
      $attribute->value = Menu_Logic::href ($href);
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
