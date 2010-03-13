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
    $database_instance->runQuery ($str);
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE logs;");
  }


  /**
  * log - Logs entry
  */
  public function log ($description) {
    $database_instance = Database_Instance::getInstance();
    $description = Database_SQLInjection::no ($description);
    $time = time();
    $query = "INSERT INTO logs VALUES (NULL, $time, '$description');";
    $database_instance->runQuery ($query);
    $time -= 86400;
    $query = "DELETE FROM logs WHERE timestamp < $time;";
    $database_instance->runQuery ($query);
  }

  /**
  * get - get the logbook entries.
  */
  public function get () {
    $server  = Database_Instance::getInstance ();
    $query   = "SELECT timestamp, event FROM logs ORDER BY id DESC;";
    $result  = $server->runQuery ($query);
    return $result;
  }


  /**
  * clear - clears the logbook entries.
  */
  public function clear () {
    $server  = Database_Instance::getInstance ();
    $query   = "DELETE FROM logs;";
    $server->runQuery ($query);
  }


}



?>
