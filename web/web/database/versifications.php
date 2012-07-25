<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE versification_names;");
    $database_instance->runQuery ("OPTIMIZE TABLE versification_data;");
  }


  public function importBibleditXml ($contents, $name)
  {
    // Delete old system if it is there, and create new one.
    $this->delete ($name);
    $id = $this->create ($name);
    $database_instance = Database_Instance::getInstance();
    $database_books = Database_Books::getInstance();
    $xml = simplexml_load_string ($contents);
    foreach ($xml as $triad) {
      $book = $triad->book;
      $book = $database_books->getIdFromEnglish ($book);
      $chapter = $triad->chapter;
      $verse = $triad->verse;
      $query = "INSERT INTO versification_data VALUES (NULL, $id, $book, $chapter, $verse);";
      $database_instance->runQuery ($query);
    }
  }


  public function exportBibleditXmlFile ($name)
  {
    $database_books = Database_Books::getInstance();
    $xml = new SimpleXMLElement ("<bibledit-versification-system></bibledit-versification-system>");
    $versification_data = $this->getBooksChaptersVerses ($name);
    while ($row = $versification_data->fetch_assoc()) {
      $triad = $xml->addChild ("triad");
      $book = $row["book"];
      $book = $database_books->getEnglishFromId ($book);     
      $triad->addChild ("book", $book);
      $chapter = $row["chapter"];
      $triad->addChild ("chapter", $chapter);
      $verse = $row["verse"];
      $triad->addChild ("verse", $verse);
    }
    $domnode = dom_import_simplexml ($xml);
    $dom = new DOMDocument ("1.0", "UTF-8");
    $domnode = $dom->importNode ($domnode, true);
    $dom->appendChild ($domnode);
    $dom->formatOutput = true;
    $string = $dom->saveXML ();
    $string = htmlentities ($string);
    return $string;
  }

  
  /**
    * delete - Delete a versification system.
    */      
  public function delete ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $id = $this->getID ($name);
    $query = "DELETE FROM versification_names WHERE system = $id;";
    $database_instance->runQuery ($query);
    $query = "DELETE FROM versification_data WHERE system = $id;";
    $database_instance->runQuery ($query);
  }

  
  /**
    * getID - Returns the ID for a named versification system.
    */      
  public function getID ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $query = "SELECT system FROM versification_names WHERE name = '$name';";
    $result = $database_instance->runQuery ($query);
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
    // Get the first free ID starting from 1000.
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT system FROM versification_names ORDER BY system DESC LIMIT 1;";
    $result = $database_instance->runQuery ($query);
    $id = 0;
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
    }
    $id++;
    if ($id < 1000) $id = 1000;
    // Create the empty system.
    $name = Database_SQLInjection::no ($name);
    $query = "INSERT INTO versification_names VALUES ($id, '$name');";
    $database_instance->runQuery ($query);
    // Return new ID.
    return $id;
  }


  /**
    * getSystems - Returns an array of the available versification systems.
    */      
  public function getSystems ()
  {
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT name FROM versification_names ORDER BY name ASC;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $systems[] = $row[0];
    }
    return $systems;
  }


  /**
    * getBooksChaptersVerses - Returns the books, chapters, verses for the given versification system.
    */
  public function getBooksChaptersVerses ($name)
  {
    $id = $this->getID ($name);
    $query = "SELECT book, chapter, verse FROM versification_data WHERE system = $id ORDER BY book, chapter, verse ASC;";
    $database_instance = Database_Instance::getInstance();
    $result = $database_instance->runQuery ($query);
    return $result;
  }
  


}



?>
