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


$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance ();


$id = $_GET ['id'];


if (isset($_POST['submit'])) {
  $verses = explode ("\n", $_POST['verses']);
  $passages = array ();
  $previousPassage = array (1, 1, 1);
  foreach ($verses as $line) {
    $line = trim ($line);
    if ($line != "") {
      $passage = Filter_Books::interpretPassage ($previousPassage, $line);
      if ($passage[0] != 0) {
        $passages [] = $passage;
        $previousPassage = $passage;
      }
    }
  }
  $notes_logic->setPassages ($id, $passages);
  Filter_Url::redirect ("actions.php?id=$id");
  die;
}


$assets_header = new Assets_Header (Locale_Translate::_("Note passages"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view->view->id = $id;


$verses = Filter_Books::passagesDisplayMultiline ($database_notes->getPassages ($id));
$view->view->verses = $verses;


$view->render ("verses.php");


Assets_Page::footer ();


?>
