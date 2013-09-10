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


class Database_Menu
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Menu ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE menu;");
    $database_instance->runQuery ("OPTIMIZE TABLE menu;");
  }
  

  public function getAccessCount ($user, $category) {
    $user = Database_SQLInjection::no ($user);
    $category = Database_SQLInjection::no ($category);
    $query = "SELECT hits FROM menu WHERE username = '$user' AND category = '$category';";
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ($query);
    if ($result->num_rows == 0) return 0;
    $result_array = $result->fetch_array();
    $count = $result_array['hits'];
    $count = (int) $count;
    return $count;
  }   


  public function increaseAccessCount ($user, $category) {
    $count = $this->getAccessCount ($user, $category);
    $user = Database_SQLInjection::no ($user);
    $category = Database_SQLInjection::no ($category);
    if ($count == 0) {
      $query = "INSERT INTO menu VALUES (NULL, '$user', '$category', 1)";
    } else {
      $count++;
      $query = "UPDATE menu SET hits = $count WHERE username = '$user' AND category = '$category';";
    }
    $server = Database_Instance::getInstance ();
    $server->runQuery ($query);
  }   


  public function clearAllAccessCounts ($user) {
    $user = Database_SQLInjection::no ($user);
    $server = Database_Instance::getInstance ();
    $query = "DELETE FROM menu WHERE username = '$user'";
    $result = $server->runQuery ($query);
  }


}


?>
