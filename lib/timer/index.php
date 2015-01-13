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


if (Filter_Cli::not_yet_ready ()) die;


// CPU-intensive actions run at night.
// This keeps the site more responsive during the day.


$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();


$client = config_logic_client_enabled ();


// The order for running the following nightly scripts is important.
// Any of those scripts may influence the subsequent ones.
// The order is such that all information generated is as recent as possible.
// More important tasks are done first, and the less important ones at the end.
// This leads to an order as visible in the code below.


// Sending and receiving Bibles to and from the git repository.
// On a production website running on an inexpensive virtual private server
// with 512 Mbyte of memory and a fast network connection,
// sending and receiving two Bibles takes more than 15 minutes when there are many changes.
if (!$client) {
  $sendreceive = (($hour == 0) && ($minute == 6));
  $repeat = (($minute % 5) == 0);
  if ($sendreceive || $repeat) {
    SendReceive_Logic::queueAll ($sendreceive);
  }
}


// Client sends/receives Bibles and Consultation.
SendReceive_Logic::queuesync ($minute);


// Deal with the changes in the Bible made per user.
// Deal with notifications for the daily changes in the Bibles.
// This takes a few minutes on a production machine with two Bibles and changes in several chapters.
if (!$client) {
  if (($hour == 0) && ($minute == 20)) {
    Changes_Logic::start ();
  }
}


// Comb through the git log and add the changes to the history.
if (!$client) {
  if (($hour == 0) && ($minute == 30)) {
    $directory = dirname (dirname (__FILE__)) . "/changes";
    tasks_logic_queue (Tasks_Logic::PHP, array ("$directory/git.php"));
    unset ($directory);
  }
}


// Run the checks on the Bibles.
// This takes 15 minutes on a production machine with two Bibles.
if (!$client) {
  if (($hour == 0) && ($minute == 30)) {
    Checks_Logic::startAll ();
  }
}


// Sprint burndown.
// It runs every hour.
// The script itself determines what to do at which hour of the day or day of the week or day of the month.
if (!$client) {
  if ($minute == 5) {
    Sprint_Logic::burndown ("", false);
  }
}


// Export the Bibles to the various output formats.
// This may take an hour on a production machine.
if (($hour == 1) && ($minute == 10)) {
  Export_Logic::scheduleAll ();
}


// Email statistics to the users.
if (!$client) {
  if (($hour == 3) && ($minute == 0)) {
    tasks_logic_queue (Tasks_Logic::PHP, array (__DIR__ . "/statistics.php"));
  }
}


// A flag is set on setup to (re-)create a number of databases. Todo
$create_databases_flag = "../config/databases";
if (file_exists ($create_databases_flag)) {
  unlink ($create_databases_flag);
  // Strong's numbers and English glosses for Bible.
  $directory = dirname (__DIR__) . "/kjv";
  tasks_logic_queue (Tasks_Logic::PHP, array ("$directory/import.php"));
  // Hebrew Bible.
  $directory = dirname (__DIR__) . "/morphhb";
  tasks_logic_queue (Tasks_Logic::PHP, array ("$directory/import.php"));
  // Greek Bible.
  $directory = dirname (__DIR__) . "/sblgnt";
  tasks_logic_queue (Tasks_Logic::PHP, array ("$directory/import.php"));
}


// Check open installation.
if ($minute == 10) {
  include ("config/open.php");
  if ($open_installation) {
    $directory = dirname (__DIR__) . "/demo";
    tasks_logic_queue (Tasks_Logic::PHP, array ("$directory/index.php"));
  }
}




?>
