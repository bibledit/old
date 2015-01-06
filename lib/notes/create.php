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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::consultant ());


$database_notes = Database_Notes::getInstance();
$database_config_user = Database_Config_User::getInstance();
$notes_logic = Notes_Logic::getInstance();
$ipc_focus = Ipc_Focus::getInstance();


// Is is possible to pass a Bible to this script.
// The note will then be created for this Bible.
// If no Bible is passed, it takes the user's active Bible.
@$bible = request->post ['bible'];
if ($bible == "") {
  $bible = access_bible_clamp ($database_config_user->getBible ());
}


@$book = request->post ['book'];
if ($book == "") {
  $book = $ipc_focus->getBook ();
}
@$chapter = request->post ['chapter'];
if ($chapter == "") {
  $chapter = $ipc_focus->getChapter ();
}
@$verse = request->post ['verse'];
if ($verse == "") {
  $verse = $ipc_focus->getVerse ();
}


if (isset(request->post['submit'])) {
  $summary = filter_string_trim (request->post['summary']);
  $contents = filter_string_trim (request->post['contents']);
  $notes_logic->createNote ($bible, $book, $chapter, $verse, $summary, $contents, false);
  redirect_browser ("index.php");
  die;
}


if (isset(request->post['cancel'])) {
  redirect_browser ("index.php");
  die;
}


$assets_header = new Assets_Header (gettext("Create note"));
$assets_header->run();


$view = new Assets_View (__FILE__);


// This script can be called from a change notification.
// It will then create a note based on that change notification.
@$fromchange = request->query ['fromchange'];
if (isset ($fromchange)) {
  $database_modifications = Database_Modifications::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  $bible = $database_modifications->getNotificationBible ($fromchange);
  $summary = gettext("Query about a change in the text");
  $contents = "<p>" . gettext("Old text:") . "</p>";
  $contents += $database_modifications->getNotificationOldText ($fromchange);
  $contents += "<p>" .  gettext("Change:") . "</p>";
  $contents += "<p>" . $database_modifications->getNotificationModification ($fromchange) . "</p>";
  $contents += "<p>" . gettext("New text:") . "</p>";
  $contents += $database_modifications->getNotificationNewText ($fromchange);
  $view->view->summary = $summary;
  $view->view->contents = $contents;
}


$view->view->bible = $bible;
$view->view->book = $book;
$view->view->chapter = $chapter;
$view->view->verse = $verse;
$passage = filter_passage_display ($book, $chapter, $verse);
$view->view->passage = $passage;


$view->render ("create.php");


Assets_Page::footer ();
?>
