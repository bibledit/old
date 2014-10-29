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


$database_check = Database_Check::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


Assets_Page::header (Locale_Translate::_("Suppressed check results"));
$view = new Assets_View (__FILE__);


@$release = $_GET['release'];
if (isset($release)) {
  $database_check->release ($release);
  $view->view->success = Locale_Translate::_("The check result will no longer be suppressed.");
}


// Get the Bibles the user has write-access to.
$bibleIDs = array ();
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  if (Access_Bible::write ($bible)) {
    $id = $database_bibles->getID ($bible);
    $bibleIDs [] = $id;
  }
}


$ids = array ();
$data = array ();
$suppressions = $database_check->getSuppressions ();
foreach ($suppressions as $suppression) {
  $bibleID = $suppression['bible'];
  // Only display entries for Bibles the user has write access to.
  if (in_array ($bibleID, $bibleIDs)) {
    $ids [] = $suppression['rowid'];
    $bible = Filter_Html::sanitize ($database_bibles->getName ($bibleID));
    $passage = Filter_Books::passagesDisplayInline (array (array ($suppression['book'], $suppression['chapter'], $suppression['verse'])));
    $result = Filter_Html::sanitize ($suppression['data']);
    $result = "$bible $passage $result";
    $data [] = $result;
  }
}
$view->view->ids = $ids;
$view->view->data = $data;


$view->render ("suppress.php");
Assets_Page::footer ();


?>
