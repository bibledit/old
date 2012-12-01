<?php


class Database_Repositories
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Repositories();
    }
    return self::$instance;
  }

  /**
  * Verify database table, optionally creating and/or optimizing it
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
    // Table no longer used.
    $database_instance->runQuery ("DROP TABLE repositories;");
  }


  public function optimize () {
  }


}



?>