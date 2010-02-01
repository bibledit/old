<?php


// Page bootstrapping.
require_once ("bootstrap/bootstrap.php");


class Database_Users
{

  public $num_active_users;   //Number of active users viewing site
  public $num_active_guests;  //Number of active guests viewing site
  public $num_members;        //Number of signed-up users
  /* Note: call getNumMembers() to access $num_members! */
  private static $instance;


  private function __construct()
  { 

    $server = Database_Instance::getInstance ();

    // Create the table structures if these do not yet exist.
    
$str = <<<EOD
CREATE TABLE IF NOT EXISTS users (
username varchar(30) primary key,
password varchar(32),
userid varchar(32),
userlevel tinyint(1) unsigned not null,
email varchar(50),
timestamp int(11) unsigned not null
);
EOD;
    $server->mysqli->query ($str);

$str = <<<EOD
CREATE TABLE IF NOT EXISTS active_users (
username varchar(30) primary key,
timestamp int(11) unsigned not null
);
EOD;
    $server->mysqli->query ($str);

$str = <<<EOD
CREATE TABLE IF NOT EXISTS active_guests (
ip varchar(15) primary key,
timestamp int(11) unsigned not null
);
EOD;
    $server->mysqli->query ($str);

$str = <<<EOD
CREATE TABLE IF NOT EXISTS banned_users (
username varchar(30) primary key,
timestamp int(11) unsigned not null
);
EOD;
    $server->mysqli->query ($str);
  }




  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Users();
    }
    return self::$instance;
  }




  /**
  * confirmUserPass - Checks whether or not the given
  * username is in the database, if so it checks if the
  * given password is the same password in the database
  * for that user. If the user doesn't exist or if the
  * passwords don't match up, it returns an error code
  * (1 or 2). On success it returns 0.
  */
  function confirmUserPass($username, $password){
   /* Add slashes if necessary (for query) */
    if(!get_magic_quotes_gpc()) {
      $username = addslashes($username);
    }
 
    $server = Database_Instance::getInstance ();

    /* Verify that user is in database */
    $q = "SELECT password FROM users WHERE username = '$username';";
    $result = $server->mysqli->query ($q);
    if(!$result || ($result->num_rows < 1)){
       return 1; //Indicates username failure
    }

    /* Retrieve password from result, strip slashes */
    $dbarray = $result->fetch_array();
    $dbarray['password'] = stripslashes($dbarray['password']);
    $password = stripslashes($password);

    /* Validate that password is correct */
    if($password == $dbarray['password']){
      return 0; //Success! Username and password confirmed
    }
    else{
      return 2; //Indicates password failure
    }
  }
     


  /**
  * confirmUserID - Checks whether or not the given
  * username is in the database, if so it checks if the
  * given userid is the same userid in the database
  * for that user. If the user doesn't exist or if the
  * userids don't match up, it returns an error code
  * (1 or 2). On success it returns 0.
  */
  public function confirmUserID($username, $userid){
    /* Add slashes if necessary (for query) */
    if(!get_magic_quotes_gpc()) {
     $username = addslashes($username);
    }

    $server = Database_Instance::getInstance ();

    /* Verify that user is in database */
    $q = "SELECT userid FROM users WHERE username = '$username'";
    $result = $server->mysqli->query ($q);
    if(!$result || ($result->num_rows < 1)){
       return 1; //Indicates username failure
    }

    /* Retrieve userid from result, strip slashes */
    $dbarray = $result->fetch_array();
    $dbarray['userid'] = stripslashes($dbarray['userid']);
    $userid = stripslashes($userid);

    /* Validate that userid is correct */
    if($userid == $dbarray['userid']){
       return 0; //Success! Username and userid confirmed
    }
    else{
       return 2; //Indicates userid invalid
    }
  }



  /**
  * getUserInfo - Returns the result array from a mysql
  * query asking for all information stored regarding
  * the given username. If query fails, NULL is returned.
  */
  public function getUserInfo($username){
    $server = Database_Instance::getInstance ();
    $q = "SELECT * FROM users WHERE username = '$username'";
    $result = $server->mysqli->query ($q);
    /* Error occurred, return given name by default */
    if(!$result || ($result->num_rows < 1)){
      return NULL;
    }
    /* Return result array */
    $dbarray = $result->fetch_array();
    return $dbarray;
  }



  /**
  * updateUserField - Updates a field, specified by the field
  * parameter, in the user's row of the database.
  */
  function updateUserField($username, $field, $value){
    $server = Database_Instance::getInstance ();
    $q = "UPDATE users SET ".$field." = '$value' WHERE username = '$username'";
    return $server->mysqli->query ($q);
  }
   
   

  /**
  * addActiveUser - Updates username's last active timestamp
  * in the database, and also adds him to the table of
  * active users, or updates timestamp if already there.
  */
  public function addActiveUser($username, $time){
    $q = "UPDATE users SET timestamp = '$time' WHERE username = '$username'";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($q);
    
    // Track visitors.
    $q = "REPLACE INTO active_users VALUES ('$username', '$time')";
    $server->mysqli->query ($q);
    $this->calcNumActiveUsers();
  }
   
 
  /**
  * calcNumActiveUsers - Finds out how many active users
  * are viewing site and sets class variable accordingly.
  */
  public function calcNumActiveUsers(){
    /* Calculate number of users at site */
    $q = "SELECT * FROM active_users";
    $server = Database_Instance::getInstance ();
    $result = $server->mysqli->query ($q);
    $this->num_active_users = $result->num_rows;
  }


  /* removeActiveGuest */
  public function removeActiveGuest($ip){
    $q = "DELETE FROM active_guests WHERE ip = '$ip'";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($q);
    $this->calcNumActiveGuests();
  }
   

  /**
  * calcNumActiveGuests - Finds out how many active guests
  * are viewing site and sets class variable accordingly.
  */
  public function calcNumActiveGuests(){
    /* Calculate number of guests at site */
    $q = "SELECT * FROM active_guests";
    $server = Database_Instance::getInstance ();
    $result = $server->mysqli->query ($q);
    $this->num_active_guests = $result->num_rows;
  }
   


  /**
  * addNewUser - Inserts the given (username, password, email)
  * info into the database. Appropriate user level is set.
  * Returns true on success, false otherwise.
  */
  public function addNewUser($username, $password, $email){
    $time = time();
    /* If admin sign up, give admin user level */
    require ("admin/credentials.php");
    require ("session/levels.php");
    if (($username == $site_admin_username) && ($password == md5($site_admin_password))) {
      $ulevel = ADMIN_LEVEL;
    } else {
      $ulevel = PUBLIC_LEVEL;
    }
    $q = "INSERT INTO users VALUES ('$username', '$password', '0', $ulevel, '$email', $time)";
    $server = Database_Instance::getInstance ();
    return $server->mysqli->query ($q);
  }
   


  /* removeActiveUser */
  function removeActiveUser($username){
    $q = "DELETE FROM active_users WHERE username = '$username'";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($q);
    $this->calcNumActiveUsers();
  }
   


  /* addActiveGuest - Adds guest to active guests table */
  function addActiveGuest($ip, $time){
    $q = "REPLACE INTO active_guest VALUES ('$ip', '$time')";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($q);
    $this->calcNumActiveGuests();
  }




  /* removeInactiveUsers */
  public function removeInactiveUsers(){
    $timeout = time()-USER_TIMEOUT*60;
    $q = "DELETE FROM active_users WHERE timestamp < $timeout";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($q);
    $this->calcNumActiveUsers();
  }



  /* removeInactiveGuests */
  public function removeInactiveGuests(){
    $timeout = time()-GUEST_TIMEOUT*60;
    $q = "DELETE FROM active_guest WHERE timestamp < $timeout";
    $server = Database_Instance::getInstance ();
    $server->mysqli->query ($q);
    $this->calcNumActiveGuests();
  }
   

  





   
 


}

?>
