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


class Sprint_Logic
{


  // This function runs the sprint burndown history logger.
  // If $mail is true, it will mail the burndown chart to the subscribed users.
  // If $mail is false, it decides on its own whether to mail the chart to the users.
  static public function burndown ($mail)
  {
    $timer_logger = new Timer_Logger ();
    $workingdirectory = escapeshellarg (dirname (__FILE__));
    $logfilename = $timer_logger->getLogFilename (Timer_Logger::burndown);
    $mail = Filter_Bool::int ($mail);
    $command = "cd $workingdirectory; php burndown.php $mail > $logfilename 2>&1 & echo $!";
    $pid = shell_exec ($command);
    $timer_logger->registerLogfile ($command, $pid, $logfilename);
  }


}


?>
