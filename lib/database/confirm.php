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


// Handles email and web page confirmations.
// Database resilience: It is used infrequently.
// It does not contain essential information.


class Database_Confirm
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("confirm");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Confirm();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS confirm (
  id integer,
  query text,
  timestamp integer,
  mailto text,
  subject text,
  body text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM confirm;");
  }


  /**
  * getNewID - returns a new unique confirmation ID as an integer
  */
  public function getNewID ()
  {
    do {
      $id = rand (100000000, 999999999);
    } while ($this->IDExists ($id));
    return $id;
  }


  /**
  * IDExists - returns true if the $id exists
  */
  public function IDExists ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT id FROM confirm WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  /**
  * store - stores a confirmation cycle
  */
  public function store ($id, $query, $to, $subject, $body)
  {
    $id        = Database_SQLiteInjection::no ($id);
    $query     = Database_SQLiteInjection::no ($query);
    $timestamp = time ();
    $to        = Database_SQLiteInjection::no ($to);
    $subject   = Database_SQLiteInjection::no ($subject);
    $body      = Database_SQLiteInjection::no ($body);
    $query     = "INSERT INTO confirm VALUES ($id, '$query', $timestamp, '$to', '$subject', '$body');";
    Database_SQLite::exec ($this->db, $query);
  }


  /**
  * searchID - search the database for an existing ID in $subject.
  * If it exists, it returns the ID number, else it returns 0.
  */
  public function searchID ($subject)
  {
    $query = "SELECT id FROM confirm;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $id = $row [0];
      $pos = strpos ($subject, $id);
      if ($pos !== false) {
        return $id;
      }
    }
    return 0;
  }


  /**
  * getQuery - Returns the query for $id.
  */
  public function getQuery ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT query FROM confirm WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  /**
  * getMailTo - Returns the To: address for $id.
  */
  public function getMailTo ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT mailto FROM confirm WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  /**
  * getSubject - Returns the Subject: for $id.
  */
  public function getSubject ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT subject FROM confirm WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  /**
  * getBody - Returns the email's body for $id.
  */
  public function getBody ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT body FROM confirm WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  /**
  * delete - Deletes $id from the table.
  */
  public function delete ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "DELETE FROM confirm WHERE id = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function trim ()
  {
    // Leave entries for 30 days.
    $time = time() - 2592000; 
    $query = "DELETE FROM confirm WHERE timestamp < $time;";
    Database_SQLite::exec ($this->db, $query);
  }


}



?>
