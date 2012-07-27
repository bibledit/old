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
  * $page is page number, where the last page would be 1, 
  * the one-but last would be 2, and so on.
  */
  public function get ($page) {
    $session_logic = Session_Logic::getInstance ();
    $level = $session_logic->currentLevel ();
    $server  = Database_Instance::getInstance ();
    $limits = $this->limits ($page);
    $query = "SELECT timestamp, event FROM logs WHERE level <= $level ORDER BY id $limits;";
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
  * Returns the LIMIT clause for the database query.
  * $page: the page in the database, starting at 1.
  * If the $page is 0, there will be no limit.
  */
  private function limits ($page)
  {
    $page = Database_SQLInjection::no ($page);
    $server  = Database_Instance::getInstance ();
    $query = "SELECT COUNT(*) FROM logs;";
    $result  = $server->runQuery ($query);
    $count = $result->fetch_row ();
    $count = $count[0];
    $amount = 60;
    $limit = $count - ($page * $amount);
    if ($limit < 0) {
      $amount = $amount + $limit;
      $limit = 0;
    }
    if ($amount < 0) $amount = 0;
    $query = "LIMIT $limit,$amount";
    if ($page == 0) $query = "";
    return $query;
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
