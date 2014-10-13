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


$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_sprint = Database_Sprint::getInstance ();


if (isset ($_GET ['previoussprint'])) {
  $month = $database_config_user->getSprintMonth ();
  $year = $database_config_user->getSprintYear ();
  $time = mktime (0, 0, 0, $month - 1, 1, $year);
  $database_config_user->setSprintMonth (date ("n", $time));
  $database_config_user->setSprintYear (date ("Y", $time));
}


if (isset ($_GET ['currentprint'])) {
  $database_config_user->setSprintMonth (date ("n"));
  $database_config_user->setSprintYear (date ("Y"));
}


if (isset ($_GET ['nextsprint'])) {
  $month = $database_config_user->getSprintMonth ();
  $year = $database_config_user->getSprintYear ();
  $time = mktime (0, 0, 0, $month + 1, 1, $year);
  $database_config_user->setSprintMonth (date ("n", $time));
  $database_config_user->setSprintYear (date ("Y", $time));
}


$bible = Access_Bible::clamp ($database_config_user->getBible ());
$month = $database_config_user->getSprintMonth ();
$year = $database_config_user->getSprintYear ();


$header = new Assets_Header (Locale_Translate::_("Sprint"));
$view = new Assets_View (__FILE__);


@$title = $_POST ['add'];
if (isset ($title)) {
  $database_sprint->storeTask ($bible, $year, $month, $title);
  $view->view->success = Locale_Translate::_("New task added");
  // Focus the entry for adding tasks only in case a new task was added.
  $header->setBodyOnload ('document.addtask.add.focus();');
}


@$mail = $_GET ['mail'];
if (isset ($mail)) {
  Sprint_Logic::burndown ($bible, true);
  $view->view->success = Locale_Translate::_("The information was mailed to the subscribers");
  // Give the burndown logic time to update the sprint history,
  // so the page will display the updated burndown chart.
  sleep (2);
}


$header->run ();


@$bible = $_GET['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select which Bible to display the Sprint for"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      // Select from Bibles the user has write access to.
      if (Access_Bible::write ($bible)) {
        $dialog_list->add_row ($bible, "&bible=$bible");
      }
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}


$bible = Access_Bible::clamp ($database_config_user->getBible ());


@$id = $_GET ['id'];


@$moveback = $_GET ['moveback'];
if (isset ($moveback)) {
  $time = mktime (0, 0, 0, $month - 1, 1, $year);
  $database_sprint->updateMonthYear ($id, date ("n", $time), date ("Y", $time));
  $view->view->success = Locale_Translate::_("The task was moved to the previous sprint");
}


@$moveforward = $_GET ['moveforward'];
if (isset ($moveforward)) {
  $time = mktime (0, 0, 0, $month + 1, 1, $year);
  $database_sprint->updateMonthYear ($id, date ("n", $time), date ("Y", $time));
  $view->view->success = Locale_Translate::_("The task was moved to the next sprint");
}


@$complete = $_GET ['complete'];
if (isset ($complete)) {
  $complete = Filter_Numeric::integer_in_string ($complete);
  $database_sprint->updateComplete ($id, $complete);
}


@$categories = $_POST ['categories'];
if (isset ($categories)) {
  $categories2 = array ();
  $categories = trim ($categories);
  $categories = explode ("\n", $categories);
  foreach ($categories as $category) {
    $category = trim ($category);
    if ($category != "") $categories2 [] = $category;
  }
  $categories = implode ("\n", $categories2);
  $database_config_bible->setSprintTaskCompletionCategories ($bible, $categories);
}


$tasks = $database_sprint->getTasks ($bible, $year, $month);
$titles = array ();
$percentages = array ();
foreach ($tasks as &$id) {
  $titles [] = Filter_Html::sanitize ($database_sprint->getTitle ($id));
  $percentages [] = $database_sprint->getComplete ($id);
}


$view->view->bible = $bible;
$view->view->sprint = date ("F Y", mktime (0, 0, 0, $month, 1, $year));
$view->view->tasks = $tasks;
$view->view->titles = $titles;
$view->view->percentages = $percentages;
//if (extension_loaded ("gd")) {
//  $view->view->chart = base64_encode (Sprint_Logic::createGraphicalBurndownChart ($bible, $year, $month));
//}
$view->view->chart2 = Sprint_Logic::createTextBasedBurndownChart ($bible, $year, $month);


$categorytext = $database_config_bible->getSprintTaskCompletionCategories ($bible);
$view->view->categorytext = $categorytext;
$categories = explode ("\n", $categorytext);
$view->view->categories = $categories;


$view->render ("index.php");


Assets_Page::footer ();


?>
