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


class Database_Consistency
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Consistency ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE consistency;");
    $database_instance->runQuery ("OPTIMIZE TABLE consistency;");
  }


  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    // Clear debris of the consistency checks.
    $database_instance->runQuery ("TRUNCATE TABLE consistency;");
  }   


  public function storePassagesTranslations ($id, $passages, $translations)
  {
    $id = Database_SQLInjection::no ($id);
    $passages = Database_SQLInjection::no ($passages);
    $translations = Database_SQLInjection::no ($translations);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM consistency WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    $query = "INSERT INTO consistency VALUES ($id, '$passages', '$translations', '');";
    $result = $database_instance->runQuery ($query);
  }
  
  
  public function getPassages ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT passages FROM consistency WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $passages = $row [0];
      return $passages;
    }
    return "";
  }
  
  
  public function getTranslations ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT translations FROM consistency WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $translations = $row [0];
      return $translations;
    }
    return "";
  }
  
  
  public function updateResponse ($id, $response)
  {
    $id = Database_SQLInjection::no ($id);
    $response = Database_SQLInjection::no ($response);
    $database_instance = Database_Instance::getInstance();
    $query = "UPDATE consistency SET response = '$response' WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
  }
  
  
  public function getResponse ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT response FROM consistency WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $response = $row [0];
      return $response;
    }
    return "";
  }


}



?>
