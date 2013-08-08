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


$menu = array (
"notes/index" => gettext ("Notes"),
"consultations/changes" => gettext ("Changes"),
"resource/index" => gettext ("Resources"),
"translate/index" => gettext ("Translation"),
"manage/index" => gettext ("Management"),
"administration/index" => gettext ("Administration"),
"members/index" => gettext ("Members"),
"help/index" => gettext ("Help"),
0 => 0);


$database_menu = Database_Menu::getInstance ();
$session_logic = Session_Logic::getInstance ();


$user = $session_logic->currentUser ();


@$url = $_GET ['url'];
if (isset ($url)) {
  $database_menu->increaseAccessCount ($user, $url);
  header ("Location: ../$url.php");
  die;
}


array_pop ($menu);


Assets_Page::header (gettext ("Start"));
$view = new Assets_View (__FILE__);


$user_level = $session_logic->currentLevel ();
include ("session/levels.php");


$hits = array ();
foreach ($menu as $url => $text) {
  $contents = file_get_contents ("../$url.php");
  $menu_level = 0;
  if (strpos ($contents, "GUEST_LEVEL")      !== false) $menu_level = GUEST_LEVEL;
  if (strpos ($contents, "MEMBER_LEVEL")     !== false) $menu_level = MEMBER_LEVEL;
  if (strpos ($contents, "CONSULTANT_LEVEL") !== false) $menu_level = CONSULTANT_LEVEL;
  if (strpos ($contents, "TRANSLATOR_LEVEL") !== false) $menu_level = TRANSLATOR_LEVEL;
  if (strpos ($contents, "MANAGER_LEVEL")    !== false) $menu_level = MANAGER_LEVEL;
  if (strpos ($contents, "ADMIN_LEVEL")      !== false) $menu_level = ADMIN_LEVEL;
  if ($menu_level > $user_level) continue;
  $count = $database_menu->getAccessCount ($user, $url);
  $hits [$url] = $count;
}


arsort ($hits, SORT_NUMERIC);


$newmenu = array ();
foreach ($hits as $url => $count) {
  $newmenu [$url] = $menu [$url];
}


$view->view->menu = $newmenu;


$view->render ("index.php");
Assets_Page::footer ();


?>
