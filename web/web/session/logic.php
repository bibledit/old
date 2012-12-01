<?php




require_once ("bootstrap/bootstrap.php");




class Session_Logic
{

  private $level = 0;                         // The level of the user.
  private $check_browser = true;              // Include browser name in fingerprint?
  private $check_ip_blocks = 2;               // How many numbers from IP use in fingerprint?
  private $secure_word = 'controlword';       // Control word - any word you want.
  private $regenerate_id = true;              // Regenerate session ID to prevent fixation attacks?
  private static $instance;                   // Current singleton instance.
  private $logged_in;


  // The class constructor is private, so no outsider can call it.    
  private function __construct() {
    session_name ('bibledit');
    if (php_sapi_name () != "cli") {
      session_set_cookie_params (43200);
      session_start();
    }
    $this->Open ();
  } 


  // Gets or creates the singleton object instance.
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Session_Logic();
    }
    return self::$instance;
  }


  // Call this when init session.
  public function Open()
  {
    $_SESSION['ss_fprint'] = $this->_Fingerprint();
    $this->_RegenerateId();
    if ($this->openAccess ()) return;
    if (!$this->Check() || !isset($_SESSION['logged_in']) || !$_SESSION['logged_in']) {
      $this->logged_in = false;
    } else {
      $this->logged_in =  true;
    }
  }


  // Call this to check session.
  public function Check()
  {
    $this->_RegenerateId();
    return (isset($_SESSION['ss_fprint']) && $_SESSION['ss_fprint'] == $this->_Fingerprint());
  }


  // Returns MD5 from fingerprint.
  private function _Fingerprint()
  {
     $fingerprint = $this->secure_word;
     if ($this->check_browser) {
       @$fingerprint .= $_SERVER['HTTP_USER_AGENT'];
     }
     if ($this->check_ip_blocks) {
       $num_blocks = abs(intval($this->check_ip_blocks));
       if ($num_blocks > 4) {
         $num_blocks = 4;
       }
       @$blocks = explode('.', $_SERVER['REMOTE_ADDR']);
       for ($i = 0; $i < $num_blocks; $i++) {
         @$fingerprint .= $blocks[$i] . '.';
       }
     }
     return md5($fingerprint);
  }


  // Regenerates session ID if possible.
  private function _RegenerateId()
  {
    if ($this->regenerate_id && function_exists('session_regenerate_id')) {
      if (version_compare('5.1.0', phpversion(), '>=')) {
        @session_regenerate_id(true);
      } else {
        @session_regenerate_id();
      }
    }
  }

  /**
  * Attempts to log into the system
  * 
  */
  public function attemptLogin ($user_or_email, $password) {
    $database = Database_Users::getInstance();
    $login_okay = false;
    if ($database->getAdministratorCount() == 0) {
      // If there are no administrators listed in the database, 
      // then it uses the credentials set up during installation.
      include ("administration/credentials.php");
      if ($user_or_email == $site_admin_username && $password == $site_admin_password) {
        // Remove any default administrator. If left in, it could lock the real administrator out.
        include ("session/levels.php");
        $database->removeUser($user_or_email);
        $database->addNewUser($user_or_email, $password, ADMIN_LEVEL, "$site_admin_username@localhost.localdomain");
      }
    } 
    // Match username and email.
    if ($database->matchUsernamePassword ($user_or_email, $password)) {
      $login_okay = true;
    }
    // Match password and email.
    if ($database->matchEmailPassword ($user_or_email, $password)) {
      $login_okay = true;
      // Fetch username that belongs to the email address that was used to login.
      $user_or_email = $database->getEmailToUser ($user_or_email);
    }

    if ($login_okay) {
      $this->Open();
      $_SESSION['logged_in'] = true;
      $_SESSION['user'] = $user_or_email;
      $this->logged_in = true;
      return true;
    } else {
      return false;
    }
  }


  /**
  * Returns true if the user has logged in.
  */
  public function loggedIn () {
    // The logged-in status is stored in the singleton object, so that if it is requested twice,
    // the session system is queries only once. It has been seen on some sites that if the php session
    // system was queried more than once, it did not behave consistently.
    // Buffering the status in the object resolved this.
    if ($this->openAccess ()) return true;
    return $this->logged_in;
  }


  public function currentUser () {
    @$user = $_SESSION['user'];
    return $user;
  }


  /**
  * currentLevel - returns an integer with the current level of the session.
  */
  public function currentLevel ($force = false) {
    if ($this->openAccess ()) return  $this->level;
    if (($this->level == 0) || $force) {
      if ($this->loggedIn()) {
        $database = Database_Users::getInstance();
        $this->level = $database->getUserLevel ($this->currentUser());
      } else {
        include ("session/levels.php");
        $this->level = GUEST_LEVEL;
      }
    }
    return $this->level;
  }


  function logout () {
    $_SESSION['logged_in'] = false;
    $_SESSION['user'] = "";
  }
  
  
  function openAccess () {
    include ("administration/credentials.php");
    if ($open_installation) {
      $_SESSION['logged_in'] = true;
      $_SESSION['user'] = "admin";
      include ("session/levels.php");
      $this->level = ADMIN_LEVEL;
      $this->logged_in = true;
      return true;
    }
    return false;
  }


}


?>