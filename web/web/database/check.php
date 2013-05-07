<?php


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
    $verse = Database_SQLInjection::no ($verse);
    $data = Database_SQLInjection::no ($data);
    $database_instance = Database_Instance::getInstance();
    $query = "INSERT INTO check_output VALUES (NULL, $bible, $book, $chapter, $verse, '$data');";
    $result = $database_instance->runQuery ($query);
  }


/*
 // Todo update for retrieving data.
  public function getBibles () 
  {
    $bibles = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT name FROM bible_names ORDER BY name ASC;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $bibles[] = $row[0];
    }
    return $bibles;
  }
*/



}



?>
