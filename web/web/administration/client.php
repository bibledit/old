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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


$database_config_general = Database_Config_General::getInstance ();
$database_users = Database_Users::getInstance ();
$database_noteactions = Database_NoteActions::getInstance ();
$database_bibleactions = Database_BibleActions::getInstance ();


$view = new Assets_View (__FILE__);


if (isset($_GET['disable'])) {
  Filter_Client::set (false);
  remove_all_users ();
  $database_config_general->setRepeatSendReceive (0);
}


if (isset ($_POST ['connect'])) {

  $address = $_POST ['address'];
  $database_config_general->setServerAddress ($address);

  $user = $_POST ['user'];
  $pass = $_POST ['pass'];

  $response = Filter_Client::setup ($user, md5 ($pass));

  if ($response === false) {
    $view->view->error = Locale_Translate::_("Could not connect to the server.");
  } else if (($response >= Filter_Roles::GUEST_LEVEL) && ($response <= Filter_Roles::ADMIN_LEVEL)) {
    // Enable client mode upon a successful connection.
    enable_client ($user, $pass, $response);
    // Feedback.
    $view->view->success = Locale_Translate::_("Connection is okay.");
  } else {
    $view->view->error = $response . ": " . Locale_Translate::_("Check that your username and password exist on the server.");
  }

}


if (isset ($_GET['demo'])) {

  $address = Filter_Demo::demo_address ();
  $database_config_general->setServerAddress ($address);

  $user = "admin";
  $pass = "admin";

  $response = Filter_Client::setup ($user, md5 ($pass));

  if (($response >= Filter_Roles::GUEST_LEVEL) && ($response <= Filter_Roles::ADMIN_LEVEL)) {
    // Enable client mode upon a successful connection.
    enable_client ($user, $pass, $response);
    // Feedback.
    $view->view->success = Locale_Translate::_("Demo connection is okay.");
  } else {
    $view->view->error = Locale_Translate::_("Could not connect to the demo server.");
  }
  
}


$view->view->client = Filter_Client::enabled ();


$address = $database_config_general->getServerAddress ();
$view->view->address = $address;


$users = $database_users->getUsers ();
foreach ($users as $user) {
  $level = $database_users->getUserLevel ($user);
  $view->view->role = Filter_Roles::text ($level);
}


$view->view->demo = Filter_Demo::client_demo_warning ();


Assets_Page::header (Locale_Translate::_("Client mode"));


$view->render ("client.php");


Assets_Page::footer ();


function remove_all_users ()
{
  $database_users = Database_Users::getInstance ();
  $existingusers = $database_users->getUsers ();
  foreach ($existingusers as $existinguser) {
    $database_users->removeUser ($existinguser);
  }
}


function enable_client ($username, $password, $level)
{
  // Enable client mode upon a successful connection.
  Filter_Client::set (true);

  // Remove all users from the database, and add the current one.
  remove_all_users ();
  $database_users = Database_Users::getInstance ();
  $database_users->addNewUser ($username, $password, $level, "");

  // Clear all pending note actions and Bible actions and settings updates.
  $database_noteactions = Database_NoteActions::getInstance ();
  $database_bibleactions = Database_BibleActions::getInstance ();
  $database_config_user = Database_Config_User::getInstance ();
  $session_logic = Session_Logic::getInstance ();
  $database_noteactions->clear ();
  $database_noteactions->create ();
  $database_bibleactions->clear ();
  $database_bibleactions->create ();
  $session_logic->setUsername ($username);
  $database_config_user->setUpdatedSettings (array ());
  
  // Set it repeats sync every so often.
  $database_config_general = Database_Config_General::getInstance ();
  $database_config_general->setRepeatSendReceive (2);
  
  // Schedule a sync operation straight-away.
  SendReceive_Logic::queuesync (true);
}


?>
