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


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE logs;");
  }


  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    $timestamp = strtotime ("-1 month");
    $query = "DELETE FROM logs WHERE timestamp < $timestamp;";
    $database_instance->runQuery ($query);
  }   


  /**
  * log - Logs entry
  */
  public function log ($description) 
  {
    $database_instance = Database_Instance::getInstance();
    $description = Database_SQLInjection::no ($description);
    // The level of this entry.
    $session_logic = Session_Logic::getInstance ();
    $level = $session_logic->currentLevel ();
    // System-generated entries have level 1: Fix that.    
    include ("session/levels.php");
    if ($level = GUEST_LEVEL) $level = MANAGER_LEVEL;
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
    $level = $session_logic->currentLevel ();
    $server  = Database_Instance::getInstance ();
    $today = strtotime ("today");
    $start = (int) $start;
    $end = (int) $end;
    $query = "SELECT id, timestamp, event FROM logs WHERE timestamp >= $start AND timestamp < $end AND level <= $level ORDER BY id;";
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


}



?>
