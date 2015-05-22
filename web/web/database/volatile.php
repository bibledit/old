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


// Database resilience: It is recreated every night.


class Database_Volatile
{

  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("volatile");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Volatile ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS volatile (
  id integer,
  key text,
  value text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
    $sql = "DELETE FROM volatile;";
    Database_SQLite::exec ($this->db, $sql);
  }


  public function getKeys ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $keys = array ();
    $query = "SELECT key FROM volatile WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $keys [] = $row [0];
    }
    return $keys;
  }


  public function getValue ($id, $key)
  {
    $id = Database_SQLiteInjection::no ($id);
    $key = Database_SQLiteInjection::no ($key);
    $query = "SELECT value FROM volatile WHERE id = $id AND key = '$key';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  public function setValue ($id, $key, $value)
  {
    $id = Database_SQLiteInjection::no ($id);
    $key = Database_SQLiteInjection::no ($key);
    $value = Database_SQLiteInjection::no ($value);
    $query = "DELETE FROM volatile WHERE id = $id AND key = '$key';";
    Database_SQLite::exec ($this->db, $query);
    $query = "INSERT INTO volatile VALUES ($id, '$key', '$value');";
    Database_SQLite::exec ($this->db, $query);
  }


}


?>
