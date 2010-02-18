<?php
    

require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);



$smarty = new Smarty_Bibledit (__FILE__);
$session_logic = Session_Logic::getInstance ();
$username = $session_logic->currentUser ();


// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $actions_taken   = false;
  $currentpassword = $_POST['currentpassword'];
  $newpassword     = $_POST['newpassword'];
  $newpassword2    = $_POST['newpassword2'];
  $newemail        = $_POST['newemail'];
  $database_users  = Database_Users::getInstance ();

  if (($newpassword != "") || ($newpassword2 != "")) {
    if (strlen ($newpassword) < 4) {
      $form_is_valid = false;
      $smarty->assign ('new_password_invalid_message', gettext ("Password should be at least four characters long"));
    }
    if (strlen ($newpassword2) < 4) {
      $form_is_valid = false;
      $smarty->assign ('new_password2_invalid_message', gettext ("Password should be at least four characters long"));
    }
    if ($newpassword2 != $newpassword) {
      $form_is_valid = false;
      $smarty->assign ('new_password2_invalid_message', gettext ("Passwords do not match"));
    }
    if (!$database_users->matchUsernamePassword ($username, $currentpassword)) {
      $form_is_valid = false;
      $smarty->assign ('current_password_invalid_message', gettext ("Current password is not valid"));
    }
    if ($form_is_valid) {
      $database_users->updateUserPassword ($username, $newpassword);
      $actions_taken = true;
      $success_messages[] = gettext ("The new password was saved");
    }
  }
  
  if ($newemail != "") {
    if (!Validate_Email::valid ($newemail)) {
      $form_is_valid = false;
      $smarty->assign ('new_email_invalid_message', gettext ("Email address is not valid"));
    }
    if (!$database_users->matchUsernamePassword ($username, $currentpassword)) {
      $form_is_valid = false;
      $smarty->assign ('current_password_invalid_message', gettext ("Current password is not valid"));
    }
    if ($form_is_valid) {
      $database_mail = Database_Mail::getInstance();
      $subject = "Email address verification";
      $body = "You have requested to change the email address that belongs to your account. Please confirm this new email address.";
      $database_mail->send ($newemail, $subject, $body);
      $actions_taken = true;
      $success_messages[] = gettext ("A verification email was sent to $newemail");
    }
  }

  if (!$actions_taken) {
    $success_messages[] = gettext ("No changes were made");
  }


}


$smarty->assign ('username', $username);
$smarty->assign ('success_messages', $success_messages);
$smarty->display("account.tpl");


?>
