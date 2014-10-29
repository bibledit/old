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
$database_users = Database_Users::getInstance ();


Assets_Page::header (Locale_Translate::_("Checks"));


$view = new Assets_View (__FILE__);


@$approve = $_GET['approve'];
if (isset ($approve)) {
  $database_check->approve ($approve);
  $view->view->success = Locale_Translate::_("The entry was approved and suppressed.");
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_check->delete ($delete);
  $view->view->success = Locale_Translate::_("The entry was deleted for just now.");
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
$bibles = array ();
$links = array ();
$passages = array ();
$passageTexts = array ();
$information = array ();


$hits = $database_check->getHits ();
foreach ($hits as $hit) {
  $bibleID = $hit['bible'];
  if (in_array ($bibleID, $bibleIDs)) {
    $ids [] = $hit['rowid'];
    $bible = Filter_Html::sanitize ($database_bibles->getName ($bibleID));
    $bibles [] = $bible;
    $book = $hit['book'];
    $chapter = $hit['chapter'];
    $verse = $hit['verse'];
    $link = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);
    $links [] = $link;
    $information [] = Filter_Html::sanitize ($hit['data']);
  }
}


$view->view->ids = $ids;
$view->view->bibles = $bibles;
$view->view->links = $links;
$view->view->information = $information;


$view->render ("index.php");
Assets_Page::footer ();
?>
