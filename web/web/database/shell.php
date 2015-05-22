<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


// This is a database for keeping track of shell processes.
// Resilience: It is hardly used. The risk of corruption is very low, and acceptable.


class Database_Shell
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("shell");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Shell();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS shell (
  name text,
  pid text,
  output text,
  run boolean
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM shell;");
  }


  public function trim ()
  {
    Database_SQLite::exec ($this->db, "DELETE FROM shell;");
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
    $name = Database_SQLiteInjection::no ($name);
    $pid = Database_SQLiteInjection::no ($pid);
    $output = Database_SQLiteInjection::no ($output);
    $query = "INSERT INTO shell VALUES ('$name', '$pid', '$output', 1);";
    Database_SQLite::exec ($this->db, $query);
  }


  /**
  * Gets the name of the output file belonging to a process.
  */
  public function getOutput ($name, $pid)
  {
    $name = Database_SQLiteInjection::no ($name);
    $pid = Database_SQLiteInjection::no ($pid);
    $query = "SELECT output FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "";
  }


  /**
  * Returns true if a process runs.
  */
  public function processRuns ($name, $pid)
  {
    $name = Database_SQLiteInjection::no ($name);
    $pid = Database_SQLiteInjection::no ($pid);
    $query = "SELECT run FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return  (boolean) $row[0];
    }
    return false;
  }


  /**
  * Stop a process, so that it shows up as finished.
  */
  public function stopProcess ($name, $pid)
  {
    $name = Database_SQLiteInjection::no ($name);
    $pid = Database_SQLiteInjection::no ($pid);
    $query = "UPDATE shell SET run = 0 WHERE name = '$name' AND pid = '$pid';";
    Database_SQLite::exec ($this->db, $query);
  }


  /**
  * Returns true if a process is done.
  */
  public function processDone ($name, $pid)
  {
    $name = Database_SQLiteInjection::no ($name);
    $pid = Database_SQLiteInjection::no ($pid);
    $query = "SELECT run FROM shell WHERE name = '$name' AND pid = '$pid';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return  !(boolean) $row[0];
    }
    return false;
  }


  /**
  * Remove a process' data from the database.
  */
  public function removeProcess ($name, $pid)
  {
    $name = Database_SQLiteInjection::no ($name);
    $pid = Database_SQLiteInjection::no ($pid);
    $query = "DELETE FROM shell WHERE name = '$name' AND pid = '$pid';";
    Database_SQLite::exec ($this->db, $query);
  }


  public function debug ()
  {
    $result = Database_SQLite::query ($this->db, "SELECT * from shell;");
    foreach ($result as $row) {
      for ($i = 0; $i <= 3; $i++) unset ($row[$i]);
      var_dump ($row);
    }
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
