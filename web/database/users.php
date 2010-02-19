<?php




class Database_Users
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Users();
    }
    return self::$instance;
  }


  /**
  * getAdministratorCount - Returns how many administrators there are
  */
  public function getAdministratorCount() {
    include ("session/levels.php");
    $server = Database_Instance::getInstance ();
    $query = "SELECT * FROM users WHERE level = " . ADMIN_LEVEL;
    $result = $server->mysqli->query ($query);
    return $result->num_rows; 
  }


  /**
  * matchUsernamePassword - Returns true if the username and password match
  */
  public function matchUsernamePassword($username, $password) {
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
  public function matchEmailPassword($email, $password) {
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
  public function addNewUser($username, $password, $level, $email) {
    $username = Database_SQLInjection::no ($username);
    $email = Database_SQLInjection::no ($email);
    $time = time();
    $query = "INSERT INTO users VALUES ('$username', '', '', $level, '$email', $time)";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($query);
    $this->updateUserPassword ($username, $password);
  }


  /**
  * addNewUserQuery - Returns the query to execute to add a new user.
  * Quotes are repeated since this query is stored in the database for a while
  */
  public function addNewUserQuery($username, $password, $level, $email) {
    $username = Database_SQLInjection::no ($username);
    $password = md5 ($password);
    $email = Database_SQLInjection::no ($email);
    $time = time();
    $query = "INSERT INTO users VALUES (''$username'', ''$password'', '''', $level, ''$email'', $time)";
    return $query;
  }


  /**
  * getEmailToUser - Returns the username that belongs to the $email.
  */
  public function getEmailToUser ($email) {
    $email = Database_SQLInjection::no ($email);
    $query = "SELECT username FROM users WHERE email = '$email';";
    $server = Database_Instance::getInstance ();
    $result = $server->mysqli->query ($query);
    if ($result->num_rows == 0)
      return "";
    $result_array = $result->fetch_array();
    return $result_array['username'];
  }   


  /**
  * getUserToEmail - Returns the email address that belongs to $user.
  */
  public function getUserToEmail ($user) {
    $user = Database_SQLInjection::no ($user);
    $query = "SELECT email FROM users WHERE username = '$user';";
    $server = Database_Instance::getInstance ();
    $result = $server->mysqli->query ($query);
    if ($result->num_rows == 0)
      return "";
    $result_array = $result->fetch_array();
    return $result_array['email'];
  }   


  /**
  * usernameExists - Returns true if the username exists in the database
  */
  public function usernameExists($user) {
    $server = Database_Instance::getInstance ();
    $user = Database_SQLInjection::no ($user);
    $query = "SELECT * FROM users WHERE username = '$user'";
    $result = $server->mysqli->query ($query);
    return ($result->num_rows > 0);
  }


  /**
  * emailExists - Returns true if the email address exists in the database
  */
  public function emailExists($email) {
    $server = Database_Instance::getInstance ();
    $email  = Database_SQLInjection::no ($email);
    $query  = "SELECT * FROM users WHERE email = '$email'";
    $result = $server->mysqli->query ($query);
    return ($result->num_rows > 0);
  }


  /**
  * getUserLevel - Returns the level that belongs to the user.
  */
  public function getUserLevel ($user) {
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
  public function updateUserLevel ($user, $level) {
    $user = Database_SQLInjection::no ($user);
    $level = Database_SQLInjection::no ($level);
    $query = "UPDATE users SET level = $level WHERE username = '$user';";
    return $query;
  }   


  /**
  * removeUser - Remove a user from the database.
  */
  public function removeUser($user) {
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
CREATE TABLE IF NOT EXISTS users (
username varchar(30) primary key,
password varchar(32),
id varchar(32),
level tinyint(1) unsigned not null,
email varchar(256),
timestamp int(11) unsigned not null
);
EOD;
    $database_instance->mysqli->query ($str);
    $database_instance->mysqli->query ("OPTIMIZE TABLE users;");
  }


  /**
  * getAdministrator - Returns the site administrator's username.
  * This function needs improvement since it only returns one admin, even if there are more.
  */
  public function getAdministrator () {
    $server = Database_Instance::getInstance ();
    include ("session/levels.php");
    $query = "SELECT username FROM users WHERE level = " . ADMIN_LEVEL . ";";
    $result = $server->mysqli->query ($query);
    if ($result->num_rows == 0)
      return "";
    $result_array = $result->fetch_array();
    return $result_array['username'];
  }   


  /**
  * updateUserPassword - Updates the $password for $user.
  */
  public function updateUserPassword ($user, $password) {
    $user = Database_SQLInjection::no ($user);
    $password = md5 ($password);
    $query = "UPDATE users SET password = '$password' WHERE username = '$user';";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($query);
  }   


  /**
  * updateEmailQuery - Returns the query to update a user's email address.
  * Single quotes are doubled, since this query will be stored in a database.
  */
  public function updateEmailQuery ($username, $email) {
    $query = "UPDATE users SET email = ''$email'' WHERE username = ''$username'';";
    return $query;
  }   


}



?>
