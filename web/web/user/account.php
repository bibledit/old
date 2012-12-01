<?php
    

require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);



$smarty = new Smarty_Bibledit (__FILE__);
$session_logic = Session_Logic::getInstance ();
$username = $session_logic->currentUser ();
$database_users  = Database_Users::getInstance ();
$email = $database_users->getUserToEmail ($username);
$actions_taken   = false;


// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $currentpassword = $_POST['currentpassword'];
  $newpassword     = $_POST['newpassword'];
  $newpassword2    = $_POST['newpassword2'];
  $newemail        = $_POST['newemail'];

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
    $validator = new Zend_Validate_EmailAddress ();
    if (!$validator->isValid ($newemail)) {
      $form_is_valid = false;
      $smarty->assign ('new_email_invalid_message', gettext ("Email address is not valid"));
    }
    if (!$database_users->matchUsernamePassword ($username, $currentpassword)) {
      $form_is_valid = false;
      $smarty->assign ('current_password_invalid_message', gettext ("Current password is not valid"));
    }
    if ($form_is_valid) {
      $confirm_worker = Confirm_Worker::getInstance ();
      $initial_subject = gettext ("Email address verification");
      $initial_body = gettext ("Somebody requested to change the email address that belongs to your account.");
      $query = $database_users->updateEmailQuery ($username, $newemail);
      $subsequent_subject = gettext ("Email address change");
      $subsequent_body = gettext ("The email address that belongs to your account has been changed successfully.");
      $confirm_worker->setup ($newemail, $initial_subject, $initial_body, $query, $subsequent_subject, $subsequent_body);
      $actions_taken = true;
      $success_messages[] = gettext ("A verification email was sent to $newemail");
    }
  }

  if (!$actions_taken) {
    $success_messages[] = gettext ("No changes were made");
  }


}


$smarty->assign ('username', Filter_Html::sanitize ($username));
$smarty->assign ('email', Filter_Html::sanitize ($email));
@$smarty->assign ('success_messages', $success_messages);
@$smarty->assign ('actions_taken', $actions_taken);
$smarty->display("account.tpl");


?>