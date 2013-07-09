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
  * Optimizes the table.
  */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE logs;");
  }


  /**
  * log - Logs entry
  * $manager - whether this logbook entry should be visible to the Manager as well.
  *            This has been disabled.
  */
  public function log ($description, $manager = false) 
  {
    $database_instance = Database_Instance::getInstance();
    $description = Database_SQLInjection::no ($description);
    include ("session/levels.php");
    if ($manager) $level = MANAGER_LEVEL;
    else $level = ADMIN_LEVEL;
    $time = time();
    $query = "INSERT INTO logs VALUES (NULL, $time, $level, '$description');";
    $database_instance->runQuery ($query);
  }

  /**
  * get - get the logbook entries.
  * $start and $end are the timestamps that limit the returned result.
  */
  public function get ($start, $end) {
    $session_logic = Session_Logic::getInstance ();
    $server  = Database_Instance::getInstance ();
    $today = strtotime ("today");
    $start = (int) $start;
    $end = (int) $end;
    $query = "SELECT id, timestamp, event FROM logs WHERE timestamp >= $start AND timestamp < $end ORDER BY id;";
    $result = $server->runQuery ($query);
    return $result;
  }


  /**
  * get - get the logbook entry with $id
  */
  public function getID ($id) {
    $id = (int) $id;
    $session_logic = Session_Logic::getInstance ();
    $server  = Database_Instance::getInstance ();
    $query = "SELECT timestamp, event FROM logs WHERE id = $id;";
    $result = $server->runQuery ($query);
    return $result;
  }


  /**
  * clear - clears the logbook entries.
  */
  public function clear () {
    $session_logic = Session_Logic::getInstance ();
    $level = $session_logic->currentLevel ();
    $server  = Database_Instance::getInstance ();
    $query   = "DELETE FROM logs WHERE level <= $level;";
    $server->runQuery ($query);
  }


  /**
  * Removes older entries.
  */
  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    $time = time () - 604800; // Remove entries after a week.
    $query = "DELETE FROM logs WHERE timestamp < $time;";
    $database_instance->runQuery ($query);
  }   


}



?>
