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


class Database_Sprint
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Sprint ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE sprint;");
    $database_instance->runQuery ("OPTIMIZE TABLE sprint;");
  }


  public function storeTask ($year, $month, $title)
  {
    $year = Database_SQLInjection::no ($year);
    $month = Database_SQLInjection::no ($month);
    $title = Database_SQLInjection::no ($title);
    $database_instance = Database_Instance::getInstance();
    $query = "INSERT INTO sprint VALUES (NULL, $year, $month, '$title', '', 0);";
    $database_instance->runQuery ($query);
  }
  
  
  public function deleteTask ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM sprint WHERE id = $id;";
    $database_instance->runQuery ($query);
  }


  public function getTasks ($year, $month)
  {
    $year = Database_SQLInjection::no ($year);
    $month = Database_SQLInjection::no ($month);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT id FROM sprint WHERE year = $year AND month = $month ORDER BY id ASC;";
    $result = $database_instance->runQuery ($query);
    $ids = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $ids [] = $row [0];
    }
    return $ids;
  }


  public function getTitle ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT title FROM sprint WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      return $row [0];
    }
    return "";
  }


  public function updateComplete ($id, $percentage)
  {
    $id = Database_SQLInjection::no ($id);
    $percentage = Database_SQLInjection::no ($percentage);
    $database_instance = Database_Instance::getInstance();
    $query = "UPDATE sprint SET complete = $percentage WHERE id = $id;";
    $database_instance->runQuery ($query);
  }


  public function getComplete ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT complete FROM sprint WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      return $row [0];
    }
    return "";
  }


  public function updateMonthYear ($id, $month, $year)
  {
    $id = Database_SQLInjection::no ($id);
    $month = Database_SQLInjection::no ($month);
    $year = Database_SQLInjection::no ($year);
    $database_instance = Database_Instance::getInstance();
    $query = "UPDATE sprint SET month = $month, year = $year WHERE id = $id;";
    $database_instance->runQuery ($query);
  }


  public function logHistory ($year, $month, $day, $tasks, $complete)
  {
    $year = Database_SQLInjection::no ($year);
    $month = Database_SQLInjection::no ($month);
    $day = Database_SQLInjection::no ($day);
    $tasks = Database_SQLInjection::no ($tasks);
    $complete = Database_SQLInjection::no ($complete);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM sprinthistory WHERE year = $year AND month = $month AND day = $day;";
    $database_instance->runQuery ($query);
    $query = "INSERT INTO sprinthistory VALUES (NULL, $year, $month, $day, $tasks, $complete);";
    $database_instance->runQuery ($query);
  }


  public function getHistory ($year, $month)
  {
    $year = Database_SQLInjection::no ($year);
    $month = Database_SQLInjection::no ($month);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT day, tasks, complete FROM sprinthistory WHERE year = $year AND month = $month ORDER BY day ASC;";
    $result = $database_instance->runQuery ($query);
    $history = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_assoc ();
      $history [] = $row;
    }
    return $history;
  }
  
  
  public function clearHistory ($year, $month)
  {
    $year = Database_SQLInjection::no ($year);
    $month = Database_SQLInjection::no ($month);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM sprinthistory WHERE year = $year AND month = $month;";
    $database_instance->runQuery ($query);
  }

  
}


?>
