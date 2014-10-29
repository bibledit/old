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
$ipc_focus = Ipc_Focus::getInstance();


$id = $_GET ['id'];


// When a note is opened, then the passage navigator should go to the passage that belongs to that note.
$passages = $database_notes->getPassages ($id);
if (is_array ($passages)) {
  if (!empty ($passages)) {
    $ipc_focus->set ($passages[0][0], $passages[0][1], $passages[0][2]);
    Navigation_Passage::recordHistory ($passages[0][0], $passages[0][1], $passages[0][2]);
  }
}


$header = new Assets_Header (Locale_Translate::_("Note"));
// After adding a comment to a note it returns to the note.
// When doing nothing for several seconds, the browser then returns to the list of notes.
if (isset ($_GET ['temporal'])) {
  $header->addHeadLine ('<META HTTP-EQUIV="refresh" CONTENT="5;URL=index.php">');
}
$header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$summary = $database_notes->getSummary ($id);
$view->view->summary = $summary;


$content = $database_notes->getContents ($id);
$view->view->content = $content;


$view->render ("note.php");


Assets_Page::footer ();


?>
