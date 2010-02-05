<?php
    

require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);



$smarty = new Smarty_Bibledit (__FILE__);


// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user = $_POST['user'];
  $pass = $_POST['pass'];
  if (strlen ($user) < 4) {
    $form_is_valid = false;
    $smarty->assign ('username_or_email_invalid_message', gettext ("Username should be at least four characters long"));
  }
  if (strlen ($pass) < 4) {
    $form_is_valid = false;
    $smarty->assign ('password_invalid_message', gettext ("Password should be at least four characters long"));
  }
  if ($form_is_valid) {
    $session_logic = Session_Logic::getInstance ();
    if (!$session_logic->attemptLogin ($user, $pass)) {
      $smarty->assign ('error_message', gettext ("Username or email address or password are not correct"));
      $session_logic->logout();
    }
  }
}


$session_logic = Session_Logic::getInstance ();
if ($session_logic->loggedIn ()) {
  //header('Location: ../index.php');
  //header('Refresh: 1; URL=../index.php');
  $smarty->display("loggedin.tpl");
} else {
  $smarty->assign ('logging_in', true);
  $smarty->display("login.tpl");
}


?>