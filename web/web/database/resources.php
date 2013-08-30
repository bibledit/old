<?php


class Database_Resources
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Resources ();
    }
    return self::$instance;
  }


  public function optimize () 
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE resources;");
    $database_instance->runQuery ("OPTIMIZE TABLE resources;");
  }


  public function save ($name, $command, $code)
  {
    $name = Database_SQLInjection::no ($name);
    $command = Database_SQLInjection::no ($command);
    $code = Database_SQLInjection::no ($code);
    $this->delete ($name);
    $database_instance = Database_Instance::getInstance();
    $query = "INSERT INTO resources VALUES (NULL, '$name', '$command', '$code');";
    $result = $database_instance->runQuery ($query);
  }


  public function getNames ()
  {
    $names = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT name FROM resources ORDER by name;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $names [] = $row [0];
    }
    return $names;
  }


  public function delete ($name)
  {
    $name = Database_SQLInjection::no ($name);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM resources WHERE name = '$name';";
    $result = $database_instance->runQuery ($query);
  }


  public function getDetails ($name)
  {
    $name = Database_SQLInjection::no ($name);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT command, code FROM resources WHERE name = '$name';";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      return $result->fetch_assoc ();
    }
  }


}



?>
