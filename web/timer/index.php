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



/**
* Flag stores the pid. 
* Ensure that only one instance of the script runs at any time.
*/
$crontable = Database_Cron::getInstance ();
if ($crontable->getFlag ()) die;
$crontable->setFlag ();


ignore_user_abort(true);
set_time_limit(0);
register_shutdown_function('shutdown');
header ("Connection: Close");
header ("Cache-Control: max-age=3600, must-revalidate");
header ("Content-Length: 0");
header ("Status: 404 Not Found"); 
header ("HTTP/1.0 404 Not Found"); 


/**
* Run the loop
*/
$log = Database_Logs::getInstance();
while(1)
{
  $log->log ("Minutely maintenance routine started");
  $log->log ("Minutely maintenance routine finished");

  // Run once a minute.
  sleep(60);
}


function shutdown()
{
  $crontable = Database_Cron::getInstance ();
  $crontable->clearFlag ();
}


/*






// 


// Run once a minute only.
$config_general = Database_Config_General::getInstance ();
$previous_timestamp = $config_general->getPingTimeStamp ();
$current_timestamp = time ();

if ($current_timestamp < ($previous_timestamp + 60)) {
  die;
}
$config_general->setPingTimeStamp ($current_timestamp);


$log = Database_Logs::getInstance();
$log->log ("Minutely maintenance routine started");
$log->log ("Minutely maintenance routine finished");


*/

?>
