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


Assets_Page::header (Locale_Translate::_("Users"));


$database_users = Database_Users::getInstance();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$session_logic = Session_Logic::getInstance ();


$currentUser = $session_logic->currentUser ();
$currentLevel = $session_logic->currentLevel ();


// New user creation.
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter a name for the new user"), "", "new", "");
  die;
}
if (isset($_POST['new'])) {
  $user = $_POST['entry'];
  if ($database_users->usernameExists ($user)) {
    Assets_Page::error (Locale_Translate::_("User already exists"));
  } else {
    $database_users->addNewUser($user, $user, Filter_Roles::MEMBER_LEVEL, "");
    Assets_Page::success (Locale_Translate::_("User created"));
  }
}


// The username to act on.
@$user =  $_GET['user'];


// Delete a user.
if (isset ($_GET['delete'])) {
  $level = $database_users->getUserLevel ($user);
  $role = Filter_Roles::text ($level);
  $email = $database_users->getUserToEmail ($user);
  $message = "Deleted user $user with role $role and email $email";
  $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
  $database_users->removeUser($user);
  Assets_Page::success ($message);
}


// The user's role.
@$level = $_GET['level'];
if (isset ($level)) {
  if ($level == "") {
    $dialog_list = new Dialog_List (NULL, Locale_Translate::_("Would you like to change the role given to user $user?"), "", "");
    for ($i = Filter_Roles::lowest (); $i <= Filter_Roles::highest (); $i++) {
      if ($i <= $currentLevel) {
        $parameter = "?user=$user&level=$i";
        $dialog_list->add_row (Filter_Roles::text ($i), $parameter);
      }
    }
    $dialog_list->run ();
    die;
  } else {
    $database_users->updateUserLevel($user, $level);
  }
}


// User's email address.
@$email = $_GET ['email'];
if (isset ($email)) {
  if ($email == "") {
    $dialog_entry = new Dialog_Entry (array ("usernamemail" => $_GET['user']), Locale_Translate::_("Please enter an email address for the user"), $database_users->getUserToEmail ($user), "email", "");
    die;
  }
}
if (isset($_POST['email'])) {
  $email = $_POST['entry'];
  $validator = new Zend_Validate_EmailAddress ();
  if ($validator->isValid ($email)) {
    Assets_Page::success (Locale_Translate::_("Email address was updated"));
    $database_users->updateUserEmail ($_GET['usernamemail'], $email);
  } else {
    Assets_Page::error (Locale_Translate::_("The email address is not valid"));
  }
}


// Fetch the Bibles the current user has access to.
$accessibleBibles = $database_bibles->getBibles ();
foreach ($accessibleBibles as $offset => $bible) {
  if (!$database_users->hasAccess2Bible ($currentUser, $bible)) {
    unset ($accessibleBibles [$offset]);
  }
}
$accessibleBibles = array_values ($accessibleBibles);
sort ($accessibleBibles);


// Add Bible to user account.
@$addbible = $_GET['addbible'];
if (isset ($addbible)) {
  if ($addbible == "") {
    $dialog_list = new Dialog_List (NULL, Locale_Translate::_("Would you like to grant user $user access to a Bible?"), "", "");
    foreach ($accessibleBibles as $bible) {
      $parameter = "?user=$user&addbible=$bible";
      $dialog_list->add_row ($bible, $parameter);
    }
    $dialog_list->run ();
    die;
  } else {
    Assets_Page::success (Locale_Translate::_("The user has become a member of the translation team that works on this Bible"));
    $database_users->grantAccess2Bible ($user, $addbible);
  }
}


// Remove Bible from user.
@$removebible = $_GET['removebible'];
if (isset ($removebible)) {
  $database_users->revokeAccess2Bible ($user, $removebible);
  Assets_Page::success (Locale_Translate::_("The user is no longer a member of the translation team that works on this Bible"));
}


// Toggle readonly access to Bible.
@$readonlytoggle = $_GET ['readonlytoggle'];
if (isset ($readonlytoggle)) {
  $readonly = $database_users->hasReadOnlyAccess2Bible ($user, $readonlytoggle);
  $database_users->setReadOnlyAccess2Bible ($user, $readonlytoggle, !$readonly);
}


// The level and Bibles of the user who works on this page.
// The admin has access to all Bibles.
$mylevel = $session_logic->currentLevel ();
$mybibles = $database_users->getBibles4User ($currentUser);
if ($mylevel >= Filter_Roles::ADMIN_LEVEL) {
  $mybibles = $database_bibles->getBibles ();
}


// User accounts to display.
$usernames = array ();
$named_roles = array ();
$emails = array ();
$bibles = array ();


// Retrieve assigned users.
$users = Access_User::assignees ();
foreach ($users as $user) {
  $level = $database_users->getUserLevel ($user);
  $userBibles = $database_users->getBibles4User ($user);
  // Gather details for this user account.
  $usernames [] = $user;
  $named_roles [] = Filter_Roles::text ($level);
  $email = $database_users->getUserToEmail ($user);
  if ($email == "") $email = "--";
  $emails [] = $email;
  // List no more than those Bibles the currently logged in user has access to.
  // So the currently logged-in user cannot remove any Bibles he has no access to.
  $userBibles = array_intersect ($userBibles, $accessibleBibles);
  $bibleData = array ();
  foreach ($userBibles as $bible) {
    $readonly = $database_users->hasReadOnlyAccess2Bible ($user, $bible);
    $bible = array ('bible' => $bible, 
                    'writer' => ($level >= Filter_Roles::TRANSLATOR_LEVEL),
                    'readonly' => $readonly);
    $bibleData [] = $bible;
  }
  $bibles [] = $bibleData;
}


$view = new Assets_View (__FILE__);
$view->view->usernames = $usernames;
$view->view->levels = $named_roles;
$view->view->emails = $emails;
$view->view->bibles = $bibles;
$view->render ("users.php");


Assets_Page::footer ();


?>
