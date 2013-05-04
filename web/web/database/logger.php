<?php


class Database_Logger
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
      self::$instance = new Database_Logger ();
    }
    return self::$instance;
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE logger;");
  }


  public function record ($pid, $filename)
  {
    $server = Database_Instance::getInstance();
    $pid = Database_SQLInjection::no ($pid);
    $filename = Database_SQLInjection::no ($filename);
    $query = "INSERT INTO logger VALUES ($pid, '$filename');";
    $server->runQuery ($query);
  }


  public function get ()
  {
    $data = array ();
    $server  = Database_Instance::getInstance ();
    $query = "SELECT pid, logfile FROM logger;";
    $entries = $server->runQuery ($query);
    while ($row = $entries->fetch_assoc()) {
      $pid = $row["pid"];
      $filename = $row["logfile"];
      $data [$pid] = $filename;
    }
    return $data;
  }


  public function erase ($pid)
  {
    $server  = Database_Instance::getInstance ();
    $pid = Database_SQLInjection::no ($pid);
    $query   = "DELETE FROM logger WHERE pid = $pid;";
    $server->runQuery ($query);
  }


}


?>
