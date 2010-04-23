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

  public function handlerNewNote ($identifier)  // Todo subscription handler done here, then call mail handler.
  {
    $database_config_user = Database_Config_User::getInstance ();
    if ($database_config_user->getSubscribeToConsultationNotesEditedByMe ()) {
      // User automatically gets subscribed to the note.
      $database_notes = Database_Notes::getInstance();
      $database_notes->subscribe ($identifier);
    }
    $this->notifierNote ($identifier, gettext ("New note"));
  }

  /**
  * This notifies subscribers to changes in note with $identifier.
  * The $label is the prefix to the subject line of the email.
  */
  private function notifierNote ($identifier, $label)
  {
    // Get note data.
    $database_notes = Database_Notes::getInstance();
    $summary = $database_notes->getSummary ($identifier);
    $contents = $database_notes->getContents ($identifier);
    // Get the subscribers to this note.
    $subscribers = $database_notes->getSubscribers ($identifier);
    // Get users who get notified of any change in any note.
    $database_config_user = Database_Config_User::getInstance();
    $database_users = Database_Users::getInstance();
    $users = $database_users->getUsers ();
    // Add these users to the subscribers, ensure each username is unique.
    foreach ($users as $user) {
      if ($database_config_user->getNotifyUserOfAnyConsultationNotesEdits ($user)) {
        $subscribers [] = $user;
      }
    }
    $subscribers = array_unique ($subscribers);
    // Send mail to all receipients.
    $database_mail = Database_Mail::getInstance();
    foreach ($subscribers as $subscriber) {
      $database_mail->send ($subscriber, gettext ("New note") . ": " . $summary, $contents);
    }  
  }
  

/*




*/
// Todo this is too basic, the email also needs a link and other information, such as the passage.
// Todo also needs to take in account the users who have subscribed to all the notes.
// If the user already got an email because he was subscribed, this mail should not be sent for the second time.

}  


?>
