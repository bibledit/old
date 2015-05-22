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


chdir (__DIR__);


require_once ("../bootstrap/bootstrap.php");


if (Filter_Cli::not_yet_ready ()) die;


// The script runs through the cli Server API only.
Filter_Cli::assert ();


ignore_user_abort (true);
set_time_limit (0);


// CPU-intensive actions run at night.
// This keeps the site more responsive during the day.


$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();


$hour = date ('G');
$minute = date ('i');
$client = Filter_Client::enabled ();


// At midnight, for five minutes, do nothing to allow time for external backup
// without corrupting the SQLite databases due to simultaneous access by
// Bibledit-Web and the backup program.
if ($hour == 0) {
  if ($minute <= 5) {
    die;
  }
}



// At the sixth minute after midnight, after the backup silence, and any hour after, rotate the logbook.
if ($minute == 6) {
  $directory = dirname (__DIR__) . "/journal";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/rotate.php"));
}


// Every minute send out queued email, except in offline mode.
if (!$client) {
  Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/mailer.php"));
}


// Check for new mail every five minutes.
// Do not check more often with gmail else the account may be shut down.
if (!$client) {
  if (($minute % 5) == 0) {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/receiver.php"));
  }
}


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
    Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/git.php"));
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


// Database maintenance and trimming.
// It takes a few minutes on a production machine.
if (($hour == 0) && ($minute == 50)) {
  Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/database.php"));
}


// Sprint burndown.
// It runs every hour.
// The script itself determines what to do at which hour of the day or day of the week or day of the month.
if (!$client) {
  if ($minute == 5) {
    Sprint_Logic::burndown ("", false);
  }
}


// A flag is set upon installation, so it (re-)generates the stylesheets once.
$create_css_flag = "../dyncss/create_css.txt";
if (file_exists ($create_css_flag)) {
  unlink ($create_css_flag);
  Styles_Sheets::create_all ();
}


// Export the Bibles to the various output formats.
// This may take an hour on a production machine.
if (($hour == 1) && ($minute == 10)) {
  Export_Logic::scheduleAll ();
}


// Re-index Bible and notes.
if (($hour == 2) && ($minute == 0)) {
  $directory = dirname (__DIR__) . "/search";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/rebibles.php"));
  $directory = dirname (__DIR__) . "/search";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/renotes.php"));
}


// Delete temporal files older than a few days.
if (($hour == 2) && ($minute == 0)) {
  Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/temp.php"));
}


// Email statistics to the users.
if (!$client) {
  if (($hour == 3) && ($minute == 0)) {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/statistics.php"));
  }
}


// A flag is set on setup to (re-)create a number of databases.
$create_databases_flag = "../config/databases";
if (file_exists ($create_databases_flag)) {
  unlink ($create_databases_flag);
  // Strong's numbers and English glosses for Bible.
  $directory = dirname (__DIR__) . "/kjv";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/import.php"));
  // Hebrew Bible.
  $directory = dirname (__DIR__) . "/morphhb";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/import.php"));
  // Greek Bible.
  $directory = dirname (__DIR__) . "/sblgnt";
  Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/import.php"));
}


// Check open installation.
if ($minute == 10) {
  include ("config/open.php");
  if ($open_installation) {
    $directory = dirname (__DIR__) . "/demo";
    Tasks_Logic::queue (Tasks_Logic::PHP, array ("$directory/index.php"));
  }
}


// Kick the watchdog for the minutely tasks.
$watchdog = dirname (__DIR__) . "/config/watchdog";
file_put_contents ($watchdog, "");


?>
