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
page_access_level (Filter_Roles::GUEST_LEVEL);


Assets_Page::header (Locale_Translate::_("Password"));
$view = new Assets_View (__FILE__);


// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user = $_POST['user'];
  if (strlen ($user) < 4) {
    $view->view->error_message = Locale_Translate::_("Username or email address is too short");
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
    $subject = Locale_Translate::_("Account changed");
    $body = Locale_Translate::_("Somebody requested a new password for your account.");
    $body .= "\n\n";
    $body .= Locale_Translate::_("Here is the new password:");
    $body .= "\n\n";
    $body .= $generated_password;
    $body .= "\n\n";
    $body .= Locale_Translate::_("It is recommended to log into your account with this new password, and then change it.");
    $database_mail->send ($username, $subject, $body);
    $view->view->success_message = Locale_Translate::_("A message was sent to the email address belonging to this account to help you getting the password");
  } else {
    $view->view->error_message = Locale_Translate::_("Username or email address cannot be found");
  }
}

$view->render ("password.php");

Assets_Page::footer ();

?>
