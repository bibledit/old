<?php
    
require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);

// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user = $_POST['user'];
  $pass = $_POST['pass'];
  if (strlen ($user) < 2) {
    $form_is_valid = false;
    $smarty->assign ('username_or_email_invalid_message', gettext ("Username should be at least two characters long"));
  }
  if (strlen ($pass) < 4) {
    $form_is_valid = false;
    $smarty->assign ('password_invalid_message', gettext ("Password should be at least four characters long"));
  }
  if ($form_is_valid) {
    $session_logic = Session_Logic::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    if ($session_logic->attemptLogin ($user, $pass)) {
      // Log the login.
      $database_logs->log ($session_logic->currentUser () . " logged in");
      // Store web site's base URL.
      $database_config_general = Database_Config_General::getInstance ();
      @$siteUrl = dirname (dirname ($_SERVER["HTTP_REFERER"]));
      $database_config_general->setSiteURL ($siteUrl);
    } else {
      $smarty->assign ('error_message', gettext ("Username or email address or password are not correct"));
      $session_logic->logout();
      // Log the login failure.
      $database_logs->log ("Failed login attempt for user $user with password $pass");
    }
  }
}

$session_logic = Session_Logic::getInstance ();
if ($session_logic->loggedIn ()) {
  $mail = Database_Mail::getInstance ();
  $mailcount = $mail->getMailCount ();
  $smarty->assign ('mailcount', $mailcount);
  $smarty->display("loggedin.tpl");
} else {
  $smarty->assign ('logging_in', true);
  $smarty->display("login.tpl");
}

?>
