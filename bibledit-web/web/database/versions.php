<?php

class Database_Versions
{
  private static $instance;

  // The class constructor is private, so no outsider can call it.
  private function __construct() {
  } 

  // Gets or creates the object instance, making it a singleton.
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Versions();
    }
    return self::$instance;
  }

  /**
  * verify - Verifies the database table, possibly creating it.
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS versions (
name varchar(50) primary key,
version int unsigned
);
EOD;
    $database_instance->runQuery ($str);
  }

  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE versions;");
  }
  
  public function getVersion ($database) {
    $server = Database_Instance::getInstance ();
    $query = "SELECT version FROM versions WHERE name = '$database';";
    $result = $server->runQuery ($query);
    if ($result->num_rows == 0) {
      return 0;
    }
    $result_array = $result->fetch_array();
    return $result_array['version'];
  }
  
  public function setVersion ($database, $version) {
    $server = Database_Instance::getInstance ();
    $query = "REPLACE INTO versions VALUES ('$database', $version);";
    $server->runQuery ($query);
  }
  
}

?>
