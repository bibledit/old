<?php
    

require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);


$smarty = new Smarty_Bibledit (__FILE__);


// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user = $_POST['user'];
  if (strlen ($user) < 4) {
    $smarty->assign ('error_message', gettext ("Username or email address is too short"));
    $form_is_valid = false;
  }
  $database_users = Database_Users::getInstance ();
  if ($form_is_valid) {
    $form_is_valid = false;
    $email = $database_users->getUserToEmail ($user);
    if ($email != "") {
      $form_is_valid = true;
    }
    if (!$form_is_valid) {
      if ($database_users->emailExists ($user)) {
        $form_is_valid = true;
        $email = $user;
      }
    }
  }
  if ($form_is_valid) {
    // Generate and store a new password.
    $generated_password = substr (md5 (rand ()), 0, 15);
    $username = $database_users->getEmailToUser ($email);
    $database_users->updateUserPassword ($username, $generated_password);
    $database_mail = Database_Mail::getInstance ();
    // Send the new password to the user.
    $subject = gettext ("Account changed");
    $body = gettext ("Somebody requested a new password for your account.");
    $body .= "\n\n";
    $body .= gettext ("Here is the new password:");
    $body .= "\n\n";
    $body .= $generated_password;
    $body .= "\n\n";
    $body .= gettext ("It is recommended to log into your account with this new password, and then change it.");
    $database_mail->send ($username, $subject, $body);
    $smarty->assign ('success_message', gettext ("A message was sent to the email address belonging to this account to help you getting the password"));
  } else {
    $smarty->assign ('error_message', gettext ("Username or email address cannot be found"));
  }
}


$smarty->display("password.tpl");


?>