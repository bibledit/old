<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


require_once ("../bootstrap/bootstrap.php");


// The script runs through the cli Server API only.
if (php_sapi_name () != "cli") {
  die;
}


// Change the user id for running this script.
$pwnam = posix_getpwnam ("www-data");
posix_setuid ($pwnam['uid']);
posix_setgid ($pwnam['gid']);
posix_seteuid ($pwnam['uid']);
posix_setegid ($pwnam['gid']);


ignore_user_abort (true);
set_time_limit (0);
register_shutdown_function ('shutdown');


$config_general = Database_Config_General::getInstance ();
$timer_logger = new Timer_Logger ();
//$processUser = posix_getpwuid(posix_geteuid());
//$processUser = $processUser['name'];


$current_timestamp = time ();


// CPU-intensive actions run at night.
// This keeps the site more responsive during the day.
$midnight = (date ('Gi') == 0); // Todo goes out.
$fifteenPastMidnight = (date ('Gi') == 15); // Todo goes out.
$hour = date ('G');
$minute = date ('i');

$database_logs = Database_Logs::getInstance (); // Todo temporal.


// Every minute send out any queued mail.
$timer_mailer = new Timer_Mailer ();
$timer_mailer->run ();
unset ($timer_mailer);


// Check for new mail every five minutes.
// Do not check more often with gmail else the account may be shut down.
if (($minute % 5) == 0) {
  $timer_receiver = new Timer_Receiver ();
  $timer_receiver->run ();
  unset ($timer_receiver);
}


// Check on information in used log files every 15 minutes.
if ($minute == 15) {
  $timer_logger->handleUsedLogFiles ();
}
$timer_logger->handleUsedLogFiles (); // Todo temporal


// The running order of the following nightly scripts is important.
// In general, the order will such that all information is generated as recent as possible,
// and that the more important tasks are done first, and the less important ones at the end.
// This means something as displayed in the code of this script.



// Sending and receiving Bibles to and from the git repository.
// On a production website running on an inexpensive virtual private server 
// with 512 Mbyte of memory and a fast network connection, 
// sending and receiving two Bibles takes less than a minute.
if (($current_timestamp >= $config_general->getTimerSendReceive ()) || (($hour == 0) && ($minute == 0))) {
  $config_general->setTimerSendReceive ($current_timestamp + 100000);
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::sendreceive);
  shell_exec ("cd $workingdirectory; php sendreceive.php > $logfilename 2>&1 &");
}


// Sending the daily changes in the Bibles by email.
// This takes a few minutes on a production machine with two Bibles and changes in several chapters.
if (($current_timestamp >= $config_general->getTimerDiff ()) || (($hour == 0) && ($minute == 5))) {
  $config_general->setTimerDiff ($current_timestamp + 100000);
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::changes);
  shell_exec ("cd $workingdirectory; php changes.php > $logfilename 2>&1 &");
}


// Run the checks on the Bibles.
if (($current_timestamp >= $config_general->getTimerChecks ()) || (($hour == 0) && ($minute == 10))) {
  $config_general->setTimerChecks ($current_timestamp + 100000);
  $workingdirectory = dirname (__FILE__);
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::checks);
  shell_exec ("cd $workingdirectory; php checks.php > $logfilename 2>&1 &");
}


// Trim the tables in the database.
if (($hour == 0) && ($minute == 15)) {
  $workingdirectory = dirname (__FILE__);
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::trimdatabases);
  shell_exec ("cd $workingdirectory; php trimdatabases.php > $logfilename 2>&1 &");
}


// Create a backup, so that the backup contains the most recent information
// after the previous tasks have been done.
if (($current_timestamp >= $config_general->getTimerBackup ()) || (($hour == 0) && ($minute == 20))) {
  $config_general->setTimerBackup ($current_timestamp + 100000);
  $workingdirectory = dirname (__FILE__);
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::backup);
  shell_exec ("cd $workingdirectory; php backup.php > $logfilename 2>&1 &");
}


// Index Bibles and Consultation Notes.
// Todo find out on the production website how much time it takes there.
if (($current_timestamp >= $config_general->getTimerSearch ()) || (($hour == 0) && ($minute == 25))) {
  $config_general->setTimerSearch ($current_timestamp + 100000);
  $workingdirectory = dirname (__FILE__);
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::search);
  shell_exec ("cd $workingdirectory; php search.php > $logfilename 2>&1 &");
}















// Todo
// 7. Export the Bibles to the various output formats.

$hour = 0; // Todo
$minute = 25; // Todo






$exports_timestamp = $config_general->getTimerExports ();
if (($current_timestamp >= $exports_timestamp) || $midnight) {
  $exports_timestamp += 86400;
  while ($current_timestamp >= $exports_timestamp) {
    // This loop updates the timestamp to a value larger than the current time.
    // This avoids calling many processes when the task has not been done for a while.
    $exports_timestamp += 86400;
  }
  $config_general->setTimerExports ($exports_timestamp);
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  shell_exec ("cd $workingdirectory; php exports.php > /dev/null 2>&1 &");
}
unset ($exports_timestamp);


function shutdown()
{
}


?>
