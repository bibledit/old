<?php


class Database_Config_User
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Config_User();
    }
    return self::$instance;
  }

  /**
  * Verify database table, optionally creating and/or optimizing it
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS config_user (
id int auto_increment primary key,
username varchar(30),
bible varchar(256),
ident varchar(100) not null,
value varchar(1000),
offset int not null
);
EOD;
    $database_instance->runQuery ($str);
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE config_user;");
  }


  private function getValue ($bible, $key, $default) {
    $session_logic = Session_Logic::getInstance();
    $username = $session_logic->currentUser();
    $username = Database_SQLInjection::no ($username);
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_user WHERE username = '$username' AND bible = '$bible' AND ident = '$key';";
    $result = $database->runQuery ($query);
    if ($result->num_rows == 0) {
      return $default;
    }
    $result_array = $result->fetch_row();
    return $result_array [0];
  }
  private function setValue ($bible, $key, $value) {
    $session_logic = Session_Logic::getInstance();
    $username = $session_logic->currentUser();
    $username = Database_SQLInjection::no ($username);
    $database = Database_Instance::getInstance ();
    $value = Database_SQLInjection::no ($value);
    $query = "DELETE FROM config_user WHERE username = '$username' AND bible = '$bible' AND ident = '$key';";
    $database->runQuery ($query);
    $query = "INSERT INTO config_user VALUES (NULL, '$username', '$bible', '$key', '$value', 0);";    
    $database->runQuery ($query);
  }


  public function getStylesheet() {
    return $this->getValue ("", "stylesheet", "Standard");
  }
  public function setStylesheet ($value) {
    $this->setValue ("", "stylesheet", $value);
  }   



}



?>
