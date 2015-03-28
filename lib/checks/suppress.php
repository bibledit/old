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
page_access_level (Filter_Roles::translator ());


$database_check = Database_Check::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


Assets_Page::header (translate("Suppressed check results"));
$view = new Assets_View (__FILE__);


@$release = request->query['release'];
if (isset($release)) {
  database_check.release ($release);
  $view.set_variable ("success = translate("The check result will no longer be suppressed.");
}


// Get the Bibles the user has write-access to.
$bibleIDs = array ();
$bibles = request->database_bibles()->getBibles ();
for ($bibles as $bible) {
  if (access_bible_write ($bible)) {
    $id = request->database_bibles()->getID ($bible);
    $bibleIDs [] = $id;
  }
}


$ids = array ();
$data = array ();
$suppressions = database_check.getSuppressions ();
for ($suppressions as $suppression) {
  $bibleID = $suppression['bible'];
  // Only display entries for Bibles the user has write access to.
  if (in_array ($bibleID, $bibleIDs)) {
    $ids [] = $suppression['rowid'];
    $bible = filter_string_sanitize_html (request->database_bibles()->getName ($bibleID));
    $passage = filter_passage_display_inline (array (array ($suppression['book'], $suppression['chapter'], $suppression['verse'])));
    $result = filter_string_sanitize_html ($suppression['data']);
    $result = "$bible $passage $result";
    $data [] = $result;
  }
}
$view.set_variable ("ids = $ids;
$view.set_variable ("data = $data;


$view->render ("suppress");
Assets_Page::footer ();


?>
