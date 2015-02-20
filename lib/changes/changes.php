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
page_access_level (Filter_Roles::consultant ());


$database_modifications = Database_Modifications::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_logs = Database_Logs::getInstance ();
$session_logic = Session_Logic::getInstance ();


$username = request->session_logic()->currentUser ();


// Handle AJAX call to remove a change notification.
@$remove = request->post['remove'];
if (isset ($remove)) {
  $trash_handler = Trash_Handler::getInstance ();
  $trash_handler->changeNotification ($remove);
  $database_modifications->deleteNotification ($remove);
  die;
}


// Handle AJAX call to navigate to the passage belonging to the change notification.
@$navigate = request->post['navigate'];
if (isset ($navigate)) {
  $id = $navigate;
  $passage = $database_modifications->getNotificationPassage ($id);
  if ($passage != NULL) {
    
    Ipc_Focus::set ($passage['book'], $passage['chapter'], $passage['verse']);
    Navigation_Passage::recordHistory ($passage['book'], $passage['chapter'], $passage['verse']);
  }
  // Set the correct default Bible for the user.
  $database_config_user = Database_Config_User::getInstance ();
  $bible = $database_modifications->getNotificationBible ($id);
  if ($bible) request->database_config_user()->setBible ($bible);
  die;
}


// Remove personal change proposals and their matching change notifications.
if (isset (request->query ['match'])) {
  $database_modifications->clearNotificationMatches ($username, "☺", "♺");
}


$header = new Assets_Header (translate("Changes"));
$header->setStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);


// Read the identifiers but limit the number of results.
$ids = array ();
@$filter = request->query ['filter'];
if ($filter == "personal") {
  $ids = $database_modifications->getNotificationPersonalIdentifiers ($username, "☺", true);
  $view.set_variable ("filter = 1;
} else if ($filter == "team") {
  $ids = $database_modifications->getNotificationTeamIdentifiers ($username, "♺", true);
  $view.set_variable ("filter = 2;
} else {
  $ids = $database_modifications->getNotificationIdentifiers ($username, true);
  $view.set_variable ("filter = 0;
}
$view.set_variable ("ids = $ids;


$links = array ();
$categories = array ();
$modifications = array ();
for ($ids as $id) {
  $passage = $database_modifications->getNotificationPassage ($id);
  $link = filter_passage_link_for_opening_editor_at ($passage['book'], $passage['chapter'], $passage['verse']);
  $links [] = $link;
  $category = $database_modifications->getNotificationCategory ($id);
  $category = filter_string_sanitize_html ($category);
  $categories [] = $category;
  $modification = $database_modifications->getNotificationModification ($id);
  $modifications [] = $modification;
}
$view.set_variable ("links = $links;
$view.set_variable ("categories = $categories;
$view.set_variable ("modifications = $modifications;


$loading = '"' . translate("Loading ...") . '"';


$script = <<<EOD
var loading = $loading;
EOD;
$view.set_variable ("script = $script;


$view->render ("changes");
Assets_Page::footer ();


?>
