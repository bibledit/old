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


page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$bible = $database_config_user->getTargetXrefBible ();


$currentBook = $ipc_focus->getBook ();
$currentChapter = $ipc_focus->getChapter ();
$currentPassage = array ($currentBook, $currentChapter, 1);
$currentLocation = Filter_Books::passage2integer ($currentPassage);


$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    if ($chapter == 0) continue;
    $passage = array ($book, $chapter, 1);
    $location = Filter_Books::passage2integer ($passage);
    if ($location > $currentLocation) {
      $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $xrefs = Filter_Usfm::extractNotes ($usfm, array ("x"));
      if (empty ($xrefs)) {
        $ipc_focus->set ($book, $chapter, 1);
        Filter_Url::redirect ("index.php");
        die;
      }
    }
  }
}


Filter_Url::redirect ("index.php");


?>
