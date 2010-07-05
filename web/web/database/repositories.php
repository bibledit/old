<?php


class Database_Repositories
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Repositories();
    }
    return self::$instance;
  }

  /**
  * Verify database table, optionally creating and/or optimizing it
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS repositories (
bible varchar(256),
data longblob
);
EOD;
    $database_instance->runQuery ($str);
    $database_instance->runQuery ("ALTER TABLE repositories ADD part int AFTER bible;");
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE repositories;");
  }


  public function storeRepository ($bible, $part, $data)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    $part = Database_SQLInjection::no ($part);
    $data = Database_SQLInjection::no ($data);
    $query = "INSERT INTO repositories VALUES ('$bible', $part, '$data');";
    $database_instance->runQuery ($query);
  }


  public function getParts ($bible)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    $query = "SELECT part FROM repositories WHERE bible = '$bible' ORDER BY part ASC;";
    $parts = array ();
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $parts[] = $row[0];
    }
    return $parts;
  }


  public function getRepository ($bible, $part)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    $part = Database_SQLInjection::no ($part);
    $query = "SELECT data FROM repositories WHERE bible = '$bible' AND part = $part;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows == 0) {
      return "";
    }
    $row = $result->fetch_row ();
    return $row[0];
  }


  public function deleteRepository ($bible)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    $query = "DELETE FROM repositories WHERE bible = '$bible';";
    $database_instance->runQuery ($query);
    $this->optimize();
  }
 

}



?>
