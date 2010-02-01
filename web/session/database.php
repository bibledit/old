<?php




require_once ("bootstrap/bootstrap.php");




class Session_Database
{



  private static $instance;    // Current object instance.




  // The class constructor.    
  private function __construct() {
  } 




  // Gets or creates the object instance, making it a singleton.
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Session_Database();
    }
    return self::$instance;
  }




}



?>
