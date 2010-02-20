<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


/**
* This is a database for the various versification systems.
*/
class Database_Versifications
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Versifications();
    }
    return self::$instance;
  }

  public function verify () {
    $database_instance = Database_Instance::getInstance(true);
$str = <<<EOD
CREATE TABLE IF NOT EXISTS versification_names (
system int primary key,
name varchar(256)
);
EOD;
    $database_instance->mysqli->query ($str);
    $database_instance->mysqli->query ("OPTIMIZE TABLE versification_names;");
$str = <<<EOD
CREATE TABLE IF NOT EXISTS versification_data (
id int auto_increment primary key,
system int,
book int,
chapter int,
verse int
);
EOD;
    $database_instance->mysqli->query ($str);
    $database_instance->mysqli->query ("OPTIMIZE TABLE versification_data;");
  }


  public function import ()
  {
    include_once ("messages/messages.php");    
    message_information ("Importing versification systems:");
    $directory = "../versification";
    $handler = opendir($directory);
    while ($file = readdir($handler)) {
      $pieces = explode (".", $file);
      if ($pieces[1] == "xml") {
        $file = "$directory/$file";
        message_information ("- $pieces[0]");
        flush ();
        $this->importOldBibleditXmlFile ($file, $pieces[0]);
      }
    }
    closedir($handler);
  }

  private function importOldBibleditXmlFile ($file, $name)
  {
    // Delete old system if it is there, and create new one.
    $this->delete ($name);
    $id = $this->create ($name);
    $database_instance = Database_Instance::getInstance(true);
    $database_books = Database_Books::getInstance();
    $xml = simplexml_load_file ($file);
    foreach ($xml as $triad) {
      $book = $triad->book;
      $book = $database_books->getIdFromEnglish ($book);
      $chapter = $triad->chapter;
      $verse = $triad->verse;
      $query = "INSERT INTO versification_data VALUES (NULL, $id, $book, $chapter, $verse);";
      $database_instance->mysqli->query ($query);
    }
  }


  /**
    * delete - Delete a versification system.
    */      
  public function delete ($name)
  {
    $database_instance = Database_Instance::getInstance(true);
    $id = $this->getID ($name);
    $query = "DELETE FROM versification_names WHERE system = $id;";
    $database_instance->mysqli->query ($query);
    $query = "DELETE FROM versification_data WHERE system = $id;";
    $database_instance->mysqli->query ($query);
  }

  
  /**
    * getID - Returns the ID for a named versification system.
    */      
  public function getID ($name)
  {
    $database_instance = Database_Instance::getInstance(true);
    $name = Database_SQLInjection::no ($name);
    $query = "SELECT system FROM versification_names WHERE name = '$name';";
    $result = $database_instance->mysqli->query ($query);
    if ($result->num_rows == 0) {
      return 0;
    }
    $row = $result->fetch_row();
    return $row[0];
  }

  /**
    * create - Creates a new empty versification system.
    * Returns the new ID.
    */      
  public function create ($name)
  {
    // If the versification system already exists, return its ID.
    $id = $this->getID ($name);
    if ($id > 0) {
      return $id;
    }   
    // Get the first free ID.
    $database_instance = Database_Instance::getInstance(true);
    $query = "SELECT system FROM versification_names ORDER BY system DESC LIMIT 1;";
    $result = $database_instance->mysqli->query ($query);
    $id = 0;
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
    }
    $id++;
    // Create the empty system.
    $name = Database_SQLInjection::no ($name);
    $query = "INSERT INTO versification_names VALUES ($id, '$name');";
    $database_instance->mysqli->query ($query);
    // Return new ID.
    return $id;
  }

}



?>
