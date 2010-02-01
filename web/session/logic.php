<?php




@include_once ("../bootstrap/bootstrap.php");
require_once ("bootstrap/bootstrap.php");




class Session_Logic
{

  public $check_browser = true;              // Include browser name in fingerprint?
  public $check_ip_blocks = 2;               // How many numbers from IP use in fingerprint?
  public $secure_word = 'BIBLETRANSLATION';  // Control word - any word you want.
  public $regenerate_id = true;              // Regenerate session ID to prevent fixation attacks?
  private static $instance;                  // Current singleton instance.


  // The class constructor is private, so no outsider can call it.    
  private function __construct() {
    session_start();
  } 


  // Gets or creates the singleton object instance.
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Session_Logic();
    }
    return self::$instance;
  }


  // Call this when init session.  public function Open()
  {
    $_SESSION['ss_fprint'] = $this->_Fingerprint();
    $this->_RegenerateId();
  }


  // Call this to check session.  public function Check()
  {
    $this->_RegenerateId();
    return (isset($_SESSION['ss_fprint']) && $_SESSION['ss_fprint'] == $this->_Fingerprint());
  }


  // Returns MD5 from fingerprint.  private function _Fingerprint()
  {
     $fingerprint = $this->secure_word;
     if ($this->check_browser) {
       $fingerprint .= $_SERVER['HTTP_USER_AGENT'];
     }
     if ($this->check_ip_blocks) {
       $num_blocks = abs(intval($this->check_ip_blocks));
       if ($num_blocks > 4) {
         $num_blocks = 4;
       }
       $blocks = explode('.', $_SERVER['REMOTE_ADDR']);
       for ($i = 0; $i < $num_blocks; $i++) {
         $fingerprint .= $blocks[$i] . '.';
       }
     }
     return md5($fingerprint);
  }


  // Regenerates session ID if possible.  private function _RegenerateId()
  {
    if ($this->regenerate_id && function_exists('session_regenerate_id')) {
      if (version_compare('5.1.0', phpversion(), '>=')) {
        session_regenerate_id(true);
      } else {
        session_regenerate_id();
      }
    }
  }


  public function attemptLogin ($user_or_email, $password) {
    if ($user_or_email == 'User' && $password == 'password') {
      $this->Open();
      $_SESSION['logged_in'] = true;
      $_SESSION['user'] = $user_or_email;
      return true;
    } else {
      return false;
    }
  }


  public function loggedIn () {
    if (!$this->Check() || !isset($_SESSION['logged_in']) || !$_SESSION['logged_in']) {
      return false;
    } else {
      //var_dump ($_SESSION);
      return true;
    }
  }


  public function currentUser () {
    return $_SESSION['user'];
  }

  function logout () {
    $_SESSION['logged_in'] = false;
  }


}



?>
