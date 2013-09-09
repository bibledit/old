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


$database_users = Database_Users::getInstance();


// New user creation.
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter a name for the new user"), "", "new", "");
  die;
}
if (isset($_POST['new'])) {
  $user = $_POST['entry'];
  if ($database_users->usernameExists ($user)) {
    Assets_Page::error (gettext ("User already exists"));
  } else {
    include ("session/levels.php");
    $database_users->addNewUser($user, $user, MEMBER_LEVEL, "");
    Assets_Page::success (gettext ("User created"));
  }
}


// Whether to delete a user.
if (isset ($_GET['delete'])) {
  $name = $_GET['delete'];
  @$confirm = $_GET['confirm'];
  if ($confirm == "") {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete") . " $name?", "delete");
    die;
  } else {
    $database_users->removeUser($name);
  }
}


// The user's role.
@$user =  $_GET['user'];
@$level = $_GET['level'];
if (isset ($user) || isset ($level)) {
  if (($user == "") || ($level == "")) {
    $dialog_list = new Dialog_List (NULL, gettext ("Would you like to change the role given to user $user?"), "", "");
    for ($i = GUEST_LEVEL; $i <= ADMIN_LEVEL; $i++) {
      $parameter = "?user=$user&level=$i";
      $dialog_list->add_row ($roles[$i], $parameter);
    }
    $dialog_list->run ();
    die;
  } else {
    $database_users->updateUserLevel($user, $level);
    $database_mail = Database_Mail::getInstance ();
    $database_mail->send ($user, gettext ("Access level change"), gettext ("Your access level was updated"));
  }
}


// User's email address.
if (isset ($_GET['username'])) {
  $dialog_entry = new Dialog_Entry (array ("usernamemail" => $_GET['username']), gettext ("Please enter an email address for the user"), $database_users->getUserToEmail ($_GET['username']), "email", "");
  die;
}
if (isset($_POST['email'])) {
  $email = $_POST['entry'];
  $validator = new Zend_Validate_EmailAddress ();
  if ($validator->isValid ($email)) {
    Assets_Page::success (gettext ("Email address was updated"));
    $database_users->updateUserEmail ($_GET['usernamemail'], $email);
  } else {
    Assets_Page::error (gettext ("The email address is not valid"));
  }
}


Assets_Page::header (gettext ("Users"));
$view = new Assets_View (__FILE__);
$users = Session_Users::getInstance ();
for ($i = 0; $i < count ($users->levels); $i++) {
  $named_roles[] = $roles[$users->levels[$i]];
}
$view->view->usernames = $users->usernames;
$view->view->levels = $named_roles;
$view->view->emails = $users->emails;
$view->render ("users.php");
Assets_Page::footer ();


?>
