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
* This is a database for storing output of git operations.
* This is used for updating Bibles and Notes.
*/

class Database_Git
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Git();
    }
    return self::$instance;
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE git;");
  }

  
  public function insert ($directory, $output)
  {
    $directory = Database_SQLInjection::no ($directory);
    $output = Database_SQLInjection::no ($output);
    $server = Database_Instance::getInstance ();
    $server->runQuery ("INSERT INTO git VALUES ('$directory', '$output');");
  }


  public function get ()
  {
    $data = NULL;
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ("SELECT directory, output FROM git LIMIT 1;");
    if ($result->num_rows > 0) {
      $data = $result->fetch_assoc();
    }
    return $data;
  }


  public function delete ($directory, $output)
  {
    $directory = Database_SQLInjection::no ($directory);
    $output = Database_SQLInjection::no ($output);
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ("DELETE FROM git WHERE directory = '$directory' AND output = '$output';");
  }


}


?>
