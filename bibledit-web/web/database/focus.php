<?php


class Database_Focus
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Focus();
    }
    return self::$instance;
  }


  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS focus (
id int auto_increment primary key,
channel varchar(256),
book int,
chapter int,
verse int
);
EOD;
    $database_instance->runQuery ($str);
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE focus;");
  }


  public function storeFocus ($book, $chapter, $verse)
  {
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $verse = Database_SQLInjection::no ($verse);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM focus WHERE channel = '';";
    $database_instance->runQuery ($query);
    $query = "INSERT INTO focus VALUES (NULL, '', $book, $chapter, $verse);";
    $database_instance->runQuery ($query);
  }


}



?>
