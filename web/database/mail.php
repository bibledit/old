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
  * verify - Verifies the database table
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance(true);
$str = <<<EOD
CREATE TABLE IF NOT EXISTS mail (
id int auto_increment primary key,
username varchar(30),
timestamp int,
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


  /**
  * Mail labels
  */
  public function labelInbox () {
    return "inbox";
  }
  public function labelEmailed () {
    return "emailed";
  }
  public function labelSent () {
    return "sent";
  }
  public function labelTrash () {
    return "trash";
  }
  
  

  /**
  * send - Send mail
  */
  public function send ($to, $subject, $body) {
    $session = Session_Logic::getInstance ();
    $from    = $session->currentUser ();
    if ($from == "") {
      $from = "system";
    }
    $from    = Database_SQLInjection::no ($from);
    $to      = Database_SQLInjection::no ($to);
    $subject = Database_SQLInjection::no ($subject);
    $body    = Database_SQLInjection::no ($body);
    $server  = Database_Instance::getInstance ();
    $label   = $this->labelInbox ();
    $time    = time();
    $query   = "INSERT INTO mail VALUES (NULL, '$to',   $time, '$label', '$from', '$to', '$subject', '$body');";
    $result  = $server->mysqli->query ($query);
    $label   = $this->labelSent ();
    $query   = "INSERT INTO mail VALUES (NULL, '$from', $time, '$label', '$from', '$to', '$subject', '$body');";
    $result  = $server->mysqli->query ($query);
  }


  /**
  * getMailCount - get number of mails in the inbox of the current user
  */
  public function getMailCount () {
    $session = Session_Logic::getInstance ();
    $user    = $session->currentUser();
    $user    = Database_SQLInjection::no ($user);
    $label   = $this->labelInbox ();
    $query   = "SELECT id FROM mail WHERE username = '$user' and label = '$label';";
    $server  = Database_Instance::getInstance ();
    $result  = $server->mysqli->query ($query);
    return $result->num_rows;
  }


  /**
  * getMails - get the mails of the current user as specified through the parameters.
  */
  public function getMails ($label) {
    $label = Database_SQLInjection::no ($label);
    $session = Session_Logic::getInstance ();
    $user    = $session->currentUser();
    $server  = Database_Instance::getInstance ();
    $query   = "SELECT id, timestamp, source, destination, subject FROM mail WHERE username = '$user' and label = '$label' ORDER BY timestamp DESC;";
    $result  = $server->mysqli->query ($query);
    return $result;
  }


  /**
  * delete - delete a mail.
  */
  public function delete ($id) {
    $id      = Database_SQLInjection::no ($id);
    $server  = Database_Instance::getInstance ();
    $query   = "SELECT label FROM mail WHERE id = $id;";
    $result  = $server->mysqli->query ($query);
    $row     = $result->fetch_assoc();
    $label   = $row['label'];
    // Normally move the mail into the Trash, but if it is already in the Trash, delete it completely.
    if ($label == $this->labelTrash ()) {
      $query  = "DELETE FROM mail WHERE id = $id;";
    } else {
      $label = $this->labelTrash ();
      $query  = "UPDATE mail SET label = '$label' WHERE id = $id;";
    }
    $server->mysqli->query ($query);
    

  }



}



?>
