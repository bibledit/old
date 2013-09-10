<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
if (php_sapi_name () != "cli") {
  die;
}


$database_mail = Database_Mail::getInstance();
$database_mailer = Database_Mailer::getInstance();
$send_mails = $database_mail->getMailsInboxes ();
for ($i = 0; $i < count ($send_mails); $i++) {
  $id = $send_mails[$i];
  if (!$database_mailer->isPostponed ($id)) {
    sendMail ($id);
  }
}
$retry_mails = $database_mailer->getRetryMails ();
for ($i = 0; $i < count ($retry_mails); $i++) {
  $id = $retry_mails[$i];
  sendMail ($id);
}



/**
* Sends email $id
*/  
function sendMail ($id)
{
  // The databases involved.
  $database_mail   = Database_Mail::getInstance();
  $database_mailer = Database_Mailer::getInstance();
  $database_log    = Database_Logs::getInstance();
  $database_users  = Database_Users::getInstance();

  // Get all details of the mail.
  $result   = $database_mail->get ($id);
  $row      = $result->fetch_assoc();
  $username = $row['username'];
  $email    = $database_users->getUserToEmail ($username);
  $subject  = $row['subject'];
  $body     = $row['body'];

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
    $database_mail->delete ($id);
    $message = "Email to $email was deleted because of an invalid email address";
    $database_log->log ($message);
    return;
  }

  // Send the email.
  try {
    $mail = new Mail_Send($email, $username, $subject, $body);
    unset ($mail);
    $database_mail->delete ($id);
    if ($username == "") {
      // If the username does not exist, delete this mail from the Trash also.
      $database_mail->delete ($id);
    }
    $database_mailer->delete ($id);
    $message = "Email to $email was sent successfully";
    $database_log->log ($message);
  } catch (Exception $e) {
    $database_mailer->postpone ($id);      
    $message = $e->getMessage ();
    $message = "Email to $email could not be sent - reason: $message";
    $database_log->log ($message);
  }
}
  

?>
