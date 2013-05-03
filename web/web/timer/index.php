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


// Every minute send out any mail.
$timer_mailer = new Timer_Mailer ();
$timer_mailer->run ();
unset ($timer_mailer);


// Receive mail every five minutes.
// Do not receive more often with gmail else the account may be shut down.
if (($minute % 5) == 0) {
  $timer_receiver = new Timer_Receiver ();
  $timer_receiver->run ();
  unset ($timer_receiver);
}


// Check on information in used log files every 15 minutes.
if ($minute == 15) {
  $timer_logger->handleUsedLogFiles ();
}


// Trim databases at midnight.
if ($midnight) {
  $workingdirectory = dirname (__FILE__);
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::trimdatabases);
  shell_exec ("cd $workingdirectory; php trimdatabases.php > $logfilename 2>&1 &");
}


$backup_timestamp = $config_general->getTimerBackup ();
if (($current_timestamp >= $backup_timestamp) || $midnight) {
  $backup_timestamp += 86400;
  while ($current_timestamp >= $backup_timestamp) {
    // This loop updates the backup timestamp to a value larger than the current time.
    // This avoids calling many backup processes when backups have not been made for some time.
    $backup_timestamp += 86400;
  }
  $config_general->setTimerBackup ($backup_timestamp);
  $workingdirectory = dirname (__FILE__);
  shell_exec ("cd $workingdirectory; php backup.php > /dev/null 2>&1 &");
}
unset ($backup_timestamp);


$diff_timestamp = $config_general->getTimerDiff ();
if (($current_timestamp >= $diff_timestamp) || $fifteenPastMidnight) {
  $diff_timestamp += 86400;
  while ($current_timestamp >= $diff_timestamp) {
    // This loop updates the timestamp to a value larger than the current time.
    // This avoids calling many processes when the task has not been done for a while.
    $diff_timestamp += 86400;
  }
  $config_general->setTimerDiff ($diff_timestamp);
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  shell_exec ("cd $workingdirectory; php changes.php > /dev/null 2>&1 &");
}
unset ($diff_timestamp);


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


$sendreceive_timestamp = $config_general->getTimerSendReceive ();
if (($current_timestamp >= $sendreceive_timestamp) || $midnight) {
  $sendreceive_timestamp += 86400;
  while ($current_timestamp >= $sendreceive_timestamp) {
    // This loop updates the timestamp to a value larger than the current time.
    // This avoids calling many processes when the task has not been done for a while.
    $sendreceive_timestamp += 86400;
  }
  $config_general->setTimerSendReceive ($sendreceive_timestamp);
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  shell_exec ("cd $workingdirectory; php sendreceive.php > /dev/null 2>&1 &");
}
unset ($sendreceive_timestamp);


$search_timestamp = $config_general->getTimerSearch ();
if (($current_timestamp >= $search_timestamp) || $fifteenPastMidnight) {
  $search_timestamp += 86400;
  while ($current_timestamp >= $search_timestamp) {
    // This loop updates the search timestamp to a value larger than the current time.
    // This avoids calling many search index operations when indexing has not been done for some time.
    $search_timestamp += 86400;
  }
  $config_general->setTimerSearch ($search_timestamp);
  $workingdirectory = dirname (__FILE__);
  shell_exec ("cd $workingdirectory; php search.php > /dev/null 2>&1 &");
}
unset ($search_timestamp);


$checks_timestamp = $config_general->getTimerChecks ();
if (($current_timestamp >= $checks_timestamp) || $fifteenPastMidnight) {
  $checks_timestamp += 86400;
  while ($current_timestamp >= $checks_timestamp) {
    // This loop updates the checks timestamp to a value larger than the current time.
    // This avoids calling many checking routines when the routines have not been run for some time.
    $checks_timestamp += 86400;
  }
  $config_general->setTimerChecks ($checks_timestamp);
  $workingdirectory = dirname (__FILE__);
  shell_exec ("cd $workingdirectory; php checks.php > /dev/null 2>&1 &");
}
unset ($checks_timestamp);


function shutdown()
{
}


?>
