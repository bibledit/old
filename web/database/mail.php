<?php



class Database_Mail
{



  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Mail();
    }
    return self::$instance;
  }


  /**
  * getAdministratorCount - Returns how many administrators there are
  */
  public function getAdministratorCount() { // Todo goes out.
    include ("session/levels.php");
    $server = Database_Instance::getInstance ();
    $query = "SELECT * FROM users WHERE username = " . ADMIN_LEVEL;
    $result = $server->mysqli->query ($query);
    return $result->num_rows; 
  }


  /**
  * matchUsernamePassword - Returns true if the username and password match
  */
  public function matchUsernamePassword($username, $password) { // Todo goes out.
    $server = Database_Instance::getInstance ();
    $username = Database_SQLInjection::no ($username);
    $password = md5 ($password);
    $query = "SELECT * FROM users WHERE username = '$username' and password = '$password'";
    $result = $server->mysqli->query ($query);
    return $result->num_rows; 
  }


  /**
  * matchEmailPassword - Returns true if the email and password match
  */
  public function matchEmailPassword($email, $password) { // Todo goes out.
    $server = Database_Instance::getInstance ();
    $email = Database_SQLInjection::no ($email);
    $password = md5 ($password);
    $query = "SELECT * FROM users WHERE email = '$email' and password = '$password'";
    $result = $server->mysqli->query ($query);
    return $result->num_rows; 
  }


  /**
  * addNewUser - Inserts the given (username, password, level) into the database.
  */
  public function addNewUser($username, $password, $level, $email) { // Todo goes out.
    $username = Database_SQLInjection::no ($username);
    $password = md5 ($password);
    $email = Database_SQLInjection::no ($email);
    $time = time();
    $query = "INSERT INTO users VALUES ('$username', '$password', '', $level, '$email', $time)";
    $server = Database_Instance::getInstance ();
    return $server->mysqli->query ($query);
  }


  /**
  * getEmailToUser - Returns the username that belongs to the $email.
  */
  public function getEmailToUser ($email) { // Todo goes out.
    $email = Database_SQLInjection::no ($email);
    $query = "SELECT username FROM users WHERE email = '$email';";
    if ($result->num_rows == 0)
      return "";
    $result_array = $result->fetch_array();
    return $result_array['username'];
  }   


  /**
  * usernameExists - Returns true if the username exists in the database
  */
  public function usernameExists($user) { // Todo goes out.
    $server = Database_Instance::getInstance ();
    $user = Database_SQLInjection::no ($user);
    $query = "SELECT * FROM users WHERE username = '$user'";
    $result = $server->mysqli->query ($query);
    return ($result->num_rows > 0);
  }


  /**
  * getUserLevel - Returns the level that belongs to the user.
  */
  public function getUserLevel ($user) { // Todo goes out.
    $user = Database_SQLInjection::no ($user);
    $server = Database_Instance::getInstance ();
    $query = "SELECT level FROM users WHERE username = '$user';";
    $result = $server->mysqli->query ($query);
    if ($result->num_rows == 0) {
      include ("session/levels.php");
      return GUEST_LEVEL;
    }
    $result_array = $result->fetch_array();
    return $result_array['level'];
  }   


  /**
  * updateUserLevel - Returns the MySQL query that would update the level of a given user.
  */
  public function updateUserLevel ($user, $level) { // Todo goes out.
    $user = Database_SQLInjection::no ($user);
    $level = Database_SQLInjection::no ($level);
    $query = "UPDATE users SET level = $level WHERE username = '$user';";
    return $query;
  }   


  /**
  * removeUser - Remove a user from the database.
  */
  public function removeUser($user) { // Todo goes out.
    $server = Database_Instance::getInstance ();
    $user = Database_SQLInjection::no ($user);
    $query = "DELETE FROM users WHERE username = '$user'";
    $result = $server->mysqli->query ($query);
  }


  /**
  * verify - Verifies the database table
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance(true);
$str = <<<EOD
CREATE TABLE IF NOT EXISTS mail (
id int auto_increment primary key,
username varchar(30),
timestamp timestamp,
label varchar(30),
source varchar(256),
destination varchar(256),
subject varchar(256),
body text
);
EOD;
    $database_instance->mysqli->query ($str);
    $database_instance->mysqli->query ("OPTIMIZE TABLE mail;");
  }


}



?>
