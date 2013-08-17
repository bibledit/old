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


class Database_Logger
{
  // Singleton logic.
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Logger ();
    }
    return self::$instance;
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE logger;");
  }


  public function record ($pid, $filename)
  {
    $server = Database_Instance::getInstance();
    $pid = Database_SQLInjection::no ($pid);
    $filename = Database_SQLInjection::no ($filename);
    $query = "INSERT INTO logger VALUES ($pid, '$filename');";
    $server->runQuery ($query);
  }


  public function getAll ()
  {
    $data = array ();
    $server  = Database_Instance::getInstance ();
    $query = "SELECT pid, logfile FROM logger;";
    $entries = $server->runQuery ($query);
    while ($row = $entries->fetch_assoc()) {
      $pid = $row["pid"];
      $filename = $row["logfile"];
      $data [$pid] = $filename;
    }
    return $data;
  }


  public function getPID ($logfile)
  {
    $server  = Database_Instance::getInstance ();
    $logfile = Database_SQLInjection::no ($logfile);
    $query = "SELECT pid FROM logger WHERE logfile = '$logfile';";
    $entries = $server->runQuery ($query);
    while ($row = $entries->fetch_assoc()) {
      return $row["pid"];
    }
    return 0;
  }


  public function erase ($pid)
  {
    $server  = Database_Instance::getInstance ();
    $pid = Database_SQLInjection::no ($pid);
    $query   = "DELETE FROM logger WHERE pid = $pid;";
    $server->runQuery ($query);
  }


}


?>
