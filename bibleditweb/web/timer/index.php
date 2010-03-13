<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
// No check on page level access because everybody should access this Poor Man's Crontab.


$crontable = Database_Cron::getInstance ();


// If the cron database indicates that the timer script is running, bail out.
$pid = $crontable->getPID ();
if ($pid != 0) {
  die;
}


// Ok, we're going to run: Set the stage.
ignore_user_abort(true);
set_time_limit(0);
register_shutdown_function('shutdown');
$crontable->setPID ();
$log = Database_Logs::getInstance();
$log->log ("Timer start");


// Run the loop.
while(1)
{
  $shutdown = $crontable->getShutdown ();
  if ($shutdown > 0) {
    $log->log ("Shutdown request: Timer stop");
    die;
  }  


  $config_general = Database_Config_General::getInstance ();
  $current_timestamp = time ();


  $previous_timestamp = $config_general->getTimerMinute ();
  if ($current_timestamp >= ($previous_timestamp + 60)) {
    $config_general->setTimerMinute ($current_timestamp);
    
    // Tasks to be done once a minute come here:
    
    // Mailer.
    $timer_mailer = new Timer_Mailer ();
    $timer_mailer->run ();
    unset ($timer_mailer);
    
  }


  $previous_timestamp = $config_general->getTimerFiveMinutes ();
  if ($current_timestamp >= ($previous_timestamp + 300)) {
    $config_general->setTimerFiveMinutes ($current_timestamp);
    
    // Tasks to be done each five minutes come here:

    $timer_receiver = new Timer_Receiver ();
    $timer_receiver->run ();
    unset ($timer_receiver);
    
  }


  $previous_timestamp = $config_general->getTimerDay ();
  if ($current_timestamp >= ($previous_timestamp + 86400)) {
    $config_general->setTimerDay ($current_timestamp);
    
    // Tasks to be done once a day come here:

    // Trim snapshots.
    $database_snapshots = Database_Snapshots::getInstance();
    $database_snapshots->trim(false);

    // Log memory usage.    
    $memory_usage = memory_get_usage();
    $log->log ("Memory usage is $memory_usage bytes");
    
  }


  // Wait a little for the next cycle.
  sleep(5);
}


function shutdown()
{
  $crontable = Database_Cron::getInstance ();
  $crontable->clearFlags ();
  $log = Database_Logs::getInstance();
  $log->log ("Timer shutdown");
}


?>
