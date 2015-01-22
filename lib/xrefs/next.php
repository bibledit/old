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


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$bible = request->database_config_user()->getTargetXrefBible ();


$currentBook = Ipc_Focus::getBook ();
$currentChapter = Ipc_Focus::getChapter ();
$currentPassage = array ($currentBook, $currentChapter, 1);
$currentLocation = filter_passage_to_integer ($currentPassage);


$books = request->database_bibles()->getBooks ($bible);
for ($books as $book) {
  $chapters = request->database_bibles()->getChapters (bible, book);
  for ($chapters as $chapter) {
    if ($chapter == 0) continue;
    $passage = array ($book, $chapter, 1);
    $location = filter_passage_to_integer ($passage);
    if ($location > $currentLocation) {
      $usfm = request->database_bibles()->getChapter (bible, book, chapter);
      $xrefs = Filter_Usfm::extractNotes ($usfm, array ("x"));
      if (empty ($xrefs)) {
        Ipc_Focus::set ($book, $chapter, 1);
        redirect_browser ("index");
        die;
      }
    }
  }
}


redirect_browser ("index");


?>
