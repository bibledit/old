<?php



require_once ("bootstrap/bootstrap.php");



class Session_Users
{

  public $usernames;
  public $ids;
  public $levels;
  public $emails;
  
  private static $instance;

  private function __construct() {
    // Get the level (= role) of the person that works on this page.
    // This is to ensure that no users of a higher level can be listed.
    // E.g. if the team manager goes to this page to manage users, the site admin will not be displayed.
    $session_logic = Session_Logic::getInstance ();
    $mylevel = $session_logic->currentLevel ();

    $server = Database_Instance::getInstance ();
    $query = "SELECT * FROM users ORDER BY level DESC, username ASC;";
    $result = $server->mysqli->query ($query);
    while ($row = $result->fetch_assoc()) {
      $level = $row["level"];
      if ($level <= $mylevel) {
        $this->usernames[] = $row["username"];
        $this->ids[]       = $row["id"];
        $this->levels[]    = $level;
        $this->emails[]    = $row["email"];
      }
    }
  } 

  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Session_Users();
    }
    return self::$instance;
  }
}



?>
