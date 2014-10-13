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


// The script runs through the cli Server API only.
Filter_Cli::assert ();


$database_mail = Database_Mail::getInstance();


$mails = $database_mail->getMailsToSend ();
foreach ($mails as $id) {
  sendMail ($id);
}


/**
* Sends email $id
*/
function sendMail ($id)
{
  // The databases involved.
  $database_mail = Database_Mail::getInstance();
  $database_log = Database_Logs::getInstance();
  $database_users = Database_Users::getInstance();

  // Get all details of the mail.
  $details = $database_mail->get ($id);
  $username = $details ['username'];
  $email = $database_users->getUserToEmail ($username);
  $subject = $details ['subject'];
  $body = $details ['body'];

  // Bail out when username was empty.
  if ($username == "") return;

  // If this username was not found, it could be that the email was addressed to a non-user,
  // and that the To: address was actually contained in the $username.
  if ($email == "") {
    $email = $username;
    $username = "";
  }

  // If the email address validates, ok, else remove this mail from the queue and log the action.
  $validator = new Zend_Validate_EmailAddress ();
  if (!$validator->isValid ($email)) {
    $database_mail->delete ($id);
    $message = "Email to $email was deleted because of an invalid email address";
    $database_log->log ($message);
    return;
  }

  // Send the email.
  try {
    $mail = new Mail_Send ($email, $username, $subject, $body);
    unset ($mail);
    $database_mail->delete ($id);
    $message = "Email to $email with subject $subject was sent successfully";
    $database_log->log ($message, Filter_Roles::MANAGER_LEVEL);
  } catch (Exception $e) {
    $database_mail->postpone ($id);
    $message = $e->getMessage ();
    $message = "Email to $email could not be sent - reason: $message";
    $database_log->log ($message);
  }
}


?>
