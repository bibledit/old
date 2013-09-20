<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
page_access_level (TRANSLATOR_LEVEL);


$header = new Assets_Header (gettext ("Sprint"));
$header->setBodyOnload ('document.addtask.add.focus();');
$header->run ();
$view = new Assets_View (__FILE__);


$database_config_user = Database_Config_User::getInstance ();
$database_sprint = Database_Sprint::getInstance ();


$bible = $database_config_user->getBible ();


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


$month = $database_config_user->getSprintMonth ();
$year = $database_config_user->getSprintYear ();


@$title = $_POST ['add'];
if (isset ($title)) {
  $database_sprint->storeTask ($year, $month, $title);
  $view->view->success = gettext ("New task added");
}


@$mail = $_GET ['mail'];
if (isset ($mail)) {
  Sprint_Logic::burndown (true);
  $view->view->success = gettext ("The information was mailed to the subscribers");
}


@$id = $_GET ['id'];


@$moveback = $_GET ['moveback'];
if (isset ($moveback)) {
  $time = mktime (0, 0, 0, $month - 1, 1, $year);
  $database_sprint->updateMonthYear ($id, date ("n", $time), date ("Y", $time));
  $view->view->success = gettext ("The task was moved to the previous sprint");
}


@$moveforward = $_GET ['moveforward'];
if (isset ($moveforward)) {
  $time = mktime (0, 0, 0, $month + 1, 1, $year);
  $database_sprint->updateMonthYear ($id, date ("n", $time), date ("Y", $time));
  $view->view->success = gettext ("The task was moved to the next sprint");
}


@$complete = $_GET ['complete'];
if (isset ($complete)) {
  $complete = Filter_Numeric::integer_in_string ($complete);
  $complete = in_array ($complete, array (0, 25, 50, 75, 100)) ? $complete : 50;
  $database_sprint->updateComplete ($id, $complete);
}


$tasks = $database_sprint->getTasks ($year, $month);
$titles = array ();
$percentages = array ();
foreach ($tasks as $id) {
  $titles [] = Filter_Html::sanitize ($database_sprint->getTitle ($id));
  $percentages [] = $database_sprint->getComplete ($id);
}


$view->view->sprint = date ("F Y", mktime (0, 0, 0, $month, 1, $year));
$view->view->tasks = $tasks;
$view->view->titles = $titles;
$view->view->percentages = $percentages;
$view->view->chart = base64_encode (Sprint_Logic::createBurndownChart ($year, $month));


$view->render ("index.php");
Assets_Page::footer ();
?>
