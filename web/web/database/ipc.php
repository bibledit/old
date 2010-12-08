<?php


class Database_Ipc
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Ipc();
    }
    return self::$instance;
  }


  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS ipc (
id int auto_increment primary key,
channel varchar(256),
command varchar(30),
message text
);
EOD;
    $database_instance->runQuery ($str);
    // Subsequent table updates.
    $database_instance->runQuery ("ALTER TABLE ipc ADD user varchar(30) AFTER id;");
    $database_instance->runQuery ("UPDATE ipc SET user = '';");
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE ipc;");
  }


  public function storeMessage ($user, $channel, $command, $message)
  {
    $user    = Database_SQLInjection::no ($user);
    $channel = Database_SQLInjection::no ($channel);
    $command = Database_SQLInjection::no ($command);
    $message = Database_SQLInjection::no ($message);
    $database_instance = Database_Instance::getInstance();
    if ($channel == "") {
      $query = "DELETE FROM ipc WHERE user = '$user' AND channel = '' AND command = '$command';";
      $database_instance->runQuery ($query);
    }
    $query = "INSERT INTO ipc VALUES (NULL, '$user', '$channel', '$command', '$message');";
    $database_instance->runQuery ($query);
  }


  /**
  * Retrieves a message if there is any.
  * Returns NULL if there was nothing, 
  * else an associated array with keys and values for "id", "channel", "command" and "message".
  */
  public function retrieveMessage ($id, $user, $channel)
  {
    $id = Database_SQLInjection::no ($id);
    $channel = Database_SQLInjection::no ($channel);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT id, channel, command, message FROM ipc WHERE id > $id AND (channel = '$channel' OR channel = '') AND (user = '$user' OR user = '') ORDER BY id DESC;";
    $result = $database_instance->runQuery ($query);
    $values = NULL;
    if ($result->num_rows > 0) {
      $values = $result->fetch_assoc();
    }
    return $values;
  }
  
  public function deleteMessage ($id)
  {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM ipc WHERE id = $id;";
    $database_instance->runQuery ($query);
  }

  public function getFocus ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();
    $user = Database_SQLInjection::no ($user);
    $message = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT message FROM ipc WHERE command = 'focus' AND (user = '' OR user = '$user') ORDER BY id DESC;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $message = $result->fetch_row();
    }
    return $message[0];
  }
  


}



?>
