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


/*
 * Snapshots preserve the history of a chapter being edited.
 * 
 * When a Bible, book or chapter gets deleted, snapshots are not deleted. 
 * If a book was deleted by mistake, it can then be restored easily by consulting the snapshots taken.
*/


class Database_Snapshots
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Snapshots();
    }
    return self::$instance;
  }

  /**
  * Optimize database table.
  */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE snapshots;");
    $database_instance->runQuery ("OPTIMIZE TABLE snapshots;");
  }

  /**
  * Takes a snapshot of a chapter.
  * $persistent: True or false. 
  * If true, this snapshot will be regarded more persistent than usual, it lasts longer before getting removed.
  */
  public function snapChapter ($name, $book, $chapter, $data, $persistent)
  {
    $database_bibles = Database_Bibles::getInstance();
    $bible_id = $database_bibles->getID ($name);
    if ($bible_id == 0) {
      $database_logs = Database_Logs::getInstance();
      $database_logs->log ("Bible $name does not exist: Failed to snap a chapter");
      return;
    }
    $database_instance = Database_Instance::getInstance();
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $data = Database_SQLInjection::no ($data);
    $time = time ();
    $persistent = Filter_Bool::int ($persistent);
    $query = "INSERT INTO snapshots VALUES (NULL, $bible_id, $book, $chapter, '$data', $time, $persistent);";
    $database_instance->runQuery ($query);
  }


  /**
    * Trims the snapshots.
    */
  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    // Remove entries after 30 days.
    $time = time () - 2592000; 
    $query = "DELETE FROM snapshots WHERE seconds < $time;";
    $result = $database_instance->runQuery ($query);
  }


}



?>
