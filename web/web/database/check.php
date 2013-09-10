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


class Database_Check
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Check ();
    }
    return self::$instance;
  }


  public function optimize () 
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE check_output;");
    $database_instance->runQuery ("REPAIR TABLE check_suppress;");
    $database_instance->runQuery ("OPTIMIZE TABLE check_output;");
    $database_instance->runQuery ("OPTIMIZE TABLE check_suppress;");
  }


  public function truncateOutput ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("TRUNCATE TABLE check_output;");
  }


  public function recordOutput ($bible, $book, $chapter, $verse, $data)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $bible = $database_bibles->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    if ($verse == "") $verse = 0;
    $verse = Database_SQLInjection::no ($verse);
    $data = Database_SQLInjection::no ($data);
    $database_instance = Database_Instance::getInstance();
    $query = "INSERT INTO check_output VALUES (NULL, $bible, $book, $chapter, $verse, '$data');";
    $result = $database_instance->runQuery ($query);
  }


  public function getHits ()
  {
    $hits = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT * FROM check_output WHERE NOT EXISTS (SELECT * FROM check_suppress WHERE check_output.bible=check_suppress.bible AND check_output.book=check_suppress.book AND check_output.chapter=check_suppress.chapter AND check_output.verse=check_suppress.verse AND check_output.data=check_suppress.data);";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_assoc ();
      $hits [] = $row;
    }
    return $hits;
  }


  public function approve ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    // The query copies all values, apart from the auto_increment id.
    $query = "INSERT INTO check_suppress (bible, book, chapter, verse, data) SELECT bible, book, chapter, verse, data FROM check_output WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
  }


  public function delete ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM check_output WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
  }


  public function getPassage ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT book, chapter, verse FROM check_output WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      return $result->fetch_assoc ();
    }
    return NULL;
  }


  public function getSuppressions ()
  {
    $suppressions = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT * FROM check_suppress;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_assoc ();
      $suppressions [] = $row;
    }
    return $suppressions;
  }


  public function release ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM check_suppress WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
  }


}



?>
