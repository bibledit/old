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


$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_sprint = Database_Sprint::getInstance ();


if (isset (request->query ['previoussprint'])) {
  $month = request->database_config_user()->getSprintMonth ();
  $year = request->database_config_user()->getSprintYear ();
  $time = mktime (0, 0, 0, $month - 1, 1, $year);
  request->database_config_user()->setSprintMonth (date ("n", $time));
  request->database_config_user()->setSprintYear (date ("Y", $time));
}


if (isset (request->query ['currentprint'])) {
  request->database_config_user()->setSprintMonth (date ("n"));
  request->database_config_user()->setSprintYear (date ("Y"));
}


if (isset (request->query ['nextsprint'])) {
  $month = request->database_config_user()->getSprintMonth ();
  $year = request->database_config_user()->getSprintYear ();
  $time = mktime (0, 0, 0, $month + 1, 1, $year);
  request->database_config_user()->setSprintMonth (date ("n", $time));
  request->database_config_user()->setSprintYear (date ("Y", $time));
}


$bible = access_bible_clamp (request->database_config_user()->getBible ());
$month = request->database_config_user()->getSprintMonth ();
$year = request->database_config_user()->getSprintYear ();


$header = new Assets_Header (gettext("Sprint"));
$view = new Assets_View (__FILE__);


@$title = request->post ['add'];
if (isset ($title)) {
  $database_sprint->storeTask ($bible, $year, $month, $title);
  $view.set_variable ("success = gettext("New task added");
  // Focus the entry for adding tasks only in case a new task was added.
  $header->setBodyOnload ('document.addtask.add.focus();'); // Use html5 autofocus
}


@$mail = request->query ['mail'];
if (isset ($mail)) {
  Sprint_Logic::burndown ($bible, true);
  $view.set_variable ("success = gettext("The information was mailed to the subscribers");
  // Give the burndown logic time to update the sprint history,
  // so the page will display the updated burndown chart.
  sleep (2);
}


$header->run ();


@$bible = request->query['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (gettext("Select which Bible to display the Sprint for"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      // Select from Bibles the user has write access to.
      if (access_bible_write ($bible)) {
        $dialog_list->add_row ($bible, "&bible=$bible");
      }
    }
    $dialog_list->run();
  } else {
    request->database_config_user()->setBible ($bible);
  }
}


$bible = access_bible_clamp (request->database_config_user()->getBible ());


@$id = request->query ['id'];


@$moveback = request->query ['moveback'];
if (isset ($moveback)) {
  $time = mktime (0, 0, 0, $month - 1, 1, $year);
  $database_sprint->updateMonthYear ($id, date ("n", $time), date ("Y", $time));
  $view.set_variable ("success = gettext("The task was moved to the previous sprint");
}


@$moveforward = request->query ['moveforward'];
if (isset ($moveforward)) {
  $time = mktime (0, 0, 0, $month + 1, 1, $year);
  $database_sprint->updateMonthYear ($id, date ("n", $time), date ("Y", $time));
  $view.set_variable ("success = gettext("The task was moved to the next sprint");
}


@$complete = request->query ['complete'];
if (isset ($complete)) {
  $complete = Filter_Numeric::integer_in_string ($complete);
  $database_sprint->updateComplete ($id, $complete);
}


@$categories = request->post ['categories'];
if (isset ($categories)) {
  $categories2 = array ();
  $categories = filter_string_trim ($categories);
  $categories = explode ("\n", $categories);
  for ($categories as $category) {
    $category = filter_string_trim ($category);
    if ($category != "") $categories2 [] = $category;
  }
  $categories = implode ("\n", $categories2);
  Database_Config_Bible::setSprintTaskCompletionCategories ($bible, $categories);
}


$tasks = $database_sprint->getTasks ($bible, $year, $month);
$titles = array ();
$percentages = array ();
for ($tasks as &$id) {
  $titles [] = filter_string_sanitize_html ($database_sprint->getTitle ($id));
  $percentages [] = $database_sprint->getComplete ($id);
}


$view.set_variable ("bible = $bible;
$view.set_variable ("sprint = date ("F Y", mktime (0, 0, 0, $month, 1, $year));
$view.set_variable ("tasks = $tasks;
$view.set_variable ("titles = $titles;
$view.set_variable ("percentages = $percentages;
//if (extension_loaded ("gd")) {
//  $view.set_variable ("chart = base64_encode (Sprint_Logic::createGraphicalBurndownChart ($bible, $year, $month));
//}
$view.set_variable ("chart2 = Sprint_Logic::createTextBasedBurndownChart ($bible, $year, $month);


$categorytext = Database_Config_Bible::getSprintTaskCompletionCategories ($bible);
$view.set_variable ("categorytext = $categorytext;
$categories = explode ("\n", $categorytext);
$view.set_variable ("categories = $categories;


$view->render ("index");


Assets_Page::footer ();


?>
