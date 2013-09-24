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


class Database_Changes
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Changes ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE changes;");
    $database_instance->runQuery ("OPTIMIZE TABLE changes;");
  }


  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    $time = time () - 7776000; // Remove entries after 90 days.
    $query = "DELETE FROM changes WHERE timestamp < $time;";
    $database_instance->runQuery ($query);
  }   


  public function record ($users, $bible, $book, $chapter, $verse, $oldtext, $modification, $newtext)
  {
    $bible = Database_SQLInjection::no ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    if ($verse == "") $verse = 0;
    $verse = Database_SQLInjection::no ($verse);
    $oldtext = Database_SQLInjection::no ($oldtext);
    $modification = Database_SQLInjection::no ($modification);
    $newtext = Database_SQLInjection::no ($newtext);
    $database_instance = Database_Instance::getInstance();
    // Normally this function is called just after midnight.
    // It would then put the current time on changes made the day before.
    // Make a correction for that by subtracting 6 hours.
    $timestamp = time () - 21600;
    foreach ($users as $user) {
      $user = Database_SQLInjection::no ($user);
      $query = "INSERT INTO changes VALUES (NULL, $timestamp, '$user', $bible, $book, $chapter, $verse, '$oldtext', '$modification', '$newtext');";
      $result = $database_instance->runQuery ($query);
    }
  }


  public function getIdentifiers ($username = "")
  {
    $query = "SELECT id FROM changes ";
    if ($username != "") {
      $username = Database_SQLInjection::no ($username);
      $query .= " WHERE username = '$username' ";
    }
    // Sort on reference, so that related changes are near each other.
    $query .= " ORDER BY book ASC, chapter ASC, verse ASC, id ASC;";
    $database_instance = Database_Instance::getInstance();
    $result = $database_instance->runQuery ($query);
    $ids = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $ids [] = $row [0];
    }
    return $ids;
  }


  public function delete ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
  }


  public function getTimeStamp ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT timestamp FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $timestamp = $result->fetch_row ();
      $timestamp = $timestamp [0];
      return $timestamp;
    }
    return time ();
  }


  public function getBible ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT bible FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $bible = $result->fetch_row ();
      $bible = $bible [0];
      return $bible;
    }
    return 0;
  }


  public function getPassage ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT book, chapter, verse FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      return $result->fetch_assoc ();
    }
    return NULL;
  }


  public function getOldText ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT oldtext FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $old_text = $result->fetch_row ();
      $old_text = $old_text [0];
      return $old_text;
    }
    return "";
  }


  public function getModification ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT modification FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $modification = $result->fetch_row ();
      $modification = $modification [0];
      return $modification;
    }
    return NULL;
  }


  public function getNewText ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT newtext FROM changes WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $new_text = $result->fetch_row ();
      $new_text = $new_text [0];
      return $new_text;
    }
    return "";
  }


  public function clearUser ($username)
  {
    $username = Database_SQLInjection::no ($username);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM changes WHERE username = '$username';";
    $result = $database_instance->runQuery ($query);
  }


}



?>
