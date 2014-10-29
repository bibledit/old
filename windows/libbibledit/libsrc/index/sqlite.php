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


class Database_SQLite {


  public static function connect ($database)
  {
    $file = dirname (__DIR__) . "/databases/$database.sqlite";
    try {
      $db = new PDO ("sqlite:$file");
    } catch (PDOException $e) {
      $message = $e->getMessage () . " " . $file;
      echo "<p>$message</p>";
    }
    return $db;
  }


  public static function exec ($db, $sql)
  {
    $success = $db->exec ($sql);
    if ($success === false) {
      $message = $db->errorInfo ();
      $message = "Database error " . $message [0] . " - " . $message [2] . " - Query was: " . $sql;
      echo "<p>$message</p>";
    }
  }


  public static function query ($db, $sql)
  {
    // Run the SQL on the database handle.
    $success = $result = $db->query ($sql);
    // Check / log errors.
    if ($success === false) {
      $message = $db->errorInfo ();
      $message = "Database error: " . $message [0] . " - " . $message [2] . " - Query was: " . $sql;
      echo "<p>$message</p>";
    }
    // In case of errors, the $result won't be an array.
    // Make it an ampty array so it won't produce further errors down the code.
    if (!$result) {
      $result = array ();
    }
    return $result;
  }


}


class Database_Users
{
  private static $instance;
  public $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("test");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Users();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS users (
  username text,
  password text,
  level integer,
  email text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function addNewUser ($username, $password, $level, $email) 
  {
    $query = "INSERT INTO users (username, level, email) VALUES ('$username', $level, '$email');";
    Database_SQLite::exec ($this->db, $query);
  }


  // Returns true if the username exists in the database.
  public function usernameExists ($user)
  {
    $query = "SELECT * FROM users WHERE username = '$user'";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  public function debug ()
  {
    $query = "SELECT * FROM users;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      var_dump ($row);
    }
  }


}


$database_users = Database_Users::getInstance ();
$database_users->create ();
$database_users->addNewUser ("username", "password", 123, "email@address.com");
$database_users->debug ();




?>
