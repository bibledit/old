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


class Confirm_Worker
{
  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Confirm_Worker();
    }
    return self::$instance;
  }

  /**
  * setup - Sets up a confirmation cycle in order to change something in the database.
  *         If e.g. the user requests the email address to be changed, an initial email will be
  *         sent, which the user should confirm.
  * $to - Email address for the initial email and the response.
  * $initial_subject - The subject of the initial email message.
  * $initial_body - The body of the initial email message.
  * $query - The query to be executed on the database if the user confirms the email successfully.
  * $subsequent_subject - The subject of the email to send upon user confirmation.
  * $subsequent_body - The body of the email to send upon user confirmation.
  */
  public function setup ($to, $initial_subject, $initial_body, $query, $subsequent_subject, $subsequent_body)
  {
    $database_confirm = Database_Confirm::getInstance ();
    $confirmation_id = $database_confirm->getNewID ();
    $initial_subject .= " $confirmation_id";
    $initial_body .= "\n\n";
    $initial_body .= Locale_Translate::_("Please confirm this request by replying to this email. There is a confirmation number in the subject line. Your reply should have this same confirmation number in the subject line.");
    $database_mail = Database_Mail::getInstance();
    $database_mail->send ($to, $initial_subject, $initial_body);
    $database_confirm->store ($confirmation_id, $query, $to, $subsequent_subject, $subsequent_body);
  }

  /**
  * handleEmail - handles a confirmation email received from $from with subject $subject and body $body.
  * Returns true if the mail was handled, else false.
  */
  public function handleEmail ($from, $subject, $body)
  {
    // Find out in the confirmation database whether the $subject line contains an active ID.
    // If not, bail out.
    $database_confirm = Database_Confirm::getInstance();
    $id = $database_confirm->searchID ($subject);
    if ($id == 0) {
      return false;
    }
    // An active ID was found: Execute the associated database query.
    $query = $database_confirm->getQuery ($id);
    $database_users = Database_Users::getInstance ();
    Database_SQLite::exec ($database_users->db, $query);
    // Send confirmation mail.
    $mailto = $database_confirm->getMailTo ($id);
    $subject = $database_confirm->getSubject ($id);
    $body = $database_confirm->getBody ($id);
    $database_mail = Database_Mail::getInstance();
    $database_mail->send ($mailto, $subject, $body);
    // Delete the confirmation record.
    $database_confirm->delete ($id);
    // Job done.
    return true;
  }


  /**
  * handleVisit - handles a confirmation through a page visit
  */
  public function handleVisit ()
  {

  }

}


?>
