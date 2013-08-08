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


$database_notes = Database_Notes::getInstance();
$database_config_user = Database_Config_User::getInstance();
$notes_logic = Notes_Logic::getInstance();
$ipc_focus = Ipc_Focus::getInstance();


// Is is possible to pass a Bible to this script.
// The note will then be created for this Bible.
// If no Bible is passed, it takes the user's active Bible.
@$bible = $_POST ['bible'];
if ($bible == "") {
  $bible = $database_config_user->getBible ();
}


@$book = $_POST ['book'];
if ($book == "") {
  $book = $ipc_focus->getBook ();
}
@$chapter = $_POST ['chapter'];
if ($chapter == "") {
  $chapter = $ipc_focus->getChapter ();
}
@$verse = $_POST ['verse'];
if ($verse == "") {
  $verse = $ipc_focus->getVerse ();
}


if (isset($_POST['submit'])) {
  $summary = trim ($_POST['summary']);
  $contents = trim ($_POST['contents']);
  $consultationnote = $database_notes->storeNewNote ($bible, $book, $chapter, $verse, $summary, $contents, false);
  $notes_logic->handlerNewNote ($consultationnote);
  header ("Location: index.php");
  die;
}


if (isset($_POST['cancel'])) {
  header ("Location: index.php");
  die;
}


$assets_header = new Assets_Header (gettext ("Create note"));
$assets_header->jQueryOn ();
$assets_header->run();


$view = new Assets_View (__FILE__);


// This script can be called from a change notification.
// It will then create a note based on that change notification.
@$fromchange = $_GET ['fromchange'];
if (isset ($fromchange)) {
  $database_changes = Database_Changes::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  $bible = $database_changes->getBible ($fromchange);
  $bible = $database_bibles->getName ($bible);
  $summary = gettext ("Query about a change in the text");
  $contents = "<p>" . gettext ("Old text:") . "</p>";
  $contents .= $database_changes->getOldText ($fromchange);
  $contents .= "<p>" .  gettext ("Change:") . "</p>";
  $contents .= "<p>" . $database_changes->getModification ($fromchange) . "</p>";
  $contents .= "<p>" . gettext ("New text:") . "</p>";
  $contents .= $database_changes->getNewText ($fromchange);
  $view->view->summary = $summary;
  $view->view->contents = $contents;
}


$view->view->bible = $bible;
$view->view->book = $book;
$view->view->chapter = $chapter;
$view->view->verse = $verse;
$passage = Filter_Books::passageDisplay ($book, $chapter, $verse);
$view->view->passage = $passage;


$view->render ("create.php");


Assets_Page::footer ();
?>
