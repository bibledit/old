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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::consultant ());


$database_config_user = Database_Config_User::getInstance ();
$database_books = Database_Books::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_jobs = Database_Jobs::getInstance ();
$session_logic = Session_Logic::getInstance ();


$bible = request->database_config_user()->getBible ();


if (isset (request->query ["generate"])) {
  $jobId = $database_jobs->getNewId ();
  $database_jobs->setLevel ($jobId, Filter_Roles::consultant ());
  $username = request->session_logic()->currentUser ();
  tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ . "/printcli.php", $jobId, $username, $bible));
  redirect_browser ("../jobs/index.php?id=$jobId");
  die;
}


$header = new Assets_Header (gettext("Print"));
$header->jQueryUIOn ("sortable");
$header->run ();


@$add = request->query['add'];
if (isset ($add)) {
  if ($add == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a resource to add"));
    // The selectable resources are the available ones minus the already selected ones.
    $resources = Resource_Logic::getNames ();
    $resources = filter_string_array_diff ($resources, request->database_config_user()->getPrintResources ());
    for ($resources as $resource) {
      $parameter = "&add=$resource";
      $dialog_list->add_row ($resource, $parameter);
    }
    $dialog_list->run ();
  } else {
    $resources = request->database_config_user()->getPrintResources ();
    $resources [] = $add;
    request->database_config_user()->setPrintResources ($resources);
    unset ($resources);
  }
}


@$remove = request->query['remove'];
if (isset ($remove)) {
  $resources = request->database_config_user()->getPrintResources ();
  $key = array_search ($remove, $resources);
  unset ($resources[$key]);
  request->database_config_user()->setPrintResources ($resources);
}


@$resources = request->post ['resources'];
if (isset ($resources)) {
  $resources = explode (",", $resources);
  request->database_config_user()->setPrintResources ($resources);
}


@$frombook = request->query['frombook'];
if (isset ($frombook)) {
  if ($frombook == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a book"));
    $books = request->database_bibles()->getBooks ($bible);
    for ($books as $book) {
      $parameter = "frombook=$book";
      $book = Database_Books::getEnglishFromId ($book);
      $dialog_list->add_row ($book, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set where to start from: Set book, chapter 1, verse 0.
    $frompassage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    $frompassage [0] = $frombook;
    $frompassage [1] = 0;
    $frompassage [2] = 0;
    request->database_config_user()->setPrintPassageFrom (implode (".", $frompassage));
    // Check if ending book matches.
    $topassage = explode (".", request->database_config_user()->getPrintPassageTo ());
    if (filter_passage_to_integer ($topassage) < filter_passage_to_integer ($frompassage)) {
      // Set ending passage to a sensible value.
      $topassage [0] = $frombook;
      $chapters = request->database_bibles()->getChapters ($bible, $topassage [0]);
      $topassage [1] = array_pop ($chapters);
      $verses = usfm_get_verse_numbers (request->database_bibles()->getChapter ($bible, $topassage [0], $topassage [1]));
      $topassage [2] = array_pop ($verses);
      request->database_config_user()->setPrintPassageTo (implode (".", $topassage));
    }
  }
}


@$fromchapter = request->query['fromchapter'];
if (isset ($fromchapter)) {
  if ($fromchapter == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a chapter"));
    $passage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    $chapters = request->database_bibles()->getChapters ($bible, $passage [0]);
    for ($chapters as $chapter) {
      $parameter = "fromchapter=$chapter";
      $dialog_list->add_row ($chapter, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set which chapter to start from, and the verse also.
    $frompassage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    $frompassage [1] = $fromchapter;
    $frompassage [2] = 0;
    request->database_config_user()->setPrintPassageFrom (implode (".", $frompassage));
    // Check if ending passage is sensible.
    $topassage = explode (".", request->database_config_user()->getPrintPassageTo ());
    if (filter_passage_to_integer ($topassage) < filter_passage_to_integer ($frompassage)) {
      // Set ending chapter / verse to sensible values.
      $topassage [1] = $fromchapter;
      $verses = usfm_get_verse_numbers (request->database_bibles()->getChapter ($bible, $topassage [0], $topassage [1]));
      $topassage [2] = array_pop ($verses);
      request->database_config_user()->setPrintPassageTo (implode (".", $topassage));
    }
  }
}


@$fromverse = request->query['fromverse'];
if (isset ($fromverse)) {
  if ($fromverse == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a verse"));
    $passage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    $usfm = request->database_bibles()->getChapter ($bible, $passage [0], $passage [1]);
    $verses = usfm_get_verse_numbers ($usfm);
    for ($verses as $verse) {
      $parameter = "fromverse=$verse";
      $dialog_list->add_row ($verse, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set verse.
    $frompassage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    $frompassage [2] = $fromverse;
    request->database_config_user()->setPrintPassageFrom (implode (".", $frompassage));
    // Sensible matching ending verse.
    $topassage = explode (".", request->database_config_user()->getPrintPassageTo ());
    if (filter_passage_to_integer ($topassage) < filter_passage_to_integer ($frompassage)) {
      $verses = usfm_get_verse_numbers (request->database_bibles()->getChapter ($bible, $topassage [0], $topassage [1]));
      $topassage [2] = array_pop ($verses);
      request->database_config_user()->setPrintPassageTo (implode (".", $topassage));
    }
  }
}


@$tobook = request->query['tobook'];
if (isset ($tobook)) {
  if ($tobook == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a book"));
    $books = request->database_bibles()->getBooks ($bible);
    for ($books as $book) {
      $parameter = "tobook=$book";
      $book = Database_Books::getEnglishFromId ($book);
      $dialog_list->add_row ($book, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set ending passage.
    $topassage = explode (".", request->database_config_user()->getPrintPassageTo ());
    $topassage [0] = $tobook;
    $topassage [1] = 1;
    $topassage [2] = 0;
    request->database_config_user()->setPrintPassageTo (implode (".", $topassage));
    // Check on matching starting book.
    $frompassage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    if (filter_passage_to_integer ($topassage) < filter_passage_to_integer ($frompassage)) {
      // Set starting passage to a sensible value.
      $frompassage [0] = $tobook;
      $frompassage [1] = 0;
      $frompassage [2] = 0;
      request->database_config_user()->setPrintPassageFrom (implode (".", $frompassage));
    }
  }
}


@$tochapter = request->query['tochapter'];
if (isset ($tochapter)) {
  if ($tochapter == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a chapter"));
    $passage = explode (".", request->database_config_user()->getPrintPassageTo ());
    $chapters = request->database_bibles()->getChapters ($bible, $passage [0]);
    for ($chapters as $chapter) {
      $parameter = "tochapter=$chapter";
      $dialog_list->add_row ($chapter, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set chapter.
    $topassage = explode (".", request->database_config_user()->getPrintPassageTo ());
    $topassage [1] = $tochapter;
    $topassage [2] = 0;
    request->database_config_user()->setPrintPassageTo (implode (".", $topassage));
    // Match starting passage.
    $frompassage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    if (filter_passage_to_integer ($topassage) < filter_passage_to_integer ($frompassage)) {
      // Set starting passage to a sensible value.
      $frompassage [0] = $topassage [0];
      $frompassage [1] = 0;
      $frompassage [2] = 0;
      request->database_config_user()->setPrintPassageFrom (implode (".", $frompassage));
    }
  }
}


@$toverse = request->query['toverse'];
if (isset ($toverse)) {
  if ($toverse == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a verse"));
    $passage = explode (".", request->database_config_user()->getPrintPassageTo ());
    $usfm = request->database_bibles()->getChapter ($bible, $passage [0], $passage [1]);
    $verses = usfm_get_verse_numbers ($usfm);
    for ($verses as $verse) {
      $parameter = "toverse=$verse";
      $dialog_list->add_row ($verse, $parameter);
    }
    $dialog_list->run ();
  } else {
    // Set ending verse.
    $topassage = explode (".", request->database_config_user()->getPrintPassageTo ());
    $topassage [2] = $toverse;
    request->database_config_user()->setPrintPassageTo (implode (".", $topassage));
    // Match starting verse.
    $frompassage = explode (".", request->database_config_user()->getPrintPassageFrom ());
    if (filter_passage_to_integer ($topassage) < filter_passage_to_integer ($frompassage)) {
      // Set starting passage to a sensible value.
      $frompassage [0] = $topassage [0];
      $frompassage [1] = $topassage [1];
      $frompassage [2] = 0;
      request->database_config_user()->setPrintPassageFrom (implode (".", $frompassage));
    }
  }
}


$view = new Assets_View (__FILE__);


$resources = request->database_config_user()->getPrintResources ();
$view->view->resources = $resources;


$passage = explode (".", request->database_config_user()->getPrintPassageFrom ());
$view->view->from_book = Database_Books::getEnglishFromId ($passage [0]);
$view->view->from_chapter = $passage [1];
$view->view->from_verse = $passage [2];


$passage = explode (".", request->database_config_user()->getPrintPassageTo ());
$view->view->to_book = Database_Books::getEnglishFromId ($passage [0]);
$view->view->to_chapter = $passage [1];
$view->view->to_verse = $passage [2];


$view->render ("print.php");
Assets_Page::footer ();


?>
