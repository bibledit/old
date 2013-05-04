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




class Timer_Logger
{

  const sendreceive = 'sendreceive';
  const changes = 'changes';
  const checks = 'checks';
  const trimdatabases = 'trimdatabases';
  const backup = 'backup';
  const search = 'search';


  public function getLogFilename ($which)
  {
    include ("paths/paths.php");
    $filename = sys_get_temp_dir () . "/$location" . "-" . "$which.log";
    return $filename;
  }


  public function handleUsedLogFiles ()
  {
    $reflectionClass = new ReflectionClass ('Timer_Logger');
    $constants = $reflectionClass->getConstants ();
    foreach ($constants as $constant) {
      $this->handleLogs ($this->getLogFilename ($constant));
    }
    unset ($reflectionClass);
  }
  
  
  private function handleLogs ($logfile)
  {
    if (!file_exists ($logfile)) return;
    $result = exec ("lsof +t $logfile > /dev/null");
    if ($result != 0) return;
    $lines = file ($logfile);
    unlink ($logfile);
    $database_logs = Database_Logs::getInstance (); // Todo temporal.
    foreach ($lines as $line) {
      $database_logs->log (basename ($logfile) . ": " . $line);
    }
  }
  
  
}


?>
