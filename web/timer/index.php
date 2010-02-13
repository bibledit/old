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

  // Run once a minute.
  sleep(1); // Todo should be 60 seconds (or 10 seconds, then check from genconfig whether the 60 seconds have passed).
}


function shutdown()
{
  $crontable = Database_Cron::getInstance ();
  $crontable->clearFlags ();
  $log = Database_Logs::getInstance();
  $log->log ("Timer shutdown");
}






/*




Todo admininistrator needs to view the logfiles.

 


// Run once a minute only.
$config_general = Database_Config_General::getInstance ();
$previous_timestamp = $config_general->getPingTimeStamp ();
$current_timestamp = time ();

if ($current_timestamp < ($previous_timestamp + 60)) {
  die;
}
$config_general->setPingTimeStamp ($current_timestamp);



*/



?>
