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
timestamp int,
event text
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
    $time = time();
    $query = "INSERT INTO logs VALUES (NULL, $time, '$description');";
    $database_instance->mysqli->query ($query);
    $time -= 86400;
    $query = "DELETE FROM logs WHERE timestamp < $time;";
    echo $query;
    $database_instance->mysqli->query ($query);
  }

  /**
  * get - get the logbook entries.
  */
  public function get () {
    $server  = Database_Instance::getInstance ();
    $query   = "SELECT timestamp, event FROM logs ORDER BY id DESC;";
    $result  = $server->mysqli->query ($query);
    return $result;
  }


}



?>
