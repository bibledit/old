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
* This is a database for the various book names.
*/
class Database_Books
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Books();
    }
    return self::$instance;
  }

  public function verify () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("DROP TABLE books;");
$str = <<<EOD
CREATE TABLE IF NOT EXISTS books (
sequence int auto_increment primary key,
id int,
english varchar(256),
osis varchar(10),
usfm varchar(3),
bibleworks varchar(10),
onlinebible varchar(10),
type varchar(20),
onechapter int
);
EOD;
    $database_instance->runQuery ($str);
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE books;");
  }


  public function import ()
  {
    $database_instance = Database_Instance::getInstance();
    include_once ("messages/messages.php");
    message_information ("Importing book names");
    $filename = "book/books.txt";
    $data = file($filename, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES | FILE_TEXT);
    foreach ($data as $line) {
      $pos = strpos ($line, "#");
      if ($pos !== false) continue;
      $record = explode ("|", $line);
      $id          = $record[0];
      $english     = $record[1];
      $osis        = $record[2];
      $usfm        = $record[3];
      $bibleworks  = $record[4];
      $onlinebible = $record[5];
      $type        = $record[6];
      $onechapter  = 0;
      if ($record[7] == "true") $onechapter = 1;
      $query = "INSERT INTO books VALUES (NULL, $id, '$english', '$osis', '$usfm', '$bibleworks', '$onlinebible', '$type', $onechapter);";
      $database_instance->runQuery ($query);
    }
  }


  public function getIdFromEnglish($english)
  {
    $database_instance = Database_Instance::getInstance();
    $english = Database_SQLInjection::no ($english);
    $query = "SELECT id FROM books WHERE english = '$english';";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows == 0) {
      return 0;
    }
    $row = $result->fetch_row ();
    return $row[0];
  }
      

  public function getEnglishFromId($id)
  {
    $database_instance = Database_Instance::getInstance();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT english FROM books WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows == 0) {
      return gettext ("Unknown");
    }
    $row = $result->fetch_row ();
    return $row[0];
  }

}



?>
