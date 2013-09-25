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


// Flag for sending email.
@$email = $argv [1];
if (!isset ($email)) $email = false;
$email = $email ? true : false;


$year = date ("Y");
$month = date ("n");
$monthday = date ("j"); // 1 to 31.
$weekday = date ("w"); // 0 (for Sunday) through 6 (for Saturday).
$hour = date ("G"); // 0 through 23
$sprintstart = false;
$sprintfinish = false;


// Every Friday at 2 PM (14:00h) it sends email about the sprint progress.
if (($weekday == 5) && ($hour == 14)) $email = true;
// On the first business day of the month, at 10 AM, send email about the start of the sprint.
if (Filter_Datetime::isFirstWorkingDayOfMonth ($monthday, $weekday) && ($hour == 10)) {
  $email = true;
  $sprintstart = true;
}
// On the last business day of the month, at 2 PM (14:00h), send email about the end of the sprint.
if (($monthday == Filter_Datetime::getLastBusinessDayOfMonth ($year, $month)) && ($hour == 14)) {
  $email = true;
  $sprintfinish = true;
}


$database_logs = Database_Logs::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_users = Database_Users::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_sprint = Database_Sprint::getInstance ();


$database_logs->log ("burndown: " . gettext ("Updating sprint history"));


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("burndown: Fatal: This only runs through the cli Server API", true);
  die;
}


// Determine year / month / day of the current sprint.
// If this script runs from midnight till early morning,
// it applies to the day before.
// If the script runs during the day, it applies to today.
$year = date ("Y");
$month = date ("n");
$day = date ("j");
$hour = date ("G");
$time = mktime ($hour - 6, 0, 0, $month, $day, $year);
$year = date ("Y", $time);
$month = date ("n", $time);
$day = date ("j", $time);


// Get the total number of tasks for this sprint,
// and the average percentage of completion of them,
// and store this information in the sprint history table.
$database_logs->log ("burndown: " . gettext ("Updating day") . " " . $day);
$ids = $database_sprint->getTasks ($year, $month);
$percentages = array ();
foreach ($ids as $id) {
  $percentages [] = $database_sprint->getComplete ($id);
}
$tasks = count ($ids);
if ($tasks == 0) {
  $complete = 0;
} else {
  $complete = array_sum ($percentages) / $tasks;
  $complete = intval ($complete);
}
$database_sprint->logHistory ($year, $month, $day, $tasks, $complete);


// Send email if needed.
if ($email) {
  $categories = $database_config_general->getSprintTaskCompletionCategories ();
  $categories = explode ("\n", $categories);
  $category_count = count ($categories);
  $category_percentage = intval (100 / $category_count);
  $users = $database_users->getUsers ();
  foreach ($users as $user) {
    if ($database_config_user->getUserSprintProgressNotification ($user)) {
      
      $subject = gettext ("Team's progress in Sprint");
      if ($sprintstart) $subject = gettext ("Sprint starts");
      if ($sprintfinish) $subject = gettext ("Sprint has finished");
      
      $body = array ();
      
      $body [] = "<h3>" . gettext ("Sprint Planning and Team's Progress") . "</h3>";
      $body [] = "<table>";
      $tasks = $database_sprint->getTasks ($year, $month);
      foreach ($tasks as $id) {
        $body [] = "<tr>";
        $title = $database_sprint->getTitle ($id);
        $body [] = "<td>" . $title . "</td>";
        $complete = $database_sprint->getComplete ($id);
        $text = str_repeat ("▓", intval ($complete / $category_percentage)) . str_repeat ("▁", $category_count - intval ($complete / $category_percentage));
        $body [] = "<td>" . $text . "</td>";
        $body [] = "</tr>";
      }
      $body [] = "</table>";
      
      $body [] = "<h3>" . gettext ("Sprint Burndown Chart - Remaining Tasks") . "</h3>";
      $burndownchart = Sprint_Logic::createTextBasedBurndownChart ($year, $month);
      $body [] = "<p>$burndownchart</p>";

      if (count ($body) > 0) {
        $body = implode ("\n", $body);
        $database_mail->send ($user, $subject, $body);
      }

    }
  }
}


$database_logs->log ("burndown: " . gettext ("Completed"), true);


?>
