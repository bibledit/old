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


class Database_OfflineResources
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_OfflineResources ();
    }
    return self::$instance;
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE offlineresources;");
    $database_instance->runQuery ("OPTIMIZE TABLE offlineresources;");
  }


  public function delete ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $query = "DELETE FROM offlineresources WHERE name = '$name';";
    $database_instance->runQuery ($query);
  }

  
  public function store ($name, $book, $chapter, $verse, $html)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $verse = Database_SQLInjection::no ($verse);
    $html = Database_SQLInjection::no ($html);
    $query = "INSERT INTO offlineresources VALUES (NULL, '$name', $book, $chapter, $verse, '$html');";
    $database_instance->runQuery ($query);
  }


  public function exists ($name, $book, $chapter, $verse)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $verse = Database_SQLInjection::no ($verse);
    $query = "SELECT count(*) FROM offlineresources WHERE name = '$name' AND book = $book AND chapter = $chapter AND verse = $verse;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return ($row [0] == 1);
    }
    return false;
  }
  

  // Counts the number of verses in the offline resource $name.
  public function count ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $query = "SELECT count(*) FROM offlineresources WHERE name = '$name';";
    $result = $database_instance->runQuery ($query);
    $row = $result->fetch_row();
    return $row [0];
  }
  

  public function get ($name, $book, $chapter, $verse)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $verse = Database_SQLInjection::no ($verse);
    $query = "SELECT html FROM offlineresources WHERE name = '$name' AND book = $book AND chapter = $chapter AND verse = $verse;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return $row[0];
    }
    return "";    
  }
      

}



?>
