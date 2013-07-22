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

  const mailer = 'mailer';
  const receiver = 'receiver';
  const sendreceive = 'sendreceive';
  const changes = 'changes';
  const checks = 'checks';
  const database = 'database';
  const backup = 'backup';
  const search = 'search';
  const exports = 'exports';
  const statistics = 'statistics';
  const manual = 'manual';


  public function getLogFilename ($which)
  {
    include ("paths/paths.php");
    $filename = sys_get_temp_dir () . "/$location" . "-" . "$which.log";
    return $filename;
  }


  public function registerLogfile ($command, $pid, $logfile, $log = true)
  {
    $pid = (int) $pid;
    $database_logger = Database_Logger::getInstance ();
    $database_logger->record ($pid, $logfile);
    if ($log) {
      $database_logs = Database_Logs::getInstance ();
      $database_logs->log ("log: command: $command - pid: $pid");
    }
  }


  public function handleUsedLogFiles ()
  {
    $database_logger = Database_Logger::getInstance ();
    $logfiles = $database_logger->get ();
    foreach ($logfiles as $pid => $filename) {
      if (file_exists ('/proc/'.$pid)) continue;
      $database_logger->erase ($pid);
      if (!file_exists ($filename)) continue;
      $lines = file ($filename);
      unlink ($filename);
      $database_logs = Database_Logs::getInstance ();
      foreach ($lines as $line) {
        $database_logs->log (basename ($filename) . ": " . $line);
      }
    }
  }
  
  
}


?>
