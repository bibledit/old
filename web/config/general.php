<?php




class Config_General
{


  // Singleton object instance logic.
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Config_General();
    }
    return self::$instance;
  }


  // Functions that retrieve the value or list from the database.
  private function getValue ($key, $default) {
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_general WHERE ident = '$key'";
    $result = $database->mysqli->query ($query);
    if ($result->num_rows == 0) {
      return $default;
    }
    $result_array = $result->fetch_row();
    return $result_array [0];
  }
  private function setValue ($key, $value) {
    $database = Database_Instance::getInstance ();
    $value = Database_SQLInjection::no ($value);
    $query = "DELETE FROM config_general WHERE ident = '$key';";
    $database->mysqli->query ($query);
    $query = "INSERT INTO config_general VALUES ('$key', '$value', 0);";    
    $database->mysqli->query ($query);
  }
  private function getList ($key) {
  }


  public function getSiteMailName() {
    return $this->getValue ("site-mail-name", "Bible Translation");
  }
  public function setSiteMailName ($value) {
    $this->setValue ("site-mail-name", $value);
  }   

  public function getSiteMailAddress() {
    return $this->getValue ("site-mail-address", "");
  }
  public function setSiteMailAddress ($value) {
    $this->setValue ("site-mail-address", $value);
  }   

  public function getMailStorageHost() {
    return $this->getValue ("mail-storage-host", "");
  }
  public function setMailStorageHost ($value) {
    $this->setValue ("mail-storage-host", $value);
  }   

  public function getMailStorageUsername() {
    return $this->getValue ("mail-storage-username", "");
  }
  public function setMailStorageUsername ($value) {
    $this->setValue ("mail-storage-username", $value);
  }   

  public function getMailStoragePassword() {
    return $this->getValue ("mail-storage-password", "");
  }
  public function setMailStoragePassword ($value) {
    $this->setValue ("mail-storage-password", $value);
  }   

  public function getMailSendHost() {
    return $this->getValue ("mail-send-host", "");
  }
  public function setMailSendHost ($value) {
    $this->setValue ("mail-send-host", $value);
  }   

  public function getMailSendUsername() {
    return $this->getValue ("mail-send-username", "");
  }
  public function setMailSendUsername ($value) {
    $this->setValue ("mail-send-username", $value);
  }   

  public function getMailSendPassword() {
    return $this->getValue ("mail-send-password", "");
  }
  public function setMailSendPassword ($value) {
    $this->setValue ("mail-send-password", $value);
  }   



}



?>
