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


class Notes_Logic
{

  private static $instance;


  const lowNoteIdentifier = 100000000;
  const highNoteIdentifier = 999999999;


  const notifyNoteNew             = 1;
  const notifyNoteComment         = 2;
  const notifyNoteUpdate          = 3;
  const notifyNoteDelete          = 4;
  const notifyMarkNoteForDeletion = 5;


  // Definitions for note actions.
  const noteActionCreate = 1;
  const noteActionComment = 2;
  const noteActionSummary = 3;
  const noteActionSubscribe = 4;
  const noteActionUnsubscribe = 5;
  const noteActionAssign = 6;
  const noteActionUnassign = 7;
  const noteActionStatus = 8;
  const noteActionPassage = 9;
  const noteActionSeverity = 10;
  const noteActionBible = 11;
  const noteActionMarkDeletion = 12;
  const noteActionUnmarkDeletion = 13;
  const noteActionDelete = 14;
  const noteActionGet = 15;


  private function __construct() {
  }


  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Notes_Logic();
    }
    return self::$instance;
  }


  // Create a consultation note.
  // $bible: The notes's Bible.
  // $book, $chapter, $verse: The note's passage.
  // $summary: The note's summary.
  // $contents: The note's contents.
  // $raw: Import $contents as it is.
  // It returns the $identifier of this new note.
  public function createNote ($bible, $book, $chapter, $verse, $summary, $contents, $raw)
  { 
    $summary = str_replace ("\n", "", $summary);
    $database_notes = Database_Notes::getInstance ();
    $note_id = $database_notes->storeNewNote ($bible, $book, $chapter, $verse, $summary, $contents, $raw);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $data = Filter_Client::createNoteEncode ($bible, $book, $chapter, $verse, $summary, $contents, $raw);
      $database_noteactions->record ($session_logic->currentUser (), $note_id, self::noteActionCreate, $data);
    } else {
      // Server: do the notifications.
      $this->handlerNewNote ($note_id);
    }
    return $note_id;
  }


  // Add a $comment to an exiting note identified by $identifier.
  public function addComment ($identifier, $comment)
  {
    // Do nothing if there was no content.
    if ($comment == "") return;

    $database_notes = Database_Notes::getInstance ();
    $database_notes->addComment ($identifier, $comment);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionComment, $comment);
    } else {
      // Server: do the notifications.
      $this->handlerAddComment ($identifier);
    }
  }
  

  public function setSummary ($identifier, $summary)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->setSummary ($identifier, $summary);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionSummary, $summary);
    } else {
      // Server: do nothing extra.
    }
  }

  
  public function subscribe ($identifier)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->subscribe ($identifier);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionSubscribe, "");
    } else {
      // Server: do nothing extra.
    }
  }

  
  public function unsubscribe ($identifier)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->unsubscribe ($identifier);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionUnsubscribe, "");
    } else {
      // Server: do nothing extra.
    }
  }

  
  public function assignUser ($identifier, $user)
  {
    $database_notes = Database_Notes::getInstance ();
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionAssign, $user);
    } else {
      // Server: do the notifications.
      // Assign logic comes before the database action in this particular case.
      $this->handlerAssignNote ($identifier, $user);
    }
    $database_notes->assignUser ($identifier, $user);
  }

  
  public function unassignUser ($identifier, $user)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->unassignUser ($identifier, $user);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionUnassign, $user);
    } else {
      // Server: do nothing extra.
    }
  }

  
  public function setStatus ($identifier, $status)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->setStatus ($identifier, $status);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionStatus, $status);
    } else {
      // Server: do nothing extra.
    }
  }


  // Set the passages for note $identifier.
  // $passages is an array of an array (book, chapter, verse) passages.
  public function setPassages ($identifier, $passages)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->setPassages ($identifier, $passages);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $passages = serialize ($passages);
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionPassage, $passages);
    } else {
      // Server: do nothing extra.
    }
  }


  // Sets the $severity as a number for note $identifier.
  public function setRawSeverity ($identifier, $severity)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->setRawSeverity ($identifier, $severity);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionSeverity, $severity);
    } else {
      // Server: do nothing extra.
    }
  }


  // Sets the $bible for note $identifier.
  public function setBible ($identifier, $bible)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->setBible ($identifier, $bible);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionBible, $bible);
    } else {
      // Server: do nothing extra.
    }
  }


  public function markForDeletion ($identifier)
  {
    $database_notes = Database_Notes::getInstance ();
    $trash_handler = Trash_Handler::getInstance ();
    $database_notes->markForDeletion ($identifier);
    $trash_handler->consultationNote ($identifier);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionMarkDeletion, "");
    } else {
      // Server: notifications.
      $this->handlerMarkNoteForDeletion ($identifier);
    }
  }


  public function unmarkForDeletion ($identifier)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_notes->unmarkForDeletion ($identifier);
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionUnmarkDeletion, "");
    } else {
      // Server: do nothing extra.
    }
  }


  public function delete ($identifier)
  {
    $database_notes = Database_Notes::getInstance ();
    $trash_handler = Trash_Handler::getInstance ();
    if (Filter_Client::enabled ()) {
      // Client: record the action in the database.
      $database_noteactions = Database_NoteActions::getInstance ();
      $session_logic = Session_Logic::getInstance ();
      $database_noteactions->record ($session_logic->currentUser (), $identifier, self::noteActionDelete, "");
    } else {
      // Server: notification.
      $this->handlerDeleteNote ($identifier);
    }
    $trash_handler->consultationNote ($identifier);
    $database_notes->delete ($identifier);
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
    // Take no action in client mode.
    $database_config_general = Database_Config_General::getInstance ();
    if (Filter_Client::enabled ()) return;

    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getUserAssignedConsultationNoteNotification ($user)) {
      // Only email the user if the user was not yet assigned this note.
      $database_notes = Database_Notes::getInstance();
      $assignees = $database_notes->getAssignees ($identifier);
      if (!in_array ($user, $assignees)) {
        $this->emailUsers ($identifier, Locale_Translate::_("Assigned"), array ($user), false);
      }
    }
  }


  public function handlerMarkNoteForDeletion ($identifier)
  {
    $this->notifyUsers ($identifier, self::notifyMarkNoteForDeletion);
  }


  public function handlerDeleteNote ($identifier)
  {
    $this->notifyUsers ($identifier, self::notifyNoteDelete);
  }


  /**
  * This handles notifications for the users
  * $identifier: the note that is being handled.
  * $notification: the type of action on the consultation note.
  */
  private function notifyUsers ($identifier, $notification)
  {
    // Take no action in client mode.
    $database_config_general = Database_Config_General::getInstance ();
    if (Filter_Client::enabled ()) return;

    // Databases.
    $database_notes = Database_Notes::getInstance();
    $database_config_user = Database_Config_User::getInstance ();
    $database_users = Database_Users::getInstance();
    
    
    // This notes's Bible.
    $bible = $database_notes->getBible ($identifier);
    

    // Subscription and assignment not to be used for notes marked for deletion,
    // because marking notes for deletion is nearly the same as deleting them straightaway.
    if ($notification != self::notifyMarkNoteForDeletion) {

      // Whether current user gets subscribed to the note.
      if ($database_config_user->getSubscribeToConsultationNotesEditedByMe ()) {
        $database_notes = Database_Notes::getInstance();
        $database_notes->subscribe ($identifier);
      }

      // Users to get subscribed to the note, or to whom the note is to be assigned.
      $users = $database_users->getUsers ();
      foreach ($users as $user) {
        if ($database_users->hasAccess2Bible ($user, $bible)) {
          if ($database_config_user->getNotifyUserOfAnyConsultationNotesEdits ($user)) {
            $database_notes->subscribeUser ($identifier, $user);
          }
          if ($database_config_user->getUserAssignedToConsultationNotesChanges ($user)) {
            $database_notes->assignUser ($identifier, $user, false);
          }
        }
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

    // In case the consultation note is deleted or marked for deletion,
    // notify only the users with this specific notification set.
    if (($notification == self::notifyNoteDelete) || ($notification == self::notifyMarkNoteForDeletion)) {
      $recipients = array ();
      $users = $database_users->getUsers ();
      foreach ($users as $user) {
        if ($database_config_user->getUserDeletedConsultationNoteNotification ($user)) {
          if ($database_users->hasAccess2Bible ($user, $bible)) {
            $recipients [] = $user;
          }
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
    $label = Locale_Translate::_("General");
    switch ($notification) {
      case self::notifyNoteNew             : $label = Locale_Translate::_("New");                 break;
      case self::notifyNoteComment         : $label = Locale_Translate::_("Comment");             break;
      case self::notifyNoteUpdate          : $label = Locale_Translate::_("Updated");             break;
      case self::notifyNoteDelete          : $label = Locale_Translate::_("Deleted");             break;
      case self::notifyMarkNoteForDeletion : $label = Locale_Translate::_("Marked for deletion"); break;
    }

    // Optional postponing sending email.
    $postpone = false;
    if ($notification == self::notifyNoteNew) {
      if ($database_config_user->getPostponeNewNotesMails ()) {
        $postpone = true;
      }
    }

    // Send mail to all recipients.
    $this->emailUsers ($identifier, $label, $recipients, $postpone);
  }


  /**
  * This handles email to users.
  * $identifier: the note that is being handled.
  * $label: prefix to the subject line of the email.
  * $users: array of users to be mailed.
  * $postpone: whether to postpone sending the email till the evening.
  */
  private function emailUsers ($identifier, $label, $users, $postpone)
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
    $link = "$siteUrl/notes/note.php?id=$identifier";
    $contents .= "<p><a href=\"$link\">View or respond online</a></p>\n";
    $mailto = "mailto:" . $database_config_general->getSiteMailAddress () . "?subject=(CNID$identifier)";
    $contents .= "<p><a href=\"$mailto\">Respond by email</a></p>\n";

    // Deal with possible postponing email.
    $timestamp = time ();
    if ($postpone) {
      $postpone = strtotime ("today +21 hours");
      if ($postpone > $timestamp) $timestamp = $postpone;
    }

    // Send (but not in client mode).
    foreach ($users as $user) {
      if (!Filter_Client::enabled ()) $database_mail->send ($user, "$label | $passages | $summary | (CNID$identifier)", $contents, $timestamp);
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
    $session_logic = Session_Logic::getInstance ();
    $sessionuser = $session_logic->currentUser ();
    $session_logic->setUsername ($username);
    $this->addComment ($identifier, $body);
    $session_logic->setUsername ($sessionuser);
    // Mail confirmation to the $username.
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getUserNotifyMeOfMyPosts ($username)) {
      $database_mail = Database_Mail::getInstance();
      $subject = Locale_Translate::_("Your comment was posted");
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
    if (!((is_numeric ($book) && ($book > 0)))) $noteCheck .= Locale_Translate::_("Unknown book");
    if (!is_numeric ($chapter)) $noteCheck .= " " . Locale_Translate::_("Unknown chapter");
    if (!is_numeric ($verse)) $noteCheck .= " " . Locale_Translate::_("Unknown verse");
    if (($summary == NULL) || ($summary == "")) $noteCheck .= " " . Locale_Translate::_("Unknown summary");
    // Mail user if the note could not be posted.
    $database_mail = Database_Mail::getInstance();
    if ($noteCheck != "") {
      $subject = Locale_Translate::_("Your new note could not be posted");
      $database_mail->send ($username, $subject  . ": " . $originalSubject, $noteCheck);
      return false;
    }
    // Clean the email's body.
    $body = Filter_Email::extractBody ($body);
    // Post the note.
    $session_logic = Session_Logic::getInstance ();
    $sessionuser = $session_logic->currentUser ();
    $session_logic->setUsername ($username);
    $database_notes = Database_Notes::getInstance();
    $identifier = $database_notes->storeNewNote ("", $book, $chapter, $verse, $summary, $body, false);
    $this->handlerNewNote ($identifier);
    $session_logic->setUsername ($sessionuser);
    // Mail confirmation to the $username.
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getUserNotifyMeOfMyPosts ($username)) {
      $subject = Locale_Translate::_("Your new note was posted");
      $database_mail->send ($username, $subject . ": " . $originalSubject, $body);
    }
    // Log operation.
    $database_logs = Database_Logs::getInstance ();
    $database_logs->log ("New note posted" . ":" . " " . $body);
    // Job done.
    return true;
  }
  
  
  public static function generalBibleName ()
  {
    return "[" . Locale_Translate::_("no Bible") . "]";
  }


}


?>
