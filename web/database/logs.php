<?php




class Database_Logs
{
  /**
  * Singleton logic.
  */
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Logs();
    }
    return self::$instance;
  }


  /**
  * verify - Verifies the database table
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS logs (
id int auto_increment primary key,
timestamp timestamp,
description text
);
EOD;
    $database_instance->mysqli->query ($str);
    $database_instance->mysqli->query ("OPTIMIZE TABLE logs;");
  }


  /**
  * log - Logs entry
  */
  public function log ($description) {
    $database_instance = Database_Instance::getInstance(true);
    $description = Database_SQLInjection::no ($description);
    $query = "INSERT INTO logs VALUES (NULL, NULL, '$description');";
    $database_instance->mysqli->query ($query);
  }

}



?>
