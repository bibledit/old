<?php
/**
* Database_Mail
* Handles mail sent from the translation environment to the users.
* This is a translation environment, not a message board.
* If users wish to communicate with one another about the translation, they can do so through the issues tracker.
* The application does not provide for private messages between users. They can use their email for that.
*/


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
  * Optimizes the database table
  */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE mail;");
  }


  /**
  * Mail labels
  */
  public function labelInbox () {
    return "inbox";
  }
  public function labelTrash () {
    return "trash";
  }
  
  

  /**
  * send - Send mail
  */
  public function send ($to, $subject, $body) {
    $to      = Database_SQLInjection::no ($to);
    $subject = Database_SQLInjection::no ($subject);
    $body    = Database_SQLInjection::no ($body);
    $server  = Database_Instance::getInstance ();
    $label   = $this->labelInbox ();
    $time    = time();
    $query   = "INSERT INTO mail VALUES (NULL, '$to',   $time, '$label', '$subject', '$body');";
    $result  = $server->runQuery ($query);
  }


  /**
  * getMailCount - get number of mails in the inbox of the current user
  */
  public function getMailCount () {
    $session = Session_Logic::getInstance ();
    $user = $session->currentUser();
    $user = Database_SQLInjection::no ($user);
    $label = $this->labelInbox ();
    $query = "SELECT id FROM mail WHERE username = '$user' and label = '$label';";
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ($query);
    return $result->num_rows;
  }


  /**
  * getMails - get the mails of the current user as specified through the parameters.
  */
  public function getMails ($label) {
    $label = Database_SQLInjection::no ($label);
    $session = Session_Logic::getInstance ();
    $user = $session->currentUser();
    $server = Database_Instance::getInstance ();
    $query = "SELECT id, timestamp, subject FROM mail WHERE username = '$user' and label = '$label' ORDER BY timestamp DESC;";
    $result = $server->runQuery ($query);
    return $result;
  }


  /**
  * delete - delete a mail.
  */
  public function delete ($id) {
    $id = Database_SQLInjection::no ($id);
    $server = Database_Instance::getInstance ();
    $query = "SELECT label FROM mail WHERE id = $id;";
    $result = $server->runQuery ($query);
    $row = $result->fetch_assoc();
    $label = $row['label'];
    if ($label == $this->labelTrash ()) {
      // If the mail was in the Trash, delete it completely.
      $query = "DELETE FROM mail WHERE id = $id;";
    } else {
      // Move the mail into the Trash.
      $label = $this->labelTrash ();
      $query = "UPDATE mail SET label = '$label' WHERE id = $id;";
    }
    $server->runQuery ($query);
  }


  /**
  * get - get a mail.
  */
  public function get ($id) {
    $id = Database_SQLInjection::no ($id);
    $server = Database_Instance::getInstance ();
    $query = "SELECT username, subject, body FROM mail WHERE id = $id;";
    $result = $server->runQuery ($query);
    return $result;
  }


  /**
  * getMailsInboxes - get ids of all mails in all inboxes.
  */
  public function getMailsInboxes () {
    $ids = array ();
    $server = Database_Instance::getInstance ();
    $label = $this->labelInbox();
    $query = "SELECT id FROM mail WHERE label = '$label';";
    $result = $server->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $result_array = $result->fetch_row();
      $ids[] = $result_array [0];
    }
    return $ids;
  }


  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    $time = time () - 2592000; // Remove entries after 30 days.
    $query = "DELETE FROM mail WHERE timestamp < $time;";
    $database_instance->runQuery ($query);
  }   


}


?>
