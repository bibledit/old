<?php
/**
* Database Dialog
* Handles dialog arguments.
*/


class Database_Dialog // Be aware that this one is not yet being used.
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Dialog();
    }
    return self::$instance;
  }


  /**
  * Optimizes the database table
  */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    // Not used yet.$database_instance->runQuery ("OPTIMIZE TABLE dialog;");
  }


  /**
  * getNewID - returns a new unique confirmation ID as an integer
  */
  public function getNewID ()
  {
    $server = Database_Instance::getInstance ();
    do {
      $id = rand (1000, 9999);
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
    $query = "SELECT id FROM dialog WHERE id = $id;";
    $result = $server->runQuery ($query);
    return ($result->num_rows > 0);
  }
  

  /**
  * store - stores a confirmation cycle
  */
  public function store ($object, $method, $argument)
  {
    $object    = Database_SQLInjection::no ($object);
    $method    = Database_SQLInjection::no ($method);
    $argument  = Database_SQLInjection::no ($argument);
    $id        = $this->getNewID();
    $server    = Database_Instance::getInstance ();
    $timestamp = time ();
    $query     = "INSERT INTO dialog VALUES ($id, $timestamp, '$object', '$method', '$argument');";
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


}



?>