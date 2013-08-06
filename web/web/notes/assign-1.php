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


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


$database_notes = Database_Notes::getInstance();
$notes_logic = Notes_Logic::getInstance();
$database_users = Database_Users::getInstance();


$id = $_GET ['id'];


@$assign = $_GET['assign'];
if (isset ($assign)) {
  if ($database_users->usernameExists ($assign)) {
    // Assign logic comes first.
    $notes_logic->handlerAssignNote ($id, $assign);
    $database_notes->assignUser ($id, $assign);
  }
  header ("Location: actions.php?id=$id");
  die;
}


$assets_header = new Assets_Header (gettext ("Assign note"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$users = $database_users->getUsers ();
$view->view->users = $users;


$view->render ("assign-1.php");


Assets_Page::footer ();


?>
