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


$database_config_user = Database_Config_User::getInstance ();
$database_books = Database_Books::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_jobs = Database_Jobs::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bible = $database_config_user->getBible ();


if (isset ($_GET ["generate"])) {
  $jobId = $database_jobs->getNewId ();
  $database_jobs->setLevel ($jobId, Filter_Roles::CONSULTANT_LEVEL);
  $username = $session_logic->currentUser ();
  Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/printcli.php", $jobId, $username, $bible));
  Filter_Url::redirect ("../jobs/index.php?id=$jobId");
  die;
}


$header = new Assets_Header (Locale_Translate::_("Print"));
$header->jQueryUIOn ("sortable");
$header->run ();


@$add = $_GET['add'];
if (isset ($add)) {
  if ($add == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a resource to add"));
    // The selectable resources are the available ones minus the already selected ones.
    $resources = Resource_Logic::getNames ();
    $resources = array_diff ($resources, $database_config_user->getPrintResources ());
    foreach ($resources as $resource) {
      $parameter = "&add=$resource";
      $dialog_list->add_row ($resource, $parameter);
    }
    $dialog_list->run ();
  } else {
    $resources = $database_config_user->getPrintResources ();
    $resources [] = $add;
    $database_config_user->setPrintResources ($resources);
    unset ($resources);
  }
}


@$remove = $_GET['remove'];
if (isset ($remove)) {
  $resources = $database_config_user->getPrintResources ();
  $key = array_search ($remove, $resources);
  unset ($resources[$key]);
  $database_config_user->setPrintResources ($resources);
}


@$resources = $_POST ['resources'];
if (isset ($resources)) {
  $resources = explode (",", $resources);
  $database_config_user->setPrintResources ($resources);
}


@$frombook = $_GET['frombook'];
if (isset ($frombook)) {
  if ($frombook == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a book"));
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $parameter = "frombook=$book";
      $book = $database_books->getEnglishFromId ($book);
      $dialog_list->add_row ($book, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set where to start from: Set book, chapter 1, verse 0.
    $frompassage = explode (".", $database_config_user->getPrintPassageFrom ());
    $frompassage [0] = $frombook;
    $frompassage [1] = 0;
    $frompassage [2] = 0;
    $database_config_user->setPrintPassageFrom (implode (".", $frompassage));
    // Check if ending book matches.
    $topassage = explode (".", $database_config_user->getPrintPassageTo ());
    if (Filter_Books::passage2integer ($topassage) < Filter_Books::passage2integer ($frompassage)) {
      // Set ending passage to a sensible value.
      $topassage [0] = $frombook;
      $chapters = $database_bibles->getChapters ($bible, $topassage [0]);
      $topassage [1] = array_pop ($chapters);
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $topassage [0], $topassage [1]));
      $topassage [2] = array_pop ($verses);
      $database_config_user->setPrintPassageTo (implode (".", $topassage));
    }
  }
}


@$fromchapter = $_GET['fromchapter'];
if (isset ($fromchapter)) {
  if ($fromchapter == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a chapter"));
    $passage = explode (".", $database_config_user->getPrintPassageFrom ());
    $chapters = $database_bibles->getChapters ($bible, $passage [0]);
    foreach ($chapters as $chapter) {
      $parameter = "fromchapter=$chapter";
      $dialog_list->add_row ($chapter, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set which chapter to start from, and the verse also.
    $frompassage = explode (".", $database_config_user->getPrintPassageFrom ());
    $frompassage [1] = $fromchapter;
    $frompassage [2] = 0;
    $database_config_user->setPrintPassageFrom (implode (".", $frompassage));
    // Check if ending passage is sensible.
    $topassage = explode (".", $database_config_user->getPrintPassageTo ());
    if (Filter_Books::passage2integer ($topassage) < Filter_Books::passage2integer ($frompassage)) {
      // Set ending chapter / verse to sensible values.
      $topassage [1] = $fromchapter;
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $topassage [0], $topassage [1]));
      $topassage [2] = array_pop ($verses);
      $database_config_user->setPrintPassageTo (implode (".", $topassage));
    }
  }
}


@$fromverse = $_GET['fromverse'];
if (isset ($fromverse)) {
  if ($fromverse == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a verse"));
    $passage = explode (".", $database_config_user->getPrintPassageFrom ());
    $usfm = $database_bibles->getChapter ($bible, $passage [0], $passage [1]);
    $verses = Filter_Usfm::getVerseNumbers ($usfm);
    foreach ($verses as $verse) {
      $parameter = "fromverse=$verse";
      $dialog_list->add_row ($verse, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set verse.
    $frompassage = explode (".", $database_config_user->getPrintPassageFrom ());
    $frompassage [2] = $fromverse;
    $database_config_user->setPrintPassageFrom (implode (".", $frompassage));
    // Sensible matching ending verse.
    $topassage = explode (".", $database_config_user->getPrintPassageTo ());
    if (Filter_Books::passage2integer ($topassage) < Filter_Books::passage2integer ($frompassage)) {
      $verses = Filter_Usfm::getVerseNumbers ($database_bibles->getChapter ($bible, $topassage [0], $topassage [1]));
      $topassage [2] = array_pop ($verses);
      $database_config_user->setPrintPassageTo (implode (".", $topassage));
    }
  }
}


@$tobook = $_GET['tobook'];
if (isset ($tobook)) {
  if ($tobook == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a book"));
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $parameter = "tobook=$book";
      $book = $database_books->getEnglishFromId ($book);
      $dialog_list->add_row ($book, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set ending passage.
    $topassage = explode (".", $database_config_user->getPrintPassageTo ());
    $topassage [0] = $tobook;
    $topassage [1] = 1;
    $topassage [2] = 0;
    $database_config_user->setPrintPassageTo (implode (".", $topassage));
    // Check on matching starting book.
    $frompassage = explode (".", $database_config_user->getPrintPassageFrom ());
    if (Filter_Books::passage2integer ($topassage) < Filter_Books::passage2integer ($frompassage)) {
      // Set starting passage to a sensible value.
      $frompassage [0] = $tobook;
      $frompassage [1] = 0;
      $frompassage [2] = 0;
      $database_config_user->setPrintPassageFrom (implode (".", $frompassage));
    }
  }
}


@$tochapter = $_GET['tochapter'];
if (isset ($tochapter)) {
  if ($tochapter == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a chapter"));
    $passage = explode (".", $database_config_user->getPrintPassageTo ());
    $chapters = $database_bibles->getChapters ($bible, $passage [0]);
    foreach ($chapters as $chapter) {
      $parameter = "tochapter=$chapter";
      $dialog_list->add_row ($chapter, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set chapter.
    $topassage = explode (".", $database_config_user->getPrintPassageTo ());
    $topassage [1] = $tochapter;
    $topassage [2] = 0;
    $database_config_user->setPrintPassageTo (implode (".", $topassage));
    // Match starting passage.
    $frompassage = explode (".", $database_config_user->getPrintPassageFrom ());
    if (Filter_Books::passage2integer ($topassage) < Filter_Books::passage2integer ($frompassage)) {
      // Set starting passage to a sensible value.
      $frompassage [0] = $topassage [0];
      $frompassage [1] = 0;
      $frompassage [2] = 0;
      $database_config_user->setPrintPassageFrom (implode (".", $frompassage));
    }
  }
}


@$toverse = $_GET['toverse'];
if (isset ($toverse)) {
  if ($toverse == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a verse"));
    $passage = explode (".", $database_config_user->getPrintPassageTo ());
    $usfm = $database_bibles->getChapter ($bible, $passage [0], $passage [1]);
    $verses = Filter_Usfm::getVerseNumbers ($usfm);
    foreach ($verses as $verse) {
      $parameter = "toverse=$verse";
      $dialog_list->add_row ($verse, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set ending verse.
    $topassage = explode (".", $database_config_user->getPrintPassageTo ());
    $topassage [2] = $toverse;
    $database_config_user->setPrintPassageTo (implode (".", $topassage));
    // Match starting verse.
    $frompassage = explode (".", $database_config_user->getPrintPassageFrom ());
    if (Filter_Books::passage2integer ($topassage) < Filter_Books::passage2integer ($frompassage)) {
      // Set starting passage to a sensible value.
      $frompassage [0] = $topassage [0];
      $frompassage [1] = $topassage [1];
      $frompassage [2] = 0;
      $database_config_user->setPrintPassageFrom (implode (".", $frompassage));
    }
  }
}


$view = new Assets_View (__FILE__);


$resources = $database_config_user->getPrintResources ();
$view->view->resources = $resources;


$passage = explode (".", $database_config_user->getPrintPassageFrom ());
$view->view->from_book = $database_books->getEnglishFromId ($passage [0]);
$view->view->from_chapter = $passage [1];
$view->view->from_verse = $passage [2];


$passage = explode (".", $database_config_user->getPrintPassageTo ());
$view->view->to_book = $database_books->getEnglishFromId ($passage [0]);
$view->view->to_chapter = $passage [1];
$view->view->to_verse = $passage [2];


$view->render ("print.php");
Assets_Page::footer ();


?>
