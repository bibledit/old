<?php


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
    $timestamp = time ();
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
    $query .= " ORDER BY bible ASC, book ASC, chapter ASC, verse ASC;";
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


}



?>
