<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


class Notes_Logic
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Notes_Logic();
    }
    return self::$instance;
  }

  public function handlerNewNote ($identifier)
  {
    $this->notifyUsers ($identifier, gettext ("New"));
  }

  public function handlerAddComment ($identifier)
  {
    $this->notifyUsers ($identifier, gettext ("Comment"));
    // If the note' status was Done, and a comment is added, mark it Reopened.
    $database_notes = Database_Notes::getInstance ();
    $status = $database_notes->getRawStatus ($identifier);
    if ($status == "Done") {
      $database_notes->setStatus ($identifier, "Reopened");
    }
  }

  public function handlerUpdateNote ($identifier)
  {
    $this->notifyUsers ($identifier, gettext ("Updated"));
  }

  public function handlerAssignNote ($identifier, $user)
  {
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getUserAssignedConsultationNoteNotification ($user)) {
      // Only email the user if the user was not yet assigned this note.
      $database_notes = Database_Notes::getInstance();
      $assignees = $database_notes->getAssignees ($identifier);
      if (!in_array ($user, $assignees)) {
        $this->emailUsers ($identifier, gettext ("Assigned"), array ($user));
      }
    }
  }

  public function handlerDeleteNote ($identifier)
  {
    $this->notifyUsers ($identifier, gettext ("Deleted"));
  }

  /**
  * This handles notifications for the users
  * $identifier: the note that is being handled.
  * $label: prefix to the subject line of the email.
  */
  private function notifyUsers ($identifier, $label)
  {
    // Databases.
    $database_notes = Database_Notes::getInstance();
    $database_config_user = Database_Config_User::getInstance ();
    $database_users = Database_Users::getInstance();
    
    // Whether current user gets subscribed to the note.
    if ($database_config_user->getSubscribeToConsultationNotesEditedByMe ()) {
      $database_notes = Database_Notes::getInstance();
      $database_notes->subscribe ($identifier);
    }
    
    // Users to get subscribed to the note, or to whom the note is to be assigned.
    $users = $database_users->getUsers ();
    foreach ($users as $user) {
      if ($database_config_user->getNotifyUserOfAnyConsultationNotesEdits ($user)) {
        $database_notes->subscribeUser ($identifier, $user);
      }
      if ($database_config_user->getUserAssignedToConsultationNotesChanges ($user)) {
        $database_notes->assignUser ($identifier, $user, false); // Do not add a comment for this automatic assignment.
      }
    }

    // Email notification recipients.
    $recipients = array ();
    
    // Get the subscribers who receive an email notification.
    $subscribers = $database_notes->getSubscribers ($identifier);
    foreach ($subscribers as $subscriber) {
      if ($database_config_user->getUserSubscribedConsultationNoteNotification ($subscriber)) {
        $recipients [] = $subscriber;
      }
    }
    
    // Get the assignees who receive an email notification.
    $assignees = $database_notes->getAssignees ($identifier);
    foreach ($assignees as $assignee) {
      if ($database_config_user->getUserAssignedConsultationNoteNotification ($assignee)) {
        $recipients [] = $assignee;
      }
    }

    // Unique recipients, nobody gets duplicate email.
    $recipients = array_unique ($recipients);
    
    // Send mail to all recipients.
    $this->emailUsers ($identifier, $label, $recipients);
  }


  /**
  * This handles emails to the users
  * $identifier: the note that is being handled.
  * $label: prefix to the subject line of the email.
  * $users: array of users to be mailed.
  */
  private function emailUsers ($identifier, $label, $users)
  {
    // Databases.
    $database_notes = Database_Notes::getInstance();
    $database_config_user = Database_Config_User::getInstance ();
    $database_users = Database_Users::getInstance();
    $database_mail = Database_Mail::getInstance();
    
    // Send mail to all users.
    $summary = $database_notes->getSummary ($identifier);
    $passages = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($identifier));
    $contents = $database_notes->getContents ($identifier);
    // Include link to the note on the site. Saves the user searching for the note.
    $contents .= "<br>\n";
    @$referer = $_SERVER["HTTP_REFERER"];
    $caller = explode ("?", $referer);
    $caller = $caller[0];
    $link = "$caller?consultationnote=$identifier";
    $contents .= "<p><a href=\"$link\">$link</a></p>\n";
    foreach ($users as $user) {
      $database_mail->send ($user, "$label | $passages | $summary | (CNID$identifier)", $contents);
    }
  }
  
  

  /**
  * handleEmailComment - handles an email received from $from with subject $subject and body $body.
  * Returns true if the mail was processed, else false.
  * The email is considered to have been processed if it gave a comment on an existing Consultation Note.
  */
  public function handleEmailComment ($from, $subject, $body) // Todo
  {
    // Check whether the Consultation Notes Identifier in the $subject exists in the notes database.
    // The CNID looks like: (CNID123456789)
    $pos = strpos ($subject, "(CNID");
    if ($pos === false) return false;
    $subject = substr ($subject, $pos + 5);
    $pos = strpos ($subject, ")");
    if ($pos === false) return false;
    $subject = substr ($subject, 0, $pos);
    // At this stage, the $subject contains an identifier. Check that the $identifier is an existing Consultation Note.
    $identifier = $subject;
    unset ($subject);
    $database_notes = Database_Notes::getInstance();
    if (!$database_notes->identifierExists ($identifier)) return false;
    // Check that the $from address of the email belongs to an existing user.
    $from = Filter_Email::extractEmail ($from);
    $database_users = Database_Users::getInstance ();
    if (!$database_users->emailExists ($from)) return false;
    $username = $database_users->getEmailToUser ($from);
    // Clean the email's body.
    $config_general = Database_Config_General::getInstance ();
    $year = strftime ("%Y");
    $sender = $config_general->getSiteMailName();
    $body = Filter_Email::extractBody ($body, $year, $sender);
    // Make comment on the consultation note.
    @$sessionuser = $_SESSION['user'];
    @$_SESSION['user'] = $username;
    $database_notes->addComment ($identifier, $body);
    $this->handlerAddComment ($identifier);
    $_SESSION['user'] = $sessionuser;
    // Mail confirmation to the $username.
    $database_mail = Database_Mail::getInstance();
    $subject = gettext ("Your comment was posted");
    $database_mail->send ($username, "$subject [CNID$identifier]", $body);
    // Log operation.
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("Comment posted" . ":" . " " . $body);
    // Job done.
    return true;
  }



  /**
  * handleEmailNew - handles an email received from $from with subject $subject and body $body.
  * Returns true if the mail was processed, else false.
  * The email is considered to have been processed if it created a new Consultation Note.
  */
  public function handleEmailNew ($from, $subject, $body)
  {
    // Look if the Consultation Notes Identifier in the $subject exists
    /*
    // Find out in the confirmation database whether the $subject line contains an active ID.
    // If not, bail out.
    $database_confirm = Database_Confirm::getInstance();
    $id = $database_confirm->searchID ($subject);
    if ($id == 0) {
      return false;
    }
    // An active ID was found: Execute the associated database query.
    $query = $database_confirm->getQuery ($id);
    $server = Database_Instance::getInstance ();
    $server->runQuery ($query);
    // Send confirmation mail.
    $mailto = $database_confirm->getMailTo ($id);
    $subject = $database_confirm->getSubject ($id);
    $body = $database_confirm->getBody ($id);
    $database_mail = Database_Mail::getInstance();
    $database_mail->send ($mailto, $subject, $body);
    // Delete the confirmation record.
    $database_confirm->delete ($id);
    */
    // Job done.
    return false;
  }




  

}  


?>
