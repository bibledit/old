<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


// Only runs through the cli Server API.
if (php_sapi_name () != "cli") {
  die;
}


// Change user Id of process running this script.
$pwnam = posix_getpwnam ("www-data");
posix_setuid ($pwnam['uid']);
posix_setgid ($pwnam['gid']);
posix_seteuid ($pwnam['uid']);
posix_setegid ($pwnam['gid']);


ignore_user_abort(true);
set_time_limit(0);
register_shutdown_function('shutdown');


$log = Database_Logs::getInstance();
$config_general = Database_Config_General::getInstance ();
//$processUser = posix_getpwuid(posix_geteuid());
//$processUser = $processUser['name'];


$current_timestamp = time ();


// CPU-intensive actions run at midnight.
// The advantage of this is that while the site is in use during the day, 
// CPU-intensive actions do not slow down the site.
// There have been cases that somebody submitted a comment to a consultation note, 
// but since the site didn't respond fast enough, 
// the person kept pressing the submit button,
// with the result that the comment was added multiple times to the note.
$midnight = (date ('Gi') == 0);



// Mailer is done once a minute.
$timer_mailer = new Timer_Mailer ();
$timer_mailer->run ();
unset ($timer_mailer);


$previous_timestamp = $config_general->getTimerFiveMinutes ();
if ($current_timestamp >= ($previous_timestamp + 300)) {
  $config_general->setTimerFiveMinutes ($current_timestamp);
  
  // Tasks to be done each five minutes:

  $timer_receiver = new Timer_Receiver ();
  $timer_receiver->run ();
  unset ($timer_receiver);
  
}


// Trim databases at midnight.
if ($midnight) {
  $workingdirectory = dirname (__FILE__);
  shell_exec ("cd $workingdirectory; php trimdatabases.php > /dev/null 2>&1 &");
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
if (($current_timestamp >= $diff_timestamp) || $midnight) {
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


function shutdown()
{
}


?>
