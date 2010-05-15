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

  public function handlerDeleteNote ($identifier)
  {
    $this->notifierNote ($identifier, gettext ("Note deleted"));
  }

  /**
  * This handles notifications for relevant receivers
  * $identifier: the note.
  * $label: prefix to the subject line of the email.
  */
  private function notifierNote ($identifier, $label) // Todo
  {
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getSubscribeToConsultationNotesEditedByMe ()) {
      // User automatically gets subscribed to the note.
      $database_notes = Database_Notes::getInstance();
      $database_notes->subscribe ($identifier);
    }
    // Get note data.
    $database_notes = Database_Notes::getInstance();
    $summary = $database_notes->getSummary ($identifier);
    $contents = $database_notes->getContents ($identifier);
    // Get the subscribers to this note.
    $subscribers = $database_notes->getSubscribers ($identifier);

    // Handle notifications for all users.
    $database_users = Database_Users::getInstance();
    $users = $database_users->getUsers ();
    foreach ($users as $user) {
      // Users who get notified by email of any change in any note.
      if ($database_config_user->getNotifyUserOfAnyConsultationNotesEdits ($user)) {
        $subscribers [] = $user;
      }
      // Users to whom the note gets assigned on change.
      if ($database_config_user->getUserAssignedToConsultationNotesChanges ($user)) {
        $database_notes->assignUser ($identifier, $user);
      }
    }

    // If assignees wish to receive email about changes, then include them too.
    $assignees = $database_notes->getAssignees ($identifier);
    foreach ($assignees as $assignee) {
      if ($database_config_user->getUserAssignedConsultationNoteNotification ($assignee)) {
        $subscribers [] = $assignee;
      }
    }

    // Ensure the list consists of unique subscribers, so no subscriber is mailed twice about an issue.
    $subscribers = array_unique ($subscribers);
    // Send mail to all receipients.
    $database_mail = Database_Mail::getInstance();
    foreach ($subscribers as $subscriber) {
      $database_mail->send ($subscriber, "$label - $summary", $contents);
    }  
  }
}  


?>
