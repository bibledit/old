<?php
/**
* Database_Confirm
* Handles email and web page confirmations.
*/


class Database_Confirm
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Confirm();
    }
    return self::$instance;
  }


  /**
  * verify - Verifies the database table
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS confirm (
id int primary key,
query text,
timestamp int,
mailto varchar(256),
subject varchar(256),
body text
);
EOD;
    $database_instance->runQuery ($str);
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE confirm;");
  }


  /**
  * getNewID - returns a new unique confirmation ID as an integer
  */
  public function getNewID ()
  {
    $server = Database_Instance::getInstance ();
    do {
      $id = rand (100000000, 999999999);
    } while ($this->IDExists ($id));
    return $id;
  }


  /**
  * IDExists - returns true if the $id exists
  */
  public function IDExists ($id)
  {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT id FROM confirm WHERE id = $id;";
    $result = $server->runQuery ($query);
    return ($result->num_rows > 0);
  }
  

  /**
  * store - stores a confirmation cycle
  */
  public function store ($id, $query, $to, $subject, $body)
  {
    $server    = Database_Instance::getInstance ();
    $id        = Database_SQLInjection::no ($id);
    $query     = Database_SQLInjection::no ($query);
    $timestamp = time ();
    $to        = Database_SQLInjection::no ($to);
    $subject   = Database_SQLInjection::no ($subject);
    $body      = Database_SQLInjection::no ($body);
    $query     = "INSERT INTO confirm VALUES ($id, '$query', $timestamp, '$to', '$subject', '$body');";
    $server->runQuery ($query);
  }


  /**
  * searchID - search the database for the ID in $subject.
  * If it exists, it returns the ID number, else it returns 0.
  */
  public function searchID ($subject)
  {
    $server = Database_Instance::getInstance ();
    $query = "SELECT id FROM confirm;";
    $result = $server->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $result_array = $result->fetch_row();
      $id = $result_array [0];
      $pos = strpos ($subject, $id);
      if ($pos !== false) {
        return $id;
      }
    }
    return 0;
  }
  

  /**
  * getQuery - Returns the query for $id.
  */
  public function getQuery ($id) {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT query FROM confirm WHERE id = $id;";
    $result = $server->runQuery ($query);
    $result_array = $result->fetch_row();
    return $result_array[0];
  }   


  /**
  * getMailTo - Returns the To: address for $id.
  */
  public function getMailTo ($id) {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT mailto FROM confirm WHERE id = $id;";
    $result = $server->runQuery ($query);
    $result_array = $result->fetch_row();
    return $result_array[0];
  }   


  /**
  * getSubject - Returns the Subject: for $id.
  */
  public function getSubject ($id) {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT subject FROM confirm WHERE id = $id;";
    $result = $server->runQuery ($query);
    $result_array = $result->fetch_row();
    return $result_array[0];
  }   


  /**
  * getBody - Returns the email's body for $id.
  */
  public function getBody ($id) {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT body FROM confirm WHERE id = $id;";
    $result = $server->runQuery ($query);
    $result_array = $result->fetch_row();
    return $result_array[0];
  }   


  /**
  * delete - Deletes $id from the table.
  */
  public function delete ($id) {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "DELETE FROM confirm WHERE id = $id;";
    $server->runQuery ($query);
  }
  
  
  /**
  * Removes expired entries.
  */
  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    $time = time() - 2592000; // Leave entries for 30 days.
    $query = "DELETE FROM confirm WHERE timestamp < $time;";
    $database_instance->runQuery ($query);
  }   


}



?>
