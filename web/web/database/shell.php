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


/**
* This is a database in memory for keeping track of shell processes.
* Memory is chosen since the information in it is volatile, which is what is needed,
* i.e. if the server reboots, all information should be gone since all processes
* would have been killed on server shutdown.
*/

class Database_Shell
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Shell();
    }
    return self::$instance;
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE shell;");
    $database_instance->runQuery ("TRUNCATE TABLE shell;");
    $database_instance->runQuery ("OPTIMIZE TABLE shell;");
  }
  
  /**
  * Enter process data.
  * $name - name by which the process is identified.
  * $pid - the process' ID, if it is used.
  * $output - the name of the file where the process writes its output to.
  */
  public function enterProcess ($name, $pid, $output)
  {
    $this->removeProcess ($name, $pid); 
    $name = Database_SQLInjection::no ($name);
    $pid = Database_SQLInjection::no ($pid);
    $output = Database_SQLInjection::no ($output);
    $server = Database_Instance::getInstance ();
    $query = "INSERT INTO shell VALUES ('$name', '$pid', '$output', 1);";
    $server->runQuery ($query);
  }

  /**
  * gets the name of the output file belonging to a process.
  */
  public function getOutput ($name, $pid)
  {
    $server = Database_Instance::getInstance ();
    $query = "SELECT output FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = $server->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row ();
      return $row[0];
    }
    return "";
  }

  /**
  * returns true if a process runs.
  */
  public function processRuns ($name, $pid)
  {
    $server = Database_Instance::getInstance ();
    $query = "SELECT run FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = $server->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row ();
      if ($row[0] != 0) {
        return true;
      }
    }
    return false;
  }

  /**
  * stop a process, so that it shows up as finished.
  */
  public function stopProcess ($name, $pid)
  {
    $name = Database_SQLInjection::no ($name);
    $pid = Database_SQLInjection::no ($pid);
    $server = Database_Instance::getInstance ();
    $query = "UPDATE shell SET run = 0 WHERE name = '$name' AND pid = '$pid';";
    $server->runQuery ($query);
  }

  /**
  * returns true if a process is done.
  */
  public function processDone ($name, $pid)
  {
    $server = Database_Instance::getInstance ();
    $query = "SELECT run FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = $server->runQuery ($query);
    if ($result->num_rows == 0) {
      return false;
    } else {
      $row = $result->fetch_row ();
      return !$row[0];
    }

  }
  
  /**
  * remove a process' data from the database.
  */
  public function removeProcess ($name, $pid)
  {
    $server = Database_Instance::getInstance ();
    $query = "DELETE FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = $server->runQuery ($query);
  }


  /**
  * This does some logic.
  * $name: the name of the process. This can be anything, as desired by the caller.
  * $pid: the process identifier. May be 0.
  * $output: The name of the file where the process should pipe its output.
  * Returns a value to indicate the shell process status:
  *  1: Start process.
  *  0: Process is still running.
  * -1: Process has finished.
  */
  public function logic ($name, $pid, & $output)
  {
    // The name of the output file (it may not yet have been set).
    $output = $this->getOutput ($name, $pid);
    
    // Process done handling.
    if ($this->processDone ($name, $pid)) {
      $this->removeProcess ($name, $pid);
      return -1;
    } 

    // The page should refresh since the shell process is still active or should become active.
    header ("Refresh: 2");

    // Handler for the case that the shell process still runs.
    if ($this->processRuns ($name, $pid)) {
      return 0;
    }
    
    // Handling at the start of the shell process.
    $output = tempnam (sys_get_temp_dir(), '');
    $this->enterProcess ($name, $pid, $output);
    return 1;
  }
  


}



?>
