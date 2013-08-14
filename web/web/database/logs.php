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
  public function log ($description, $level = 5) 
  {
    $database_instance = Database_Instance::getInstance();
    $description = Database_SQLInjection::no ($description);
    $level = Database_SQLInjection::no ($level);
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


  // Gets the logbook entry with id higher than $id.
  public function getNext ($id) {
    $id = (int) $id;
    $session_logic = Session_Logic::getInstance ();
    $level = $session_logic->currentLevel ();
    $server  = Database_Instance::getInstance ();
    $query = "SELECT id, timestamp, event FROM logs WHERE id > $id AND level <= $level ORDER BY id ASC LIMIT 1;";
    $result = $server->runQuery ($query);
    return $result;
  }


}



?>
