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
page_access_level (Filter_Roles::MANAGER_LEVEL);


Assets_Page::header (Locale_Translate::_("Bible"));
$view = new Assets_View (__FILE__);


$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_users = Database_Users::getInstance ();


// The Bible.
$bible = Access_Bible::clamp ($_GET['bible']);
$view->view->bible = Filter_Html::sanitize ($bible);


// Whether the user has write access to this Bible.
$write_access = Access_Bible::write ($bible);
$view->view->write_access = $write_access;


// Versification.
@$versification = $_GET['versification'];
if (isset ($versification)) {
  if ($versification == "") {
    $dialog_versifications = new Dialog_List (array ("bible"), Locale_Translate::_("Would you like to change the versification system?"), Locale_Translate::_ ("A versification system determines how many chapters are in each book, and how many verses are in each chapter. Please make your choice below."), "");
    $database_versifications = Database_Versifications::getInstance ();
    $versification_names = $database_versifications->getSystems ();
    foreach ($versification_names as $versification_name) {
      $dialog_versifications->add_row ($versification_name, "&versification=$versification_name");
    }
    $dialog_versifications->run();
    die;
  } else {
    if ($write_access) $database_config_bible->setVersificationSystem ($bible, $versification);
  }
}
$versification = $database_config_bible->getVersificationSystem ($bible);
$view->view->versification = $versification;


// Verse mapping.
@$mapping = $_GET['mapping'];
if (isset ($mapping)) {
  if ($mapping == "") {
    $dialog = new Dialog_List (array ("bible"), Locale_Translate::_("Would you like to change the verse mapping?"), Locale_Translate::_ ("A verse mapping can be used to match verses for parallel Bible display. Please make your choice below."), "");
    $database_mappings = Database_Mappings::getInstance ();
    $mapping_names = $database_mappings->names ();
    foreach ($mapping_names as $mapping_name) {
      $dialog->add_row ($mapping_name, "&mapping=$mapping_name");
    }
    $dialog->run ();
    die;
  } else {
    if ($write_access) $database_config_bible->setVerseMapping ($bible, $mapping);
  }
}
$mapping = $database_config_bible->getVerseMapping ($bible);
$view->view->mapping = $mapping;


// Book creation.
@$createbook = $_GET['createbook'];
if (isset ($createbook)) {
  if ($createbook == "") {
    $dialog_books = new Dialog_Books (array ("bible"), Locale_Translate::_("Create book"), "", "", "createbook", NULL, $database_bibles->getBooks ($bible));
    die;
  } else {
    $feedback = array ();
    if ($write_access) Book_Create::create ($bible, $createbook, NULL, $feedback);
  }
}


// Book deletion.
@$deletebook = $_GET['deletebook'];
if ($deletebook != "") {
  @$confirm = $_GET['confirm'];
  if ($confirm != "") {
    if ($write_access) Bible_Logic::deleteBook ($bible, $deletebook);
  } else {
    $dialog_yes = new Dialog_Yes (array ("bible"), Locale_Translate::_("Would you like to delete this book?"), "deletebook");
    die;
  }
}


// Available books.
$book_names = array ();
$book_ids = Filter_Books::getOrdered ($bible);
foreach ($book_ids as $book) {
  $book_name = $database_books->getEnglishFromId ($book);
  $book_name = Locale_Translate::_($book_name);
  $book_names [] = $book_name;
}
$view->view->book_ids = $book_ids;
$view->view->book_names = $book_names;
$view->view->book_count = count ($book_ids);


if (isset ($_GET['viewable'])) {
  if ($write_access) $database_config_bible->setViewableByAllUsers ($bible, !$database_config_bible->getViewableByAllUsers ($bible));
}
$view->view->viewable = $database_config_bible->getViewableByAllUsers ($bible);


$view->render ("settings.php");


Assets_Page::footer ();


?>
