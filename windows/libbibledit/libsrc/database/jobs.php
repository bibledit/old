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


// Database resilience: 
// The database does not contain important information.
// Re-create it on corruption.


class Database_Jobs
{
  /**
  * Singleton logic.
  */
  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("jobs");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Jobs ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS jobs (
  id integer,
  timestamp integer,
  level integer,
  progress text,
  result text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize () 
  {
    Database_SQLite::exec ($this->db, "VACUUM jobs;");
  }


  public function trim ()
  {
    $timestamp = strtotime ("-30 days");
    $query = "DELETE FROM jobs WHERE timestamp < $timestamp;";
    Database_SQLite::exec ($this->db, $query);
  }
  

  public function idExists ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT id FROM jobs WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  // Gets a new unique identifier.
  public function getNewId ()
  {
    // Iterate till a non-existing identifier is found.
    do {
      $id = rand (100000000, 999999999);
    } while ($this->idExists ($id));
    // Store the new id so it can't be given out again just now.
    // Also store the timestamp. used for entry expiry.
    $timestamp = time ();
    $query = "INSERT INTO jobs (id, timestamp) VALUES ($id, $timestamp);";
    Database_SQLite::exec ($this->db, $query);
    return $id;
  }
  
  
  public function setLevel ($id, $level)
  {
    $id = Database_SQLiteInjection::no ($id);
    $level = Database_SQLiteInjection::no ($level);
    $query = "UPDATE jobs SET level = $level WHERE id = $id;";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  public function getLevel ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT level FROM jobs WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row ['level'];
    }
    return false;
  }


  public function setProgress ($id, $progress)
  {
    $id = Database_SQLiteInjection::no ($id);
    $progress = Database_SQLiteInjection::no ($progress);
    $query = "UPDATE jobs SET progress = '$progress' WHERE id = $id;";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  public function getProgress ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT progress FROM jobs WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row ['progress'];
    }
    return false;
  }


  public function setResult ($id, $result)
  {
    $id = Database_SQLiteInjection::no ($id);
    $result = Database_SQLiteInjection::no ($result);
    $query = "UPDATE jobs SET result = '$result' WHERE id = $id;";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  public function getResult ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT result FROM jobs WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row ['result'];
    }
    return false;
  }


}



?>
