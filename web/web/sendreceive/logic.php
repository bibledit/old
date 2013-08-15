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


require_once ("bootstrap/bootstrap.php");


class SendReceive_Logic
{


  static public function start ()
  {
    $timer_logger = new Timer_Logger ();
    $workingdirectory = escapeshellarg (dirname (__FILE__));
    $logfilename = $timer_logger->getLogFilename (Timer_Logger::sendreceive);
    $command = "cd $workingdirectory; php sendreceive.php > $logfilename 2>&1 & echo $!";
    $pid = shell_exec ($command);
    $timer_logger->registerLogfile ($command, $pid, $logfilename);
  }


  static public function isRunning ()
  {
    $timer_logger = new Timer_Logger ();
    $database_logger = Database_Logger::getInstance ();
    $filename = $timer_logger->getLogFilename (Timer_Logger::sendreceive);
    $pid = $database_logger->getPID ($filename);
    return $pid;
  }

}


?>
