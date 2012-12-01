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
    if ($session_logic->attemptLogin ($user, $pass)) {
      // If a non-admin logs in, send admin mail.
      $mail = Database_Mail::getInstance ();
      $users = Database_Users::getInstance ();
      $admin = $users->getAdministrator ();
      $subject = $session_logic->currentUser () . " logged in";
      // Admins should not be notified if admins login. If it would be done, then each time an admin logs in,
      // he would be greeted with a message saying that there is mail for him. This one mail would be his own login notification.
      include ("session/levels.php");
      if ($session_logic->currentLevel (true) != ADMIN_LEVEL) {
        $mail->send ($admin, $subject, "");
      }
      // Store web site's base URL.
      $database_config_general = Database_Config_General::getInstance ();
      @$siteUrl = dirname (dirname ($_SERVER["HTTP_REFERER"]));
      $database_config_general->setSiteURL ($siteUrl);
    } else {
      $smarty->assign ('error_message', gettext ("Username or email address or password are not correct"));
      $session_logic->logout();
      // Inform administrator about the login failure.
      $mail = Database_Mail::getInstance ();
      $users = Database_Users::getInstance ();
      $admin = $users->getAdministrator ();
      $subject = "Failed login attempt";
      $body = "Failed login attempt for user $user with password $pass";
      $mail->send ($admin, $subject, $body);
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