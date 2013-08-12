<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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


/*
This script generates a "Start" menu.

It is based on an array of all possible menu entries.
It reads the access levels of those entries.
It only keeps those menu entries the currently logged-in user has access to.

It sorts the entries such that the ones used most often come earlier in the menu.

*/


require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);


// This array has the full start menu.
// Every entry has a unique key.
// This key is used to track usage statistics.
// The values consist of an array with paths to the file => localized menu text.
$fullmenu = array (
"translate" => array (
  "editusfm/index" => gettext ("Translate")
),
"notes" => array (
  "notes/index" => gettext ("Notes"),
  "notes/editsource" => "[" . gettext ("edit") . "]",
  "notes/duplicates" => "[" . gettext ("duplicates") . "]",
  "notes/import1" => "[" . gettext ("import") . "]"
),
"changes" => array (
  "downloads/changes" => gettext ("Changes"),
  "consultations/changes" => "[" . gettext ("notifications") . "]",
  "manage/changes" => "[" . gettext ("manage") . "]"
),
"resources" => array (
  "resource/index" => gettext ("Resources"),
  "resource/manage" => "[" . gettext ("manage") . "]",
  "resource/admin" => "[" . gettext ("admin") . "]"
),
"checks" => array (
  "checks/index" => gettext ("Checks"),
  "checks/settings" => "[" . gettext ("manage") . "]"
),
"exports" => array (
  "downloads/exports" => gettext ("Exports"),
  "manage/exports" => "[" . gettext ("manage") . "]"
),
"styles" => array (
  "styles/indext" => gettext ("Styles"),
  "styles/indexm" => "[" . gettext ("manage") . "]"
),
"management" => array (
  "manage/sendreceive" => gettext ("Receive/Send"),
  "manage/logbook" => gettext ("Logbook"),
  "manage/users" => gettext ("Users"),
  "bible/manage" => gettext ("Bibles"),
  "versification/index" => gettext ("Versifications"),
  "manage/hyphenation" => gettext ("Hyphenation")
),
"administration" => array (
  "administration/collaboration" => gettext ("Collaboration"),
  "administration/mail" => gettext ("Mail"),
  "administration/backup" => gettext ("Backup/Restore"),
  "administration/timezone" => gettext ("Timezone"),
  "administration/language" => gettext ("Language"),
  "administration/phpinfo" => gettext ("PHPInfo")
),
"help" => array (
  "help/index" => gettext ("Help")
)
);


$database_menu = Database_Menu::getInstance ();
$session_logic = Session_Logic::getInstance ();


$user = $session_logic->currentUser ();


@$url = $_GET ['url'];
if (isset ($url)) {
  $category = $_GET ['category'];
  $database_menu->increaseAccessCount ($user, $category);
  if (strpos ($url, "downloads") === false) $url .= ".php";
  header ("Location: ../$url");
  die;
}


// Sort the full menu based on usage: Most often used entries come first.
$hits = array ();
foreach ($fullmenu as $category => $menu) {
  $count = $database_menu->getAccessCount ($user, $category);
  $hits [$category] = $count;
}
arsort ($hits, SORT_NUMERIC);


$user_level = $session_logic->currentLevel ();
include ("session/levels.php");


// Take the full menu, and remove the menu the items the user has no permission for.
// This produces a menu specific to the user.
$usermenu = array ();
foreach ($hits as $category => $count) {
  $usersubmenu = array ();
  $submenu = $fullmenu [$category];
  foreach ($submenu as $url => $text) {
    @$contents = file_get_contents ("../$url.php");
    $menu_level = MEMBER_LEVEL;
    if (strpos ($contents, "GUEST_LEVEL")      !== false) $menu_level = GUEST_LEVEL;
    if (strpos ($contents, "MEMBER_LEVEL")     !== false) $menu_level = MEMBER_LEVEL;
    if (strpos ($contents, "CONSULTANT_LEVEL") !== false) $menu_level = CONSULTANT_LEVEL;
    if (strpos ($contents, "TRANSLATOR_LEVEL") !== false) $menu_level = TRANSLATOR_LEVEL;
    if (strpos ($contents, "MANAGER_LEVEL")    !== false) $menu_level = MANAGER_LEVEL;
    if (strpos ($contents, "ADMIN_LEVEL")      !== false) $menu_level = ADMIN_LEVEL;
    if ($menu_level > $user_level) continue;
    $usersubmenu [$url] = $text;
  }
  $usermenu [$category] = $usersubmenu;
}


Assets_Page::header (gettext ("Start"));
$view = new Assets_View (__FILE__);


$view->view->menu = $usermenu;


$view->render ("index.php");


Assets_Page::footer ();


?>
