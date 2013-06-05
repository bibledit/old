<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


  const notifyNoteNew     = 1;
  const notifyNoteComment = 2;
  const notifyNoteUpdate  = 3;
  const notifyNoteDelete  = 4;


  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Notes_Logic();
    }
    return self::$instance;
  }


  public function handlerNewNote ($identifier)
  {
    $this->notifyUsers ($identifier, self::notifyNoteNew);
  }


  public function handlerAddComment ($identifier)
  {
    $this->notifyUsers ($identifier, self::notifyNoteComment);
    // If the note status was Done, and a comment is added, mark it Reopened.
    $database_notes = Database_Notes::getInstance ();
    $status = $database_notes->getRawStatus ($identifier);
    if ($status == "Done") {
      $database_notes->setStatus ($identifier, "Reopened");
    }
  }

  public function handlerUpdateNote ($identifier)
  {
    $this->notifyUsers ($identifier, self::notifyNoteUpdate);
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
    $this->notifyUsers ($identifier, self::notifyNoteDelete);
  }


  /**
  * This handles notifications for the users
  * $identifier: the note that is being handled.
  * $notification: the type of notification to the consultation note.
  */
  private function notifyUsers ($identifier, $notification)
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

    // The recipients who receive a notification by email.
    $recipients = array ();
    
    // Subscribers who receive email.
    $subscribers = $database_notes->getSubscribers ($identifier);
    foreach ($subscribers as $subscriber) {
      if ($database_config_user->getUserSubscribedConsultationNoteNotification ($subscriber)) {
        $recipients [] = $subscriber;
      }
    }
    
    // Assignees who receive email.
    $assignees = $database_notes->getAssignees ($identifier);
    foreach ($assignees as $assignee) {
      if ($database_config_user->getUserAssignedConsultationNoteNotification ($assignee)) {
        $recipients [] = $assignee;
      }
    }

    // In case the consultation note is deleted, notify only the users with this specific notification set.
    if ($notification == self::notifyNoteDelete) {
      $recipients = array ();
      $users = $database_users->getUsers ();
      foreach ($users as $user) {
        if ($database_config_user->getUserDeletedConsultationNoteNotification ($user)) {
          $recipients [] = $user;
        }
      }
    }
    
    // Remove duplicates from the recipients.
    $recipients = array_unique ($recipients);
    
    // Deal with suppressing mail to the user when he made the update himself.
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    if ($database_config_user->getUserSuppressMailFromYourUpdatesNotes ($username)) {
      $recipients = array_diff ($recipients, array ($username));
    }

    // Generate the label prefixed to the subject line of the email.
    $label = gettext ("General");
    switch ($notification) {
      case self::notifyNoteNew     : $label = gettext ("New");     break;
      case self::notifyNoteComment : $label = gettext ("Comment"); break;
      case self::notifyNoteUpdate  : $label = gettext ("Updated"); break;
      case self::notifyNoteDelete  : $label = gettext ("Deleted"); break;
    }
    
    // Send mail to all recipients.
    $this->emailUsers ($identifier, $label, $recipients);
  }


  /**
  * This handles email to users.
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
    $database_config_general = Database_Config_General::getInstance ();
    
    // Send mail to all users.
    $summary = $database_notes->getSummary ($identifier);
    $passages = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($identifier));
    $contents = $database_notes->getContents ($identifier);
    // Include a link to the note on the site.
    $contents .= "<br>\n";
    $siteUrl = $database_config_general->getSiteURL ();
    $link = "$siteUrl/consultations/notes.php?consultationnote=$identifier";
    $contents .= "<p><a href=\"$link\">View or respond online</a></p>\n";
    $mailto = "mailto:" . $database_config_general->getSiteMailAddress () . "?subject=(CNID$identifier)";
    $contents .= "<p><a href=\"$mailto\">Respond by email</a></p>\n";
    foreach ($users as $user) {
      $database_mail->send ($user, "$label | $passages | $summary | (CNID$identifier)", $contents);
    }
  }
  
  

  /**
  * handleEmailComment - handles an email received from $from with subject $subject and body $body.
  * Returns true if the mail was processed, else false.
  * The email is considered to have been processed if it comments on an existing Consultation Note.
  */
  public function handleEmailComment ($from, $subject, $body)
  {
    // Check whether the Consultation Notes Identifier in the $subject exists in the notes database.
    // The CNID looks like: (CNID123456789)
    $pos = strpos ($subject, "(CNID");
    if ($pos === false) return false;
    $subject = substr ($subject, $pos + 5);
    $pos = strpos ($subject, ")");
    if ($pos === false) return false;
    $subject = substr ($subject, 0, $pos);
    // At this stage, the $subject contains an identifier.
    // Check that the $identifier is an existing Consultation Note.
    $identifier = $subject;
    unset ($subject);
    $database_notes = Database_Notes::getInstance();
    if (!$database_notes->identifierExists ($identifier)) return false;
    // Check that the $from address of the email belongs to an existing user.
    // Or else use the obfuscated email address as the user name.
    $from = Filter_Email::extractEmail ($from);
    $database_users = Database_Users::getInstance ();
    if ($database_users->emailExists ($from)) {
      $username = $database_users->getEmailToUser ($from);
    } else {
      $username = $from;
      $username = str_replace ("@", " ", $username);
      $username = str_replace (".", " ", $username);
    }
    // Clean the email's body.
    $config_general = Database_Config_General::getInstance ();
    $year = strftime ("%Y");
    $sender = $config_general->getSiteMailName();
    $body = Filter_Email::extractBody ($body, $year, $sender);
    // Remove any new lines from the body. This cleans up the email considerably,
    // because some emails that get posted would otherwise look untidy,
    // when the many new lines take up a lot of space.
    $body = str_replace ("\n", " ", $body);
    // Make comment on the consultation note.
    @$sessionuser = $_SESSION['user'];
    @$_SESSION['user'] = $username;
    $database_notes->addComment ($identifier, $body);
    $this->handlerAddComment ($identifier);
    $_SESSION['user'] = $sessionuser;
    // Mail confirmation to the $username.
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getUserNotifyMeOfMyPosts ($username)) {
      $database_mail = Database_Mail::getInstance();
      $subject = gettext ("Your comment was posted");
      $database_mail->send ($username, "$subject [CNID$identifier]", $body);
    }
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
    // Store the original subject.
    $originalSubject = $subject;
    // Check that the subject indicates that a new consultation note is to be created.
    $pos = strpos (strtolower ($subject), "new note");
    if ($pos === false) return false;
    // There is a new note. Remove that bit from the $subject.
    $subject = substr ($subject, 0, $pos) . substr ($subject, $pos + 8);
    // Clean the subject line.
    $subject = trim ($subject);
    $subject = str_replace (".", " ", $subject);
    $subject = str_replace (":", " ", $subject);
    $subject = str_replace ("  ", " ", $subject);
    $subject = str_replace ("  ", " ", $subject);
    $subject = str_replace ("  ", " ", $subject);
    $subject = str_replace ("  ", " ", $subject);
    // Check that the $from address of the email belongs to an existing user.
    $from = Filter_Email::extractEmail ($from);
    $database_users = Database_Users::getInstance ();
    if (!$database_users->emailExists ($from)) return false;
    $username = $database_users->getEmailToUser ($from);
    // Extract book, chapter, verse, and note summary from the $subject
    $book = NULL;
    $chapter = NULL;
    $verse = NULL;
    $summary = NULL;
    $subject = explode (" ", $subject);
    if (count ($subject) > 0) {
      $book = Filter_Books::interpretBook ($subject[0]);
    }
    if (count ($subject) > 1) {
      $chapter = Filter_Numeric::integer_in_string ($subject[1]);
    }
    if (count ($subject) > 2) {
      $verse = Filter_Numeric::integer_in_string ($subject[2]);
    }
    unset ($subject[0]);
    unset ($subject[1]);
    unset ($subject[2]);
    $summary = implode (" ", $subject);
    unset ($subject);
    // Check book, chapter, verse, and summary. Give feedback if there's anything wrong.
    $noteCheck = "";
    if (!((is_numeric ($book) && ($book > 0)))) $noteCheck .= gettext ("Unknown book");
    if (!is_numeric ($chapter)) $noteCheck .= " " . gettext ("Unknown chapter");
    if (!is_numeric ($verse)) $noteCheck .= " " . gettext ("Unknown verse");
    if (($summary == NULL) || ($summary == "")) $noteCheck .= " " . gettext ("Unknown summary");
    // Mail user if the note could not be posted.
    $database_mail = Database_Mail::getInstance();
    if ($noteCheck != "") {
      $subject = gettext ("Your new note could not be posted");
      $database_mail->send ($username, $subject  . ": " . $originalSubject, $noteCheck);
      return false;
    }
    // Clean the email's body.
    $body = Filter_Email::extractBody ($body);
    // Post the note.
    @$sessionuser = $_SESSION['user'];
    @$_SESSION['user'] = $username;
    $database_notes = Database_Notes::getInstance();
    $identifier = $database_notes->storeNewNote ("", $book, $chapter, $verse, $summary, $body, false);
    $this->handlerNewNote ($identifier);
    $_SESSION['user'] = $sessionuser;
    // Mail confirmation to the $username.
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getUserNotifyMeOfMyPosts ($username)) {
      $subject = gettext ("Your new note was posted");
      $database_mail->send ($username, $subject . ": " . $originalSubject, $body);
    }
    // Log operation.
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("New note posted" . ":" . " " . $body);
    // Job done.
    return true;
  }
  

}  


?>
