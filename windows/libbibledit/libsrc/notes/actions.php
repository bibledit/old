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


$database_notes = Database_Notes::getInstance();
$database_config_user = Database_Config_User::getInstance();
$notes_logic = Notes_Logic::getInstance();
$session_logic = Session_Logic::getInstance();
$trash_handler = Trash_Handler::getInstance ();


$user = $session_logic->currentUser ();


$success = "";
$error = "";


@$id = $_GET ['id'];
if ($id == "") {
  @$id = $_POST ['id'];
}


if (isset ($_GET['unsubscribe'])) {
  $notes_logic->unsubscribe ($id);
}


if (isset ($_GET['subscribe'])) {
  $notes_logic->subscribe ($id);
}


@$unassign = $_GET['unassign'];
if (isset ($unassign)) {
  $notes_logic->unassignUser ($id, $unassign);
}


if (isset ($_GET['done'])) {
  $notes_logic->unassignUser ($id, $user);
}


if (isset ($_GET['markdel'])) {
  $notes_logic->markForDeletion ($id);
  $success = Locale_Translate::_("The note will be deleted after a week.") . " " . Locale_Translate::_ ("Adding a comment to the note cancels the deletion.");
}


if (isset ($_GET['unmarkdel'])) {
  $notes_logic->unmarkForDeletion ($id);
}


if (isset ($_GET['delete'])) {
  $notes_logic->delete ($id);
  Filter_Url::redirect ("index.php");
  die;
}


$assets_header = new Assets_Header (Locale_Translate::_("Note actions"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$summary = $database_notes->getSummary ($id);
$view->view->summary = $summary;


$subscribed = $database_notes->isSubscribed ($id, $user);
$view->view->subscribed = $subscribed;


$assignees = $database_notes->getAssignees ($id);
$view->view->assignees = $assignees;


$assignee = $database_notes->isAssigned ($id, $user);
$view->view->assignee = $assignee;


$status = $database_notes->getStatus ($id);
$view->view->status = $status;


$verses = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($id));
$view->view->verses = $verses;


$severity = $database_notes->getSeverity ($id);
$view->view->severity = $severity;


$bible = $database_notes->getBible ($id);
$view->view->bible = $bible;


$marked = $database_notes->isMarkedForDeletion ($id);
$view->view->marked = $marked;


$view->view->success = $success;
$view->view->error = $error;


$view->render ("actions.php");


Assets_Page::footer ();
?>
