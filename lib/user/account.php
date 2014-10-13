<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MEMBER_LEVEL);


Assets_Page::header (Locale_Translate::_("Account"));
$view = new Assets_View (__FILE__);
$session_logic = Session_Logic::getInstance ();
$username = $session_logic->currentUser ();
$database_users  = Database_Users::getInstance ();
$email = $database_users->getUserToEmail ($username);
$actions_taken   = false;
$success_messages = array ();


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
      $view->view->new_password_invalid_message = Locale_Translate::_("Password should be at least four characters long");
    }
    if (strlen ($newpassword2) < 4) {
      $form_is_valid = false;
      $view->view->new_password2_invalid_message = Locale_Translate::_("Password should be at least four characters long");
    }
    if ($newpassword2 != $newpassword) {
      $form_is_valid = false;
      $view->view->new_password2_invalid_message = Locale_Translate::_("Passwords do not match");
    }
    if (!$database_users->matchUsernamePassword ($username, $currentpassword)) {
      $form_is_valid = false;
      $view->view->current_password_invalid_message = Locale_Translate::_("Current password is not valid");
    }
    if ($form_is_valid) {
      $database_users->updateUserPassword ($username, $newpassword);
      $actions_taken = true;
      $success_messages[] = Locale_Translate::_("The new password was saved");
    }
  }

  if ($newemail != "") {
    $validator = new Zend_Validate_EmailAddress ();
    if (!$validator->isValid ($newemail)) {
      $form_is_valid = false;
      $view->view->new_email_invalid_message = Locale_Translate::_("Email address is not valid");
    }
    if (!$database_users->matchUsernamePassword ($username, $currentpassword)) {
      $form_is_valid = false;
      $view->view->current_password_invalid_message = Locale_Translate::_("Current password is not valid");
    }
    if ($form_is_valid) {
      $confirm_worker = Confirm_Worker::getInstance ();
      $initial_subject = Locale_Translate::_("Email address verification");
      $initial_body = Locale_Translate::_("Somebody requested to change the email address that belongs to your account.");
      $query = $database_users->updateEmailQuery ($username, $newemail);
      $subsequent_subject = Locale_Translate::_("Email address change");
      $subsequent_body = Locale_Translate::_("The email address that belongs to your account has been changed successfully.");
      $confirm_worker->setup ($newemail, $initial_subject, $initial_body, $query, $subsequent_subject, $subsequent_body);
      $actions_taken = true;
      $success_messages[] = Locale_Translate::_("A verification email was sent to $newemail");
    }
  }

  if (!$actions_taken) {
    $success_messages[] = Locale_Translate::_("No changes were made");
  }


}


$view->view->username = Filter_Html::sanitize ($username);
$view->view->email = Filter_Html::sanitize ($email);
$view->view->success_messages = $success_messages;
$view->view->actions_taken = $actions_taken;
$view->render ("account.php");


Assets_Page::footer ();


?>
