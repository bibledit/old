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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MANAGER_LEVEL);


Assets_Page::header (Locale_Translate::_("Verse mappings"));
$view = new Assets_View (__FILE__);


$database_mappings = Database_Mappings::getInstance ();
$session_logic = Session_Logic::getInstance ();


$username = $session_logic->currentUser ();
$userlevel = $session_logic->currentLevel ();


$name = $_GET['name'];
$view->view->name = Filter_Html::sanitize ($name);


//$write = $database_styles->hasWriteAccess ($username, $name);
$write = true;
if ($userlevel >= Filter_Roles::ADMIN_LEVEL) $write = true;


if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  if ($write) {
    $database_mappings->import ($name, $data);
  }
}


$data = $database_mappings->export ($name);
$view->view->data = $data;


$view->render ("map.php");


Assets_Page::footer ();


?>
