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
page_access_level (CONSULTANT_LEVEL);


$database_changes = Database_Changes::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_logs = Database_Logs::getInstance ();
$session_logic = Session_Logic::getInstance ();


$username = $session_logic->currentUser ();


// Handle AJAX call to remove a change notification.
@$remove = $_POST['remove'];
if (isset ($remove)) {
  $trash_handler = Trash_Handler::getInstance ();
  $trash_handler->changeNotification ($remove);
  $database_changes->delete ($remove);
  die;
}


// Handle AJAX call to navigate to the passage belonging to the change notification.
@$navigate = $_POST['navigate'];
if (isset ($navigate)) {
  $id = $navigate;
  $passage = $database_changes->getPassage ($id);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
    Navigation_Logic::recordHistory ($passage['book'], $passage['chapter'], $passage['verse']);
  }
  // Set the correct default Bible for the user.
  $database_config_user = Database_Config_User::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  $bible = $database_changes->getBible ($id);
  $bible = $database_bibles->getName ($bible);
  $database_config_user->setBible ($bible);
  die;
}


$header = new Assets_Header (gettext ("Changes"));
$header->jQueryOn ();
$header->run ();


$view = new Assets_View (__FILE__);


$ids = $database_changes->getIdentifiers ($username);
$view->view->ids = $ids;
  

$passages = array ();
$categories = array ();
$modifications = array ();
foreach ($ids as $id) {
  $passage = $database_changes->getPassage ($id);
  $passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
  $passageText = Filter_Html::sanitize ($passageText);
  $passages [] = $passageText;
  $category = $database_changes->getCategory ($id);
  $category = Filter_Html::sanitize ($category);
  $categories [] = $category;
  $modification = $database_changes->getModification ($id);
  $modifications [] = $modification;
}
$view->view->passages = $passages;
$view->view->categories = $categories;
$view->view->modifications = $modifications;


$loading = '"' . gettext ("Loading ...") . '"';


$script = <<<EOD
var loading = $loading;
EOD;
$view->view->script = $script;


$view->render ("changes.php");
Assets_Page::footer ();


?>
