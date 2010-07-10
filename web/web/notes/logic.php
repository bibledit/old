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
    $this->notifierNote ($identifier, gettext ("New note"));
  }

  public function handlerAddComment ($identifier)
  {
    $this->notifierNote ($identifier, gettext ("Comment added"));
    // If the note' status was Done, and a comment is added, mark it Reopened.
    $database_notes = Database_Notes::getInstance ();
    $status = $database_notes->getRawStatus ($identifier);
    if ($status == "Done") {
      $database_notes->setStatus ($identifier, "Reopened");
    }
  }

  public function handlerUpdateNote ($identifier)
  {
    //$this->notifierNote ($identifier, gettext ("Note updated"));
  }

  public function handlerDeleteNote ($identifier)
  {
    $this->notifierNote ($identifier, gettext ("Note deleted"));
  }

  /**
  * This handles notifications for relevant receivers
  * $identifier: the note.
  * $label: prefix to the subject line of the email.
  */
  private function notifierNote ($identifier, $label)
  {
    // Databases.
    $database_notes = Database_Notes::getInstance();
    $session_logic = Session_Logic::getInstance ();
    $database_config_user = Database_Config_User::getInstance ();
    $database_users = Database_Users::getInstance();
    $database_mail = Database_Mail::getInstance();
    
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

    // Ensure the list consists of unique recipients, so nobody is mailed twice about an issue.
    $recipients = array_unique ($recipients);

    // Send mail to all recipients.
    $summary = $database_notes->getSummary ($identifier);
    $contents = $database_notes->getContents ($identifier);
    foreach ($recipients as $recipient) {
      $database_mail->send ($recipient, "$label - $summary", $contents);
    }
  }
}  


?>
