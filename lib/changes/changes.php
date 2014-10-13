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


$database_modifications = Database_Modifications::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_logs = Database_Logs::getInstance ();
$session_logic = Session_Logic::getInstance ();


$username = $session_logic->currentUser ();


// Handle AJAX call to remove a change notification.
@$remove = $_POST['remove'];
if (isset ($remove)) {
  $trash_handler = Trash_Handler::getInstance ();
  $trash_handler->changeNotification ($remove);
  $database_modifications->deleteNotification ($remove);
  die;
}


// Handle AJAX call to navigate to the passage belonging to the change notification.
@$navigate = $_POST['navigate'];
if (isset ($navigate)) {
  $id = $navigate;
  $passage = $database_modifications->getNotificationPassage ($id);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
    Navigation_Passage::recordHistory ($passage['book'], $passage['chapter'], $passage['verse']);
  }
  // Set the correct default Bible for the user.
  $database_config_user = Database_Config_User::getInstance ();
  $bible = $database_modifications->getNotificationBible ($id);
  if ($bible) $database_config_user->setBible ($bible);
  die;
}


// Remove personal change proposals and their matching change notifications.
if (isset ($_GET ['match'])) {
  $database_modifications->clearNotificationMatches ($username, "☺", "♺");
}


$header = new Assets_Header (Locale_Translate::_("Changes"));
$header->setStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);


// Read the identifiers but limit the number of results.
$ids = array ();
@$filter = $_GET ['filter'];
if ($filter == "personal") {
  $ids = $database_modifications->getNotificationPersonalIdentifiers ($username, "☺", true);
  $view->view->filter = 1;
} else if ($filter == "team") {
  $ids = $database_modifications->getNotificationTeamIdentifiers ($username, "♺", true);
  $view->view->filter = 2;
} else {
  $ids = $database_modifications->getNotificationIdentifiers ($username, true);
  $view->view->filter = 0;
}
$view->view->ids = $ids;


$links = array ();
$categories = array ();
$modifications = array ();
foreach ($ids as $id) {
  $passage = $database_modifications->getNotificationPassage ($id);
  $link = Filter_Books::linkForOpeningEditorAt ($passage['book'], $passage['chapter'], $passage['verse']);
  $links [] = $link;
  $category = $database_modifications->getNotificationCategory ($id);
  $category = Filter_Html::sanitize ($category);
  $categories [] = $category;
  $modification = $database_modifications->getNotificationModification ($id);
  $modifications [] = $modification;
}
$view->view->links = $links;
$view->view->categories = $categories;
$view->view->modifications = $modifications;


$loading = '"' . Locale_Translate::_("Loading ...") . '"';


$script = <<<EOD
var loading = $loading;
EOD;
$view->view->script = $script;


$view->render ("changes.php");
Assets_Page::footer ();


?>
