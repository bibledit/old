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


Assets_Page::header (Locale_Translate::_("Hyphenation"));
$view = new Assets_View (__FILE__);


$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bible = Access_Bible::clamp ($database_config_user->getBible ());


$success = "";
$error = "";


// Character sets submission.
if (isset($_POST['sets'])) {
  $firstset = $_POST['firstset'];
  $database_config_bible->setHyphenationFirstSet ($bible, $firstset);
  $secondset = $_POST['secondset'];
  $database_config_bible->setHyphenationSecondSet ($bible, $secondset);
  $success = Locale_Translate::_("The two sets of characters were saved");
}
$firstset = $database_config_bible->getHyphenationFirstSet ($bible);
$secondset = $database_config_bible->getHyphenationSecondSet ($bible);


@$bible = $_GET ['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Which Bible would you like to take the data from?"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $item) {
      $dialog_list->add_row ($item, "bible=$item");
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}
$bible = Access_Bible::clamp ($database_config_user->getBible ());


if (isset($_GET['run'])) {
  if ($bible == "") {
    $error = Locale_Translate::_("No Bible given");
  } else if ($firstset == "") {
    $error = Locale_Translate::_("No first set of characters given");
  } else if ($secondset == "") {
    $error = Locale_Translate::_("No second set of characters given");
  } else {
    $workingdirectory = __DIR__;
    Tasks_Logic::queue (Tasks_Logic::PHP, array ("$workingdirectory/hyphenate.php", $bible, $session_logic->currentUser ()));
    $success = Locale_Translate::_("The Bible is being hyphenated. See the Journal for details.");
  }
}


$view->view->firstset = $firstset;
$view->view->secondset = $secondset;
$view->view->bible = $bible;
$view->view->success = $success;
$view->view->error = $error;
$view->render ("hyphenation.php");


Assets_Page::footer ();


?>
