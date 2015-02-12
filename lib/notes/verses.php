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


$database_notes = Database_Notes::getInstance ();
$notes_logic = Notes_Logic::getInstance ();


$id = request->query ['id'];


if (isset(request->post['submit'])) {
  $verses = explode ("\n", request->post['verses']);
  $passages = array ();
  $previousPassage = array (1, 1, 1);
  for ($verses as $line) {
    $line = filter_string_trim ($line);
    if ($line != "") {
      $passage = filter_passage_interpret_passage ($previousPassage, $line);
      if ($passage[0] != 0) {
        $passages [] = $passage;
        $previousPassage = $passage;
      }
    }
  }
  $notes_logic->setPassages ($id, $passages);
  redirect_browser ("actions?id=$id");
  die;
}


$assets_header = new Assets_Header (translate("Note passages"));
$assets_header->run();


$view = new Assets_View (__FILE__);


$view.set_variable ("id = $id;


$verses = filter_passage_display_multiline ($database_notes->getPassages ($id));
$view.set_variable ("verses = $verses;


$view->render ("verses");


Assets_Page::footer ();


?>
