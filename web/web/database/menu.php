<?php


class Database_Menu
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Menu ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE menu;");
  }
  

  public function getAccessCount ($user, $url) {
    $user = Database_SQLInjection::no ($user);
    $url = Database_SQLInjection::no ($url);
    $query = "SELECT hits FROM menu WHERE username = '$user' AND url = '$url';";
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ($query);
    if ($result->num_rows == 0) return 0;
    $result_array = $result->fetch_array();
    $count = $result_array['hits'];
    $count = (int) $count;
    return $count;
  }   


  public function increaseAccessCount ($user, $url) {
    $count = $this->getAccessCount ($user, $url);
    $user = Database_SQLInjection::no ($user);
    $url = Database_SQLInjection::no ($url);
    if ($count == 0) {
      $query = "INSERT INTO menu VALUES (NULL, '$user', '$url', 1)";
    } else {
      $count++;
      $query = "UPDATE menu SET hits = $count WHERE username = '$user' AND url = '$url';";
    }
    $server = Database_Instance::getInstance ();
    $server->runQuery ($query);
  }   


  public function clearAllAccessCounts ($user) {
    $user = Database_SQLInjection::no ($user);
    $server = Database_Instance::getInstance ();
    $query = "DELETE FROM menu WHERE username = '$user'";
    $result = $server->runQuery ($query);
  }


}


?>
