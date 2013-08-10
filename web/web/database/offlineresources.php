<?php


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
