<?php
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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::manager ());


$database_notes = Database_Notes::getInstance();
$notes_logic = Notes_Logic::getInstance();
$database_users = Database_Users::getInstance();


$id = request->query ['id'];


@$assign = request->query['assign'];
if (isset ($assign)) {
  if ($database_users->usernameExists ($assign)) {
    $notes_logic->assignUser ($id, $assign);
  }
  redirect_browser ("actions?id=$id");
  die;
}


$assets_header = new Assets_Header (gettext("Assign note"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


// Notes can be assigned to users who have access to the Bibles the currently logged-in user has access to.
$bibles = access_bible_bibles ();
$users = $database_users->getUsers ();
for ($users as $offset => $user) {
  $access = false;
  for ($bibles as $bible) {
    if (!$access) {
      $access = $database_users->hasAccess2Bible ($user, $bible);
    }
  }
  if (!$access) unset ($users [$offset]);
}
$view->view->users = $users;


$view->render ("assign-1");


Assets_Page::footer ();


?>
