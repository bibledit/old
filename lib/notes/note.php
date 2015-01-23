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


$database_notes = Database_Notes::getInstance();



$id = request->query ['id'];


// When a note is opened, then the passage navigator should go to the passage that belongs to that note.
$passages = $database_notes->getPassages ($id);
if (is_array ($passages)) {
  if (!empty ($passages)) {
    Ipc_Focus::set ($passages[0][0], $passages[0][1], $passages[0][2]);
    Navigation_Passage::recordHistory ($passages[0][0], $passages[0][1], $passages[0][2]);
  }
}


$header = new Assets_Header (gettext("Note"));
// After adding a comment to a note it returns to the note.
// When doing nothing for several seconds, the browser then returns to the list of notes.
if (isset (request->query ['temporal'])) {
  $header->addHeadLine ('<META HTTP-EQUIV="refresh" CONTENT="5;URL=index">'); // This can use existing function, expanded with the URL: Function refresh.
}
$header->run();


$view = new Assets_View (__FILE__);


$view.set_variable ("id = $id;


$summary = $database_notes->getSummary ($id);
$view.set_variable ("summary = $summary;


$content = $database_notes->getContents ($id);
$view.set_variable ("content = $content;


$view->render ("note");


Assets_Page::footer ();


?>
