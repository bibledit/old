<?php




class Database_Config_General
{


  // Singleton object instance logic.
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Config_General();
    }
    return self::$instance;
  }

  /**
  * Verify database table, optionally creating and/or optimizing it
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS config_general (
ident VARCHAR(100) NOT NULL,
value VARCHAR(1000),
offset INT NOT NULL
);
EOD;
    $database_instance->runQuery ($str);
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE config_general;");
  }

  // Functions that retrieve the value or list from the database.
  private function getValue ($key, $default) {
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_general WHERE ident = '$key'";
    $result = $database->runQuery ($query);
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
    $database->runQuery ($query);
    $query = "INSERT INTO config_general VALUES ('$key', '$value', 0);";    
    $database->runQuery ($query);
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

  public function getMailStorageSecurity() {
    return $this->getValue ("mail-storage-security", "");
  }
  public function setMailStorageSecurity ($value) {
    $this->setValue ("mail-storage-security", $value);
  }   

  public function getMailStoragePort() {
    return $this->getValue ("mail-storage-port", "");
  }
  public function setMailStoragePort ($value) {
    $this->setValue ("mail-storage-port", $value);
  }   

  public function getMailSendHost() {
    return $this->getValue ("mail-send-host", "");
  }
  public function setMailSendHost ($value) {
    $this->setValue ("mail-send-host", $value);
  }   

  public function getMailSendAuthentication() {
    return $this->getValue ("mail-send-authentication", "");
  }
  public function setMailSendAuthentication ($value) {
    $this->setValue ("mail-send-authentication", $value);
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

  public function getMailSendSecurity() {
    return $this->getValue ("mail-send-security", "");
  }
  public function setMailSendSecurity ($value) {
    $this->setValue ("mail-send-security", $value);
  }   

  public function getMailSendPort() {
    return $this->getValue ("mail-send-port", "");
  }
  public function setMailSendPort ($value) {
    $this->setValue ("mail-send-port", $value);
  }   

  public function getTimerMinute() {
    return $this->getValue ("timer-minute", "");
  }
  public function setTimerMinute ($value) {
    $this->setValue ("timer-minute", $value);
  }   

  public function getTimerFiveMinutes() {
    return $this->getValue ("timer-five-minutes", "");
  }
  public function setTimerFiveMinutes ($value) {
    $this->setValue ("timer-five-minutes", $value);
  }   

  public function getTimerDay() {
    return $this->getValue ("timer-day", "");
  }
  public function setTimerDay ($value) {
    $this->setValue ("timer-day", $value);
  }   


}



?>
