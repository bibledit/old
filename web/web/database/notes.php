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


class Database_Notes
{
  private static $instance;
  private $standard_statuses = array ("New", "Pending", "In progress", "Done", "Reopened");
  private $standard_severities = array (0 => "Wish", 1 => "Minor", 2 => "Normal", 3 => "Important", 4 => "Major", 5 => "Critical");
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Notes();
    }
    // Enter the standard statuses in the list of translatable strings.
    if (false) {
      gettext ("New");
      gettext ("Pending");
      gettext ("In progress");
      gettext ("Done");
      gettext ("Reopened");
    }
    // Enter the standard severities in the list of translatable strings.
    if (false) {
      gettext ("Wish");
      gettext ("Minor");
      gettext ("Normal");
      gettext ("Important");
      gettext ("Major");
      gettext ("Critical");
    }
    return self::$instance;
  }

  public function verify () {
    $database_instance = Database_Instance::getInstance();
$query = <<<EOD
CREATE TABLE IF NOT EXISTS notes (
id int auto_increment primary key,
identifier int NOT NULL,  # Note identifier.
modified int NOT NULL,    # Date modified.
assigned text,            # Note assigned to: lists users.
bible varchar (256),      # The Bible, if any, the note refers to.
passage text,             # The passage or verse the note refers to.
status varchar (256),     # Note status: New / Pending / In progress / Done / etc.
severity int,             # Severity of note: Wish / Minor / Normal / Important / Major / Critical
private tinyint,          # Note privacy: (Public (0) / Consultants (1) / Translators (2) / Me only (3).
summary varchar (256),    # Note summary.
contents text             # Note contents.
);
EOD;
    $database_instance->runQuery ($query);
    // Table update. Subscriptions: Contains users subscribed to this note.
    $database_instance->runQuery ("ALTER TABLE notes ADD subscriptions text AFTER assigned;");
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE notes;");
  }


  public function identifierExists ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT identifier FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    return ($result->num_rows > 0);
  }
  
  
  public function getNewUniqueIdentifier ()
  {
    do {
      $identifier = rand (100000000, 999999999);
    } while ($this->identifierExists ($identifier));
    return $identifier;
  }


  private function assembleContents ($identifier, $contents)
  {
    $new_contents = "";
    if (is_numeric ($identifier)) {
      $new_contents = $this->getContents ($identifier);
    }
    $session_logic = Session_Logic::getInstance();
    if (version_compare(PHP_VERSION, '5.2.0', '>=')) {
      $datetime = new DateTime();
      Filter_Datetime::user_zone ($datetime);
      $datetime = $datetime->format(DATE_RSS);
    } else {
      $datetime = strftime ("%a, %e %b %G %H:%M:%S %z");
    }
    $user = $session_logic->currentUser ();
    $new_contents .= "<p>$user ($datetime):</p>\n";
    $lines = explode ("\n", $contents);
    foreach ($lines as $line) {
      $line = trim ($line);
      $line = Filter_Html::sanitize ($line);
      $new_contents .= "<p>$line</p>\n";
    }
    return $new_contents;
  }

  
  public function storeNewNote ($bible, $book, $chapter, $verse, $summary, $contents)
  {
    // Store new default note into the database.
    $server = Database_Instance::getInstance ();
    $identifier = $this->getNewUniqueIdentifier ();
    $modified = time();
    $bible = Database_SQLInjection::no ($bible);
    $passage = Database_SQLInjection::no ($this->encodePassage ($book, $chapter, $verse));
    // If the $summary is not given, take the first line of the $contents as the $summary.
    if ($summary == "") {
      $summary = explode ("\n", $contents);
      $summary = $summary[0];
    }
    $summary = Database_SQLInjection::no ($summary);
    $contents = $this->assembleContents ($identifier, $contents);
    $contents = Database_SQLInjection::no ($contents);
    if (($contents == "") && ($summary == "")) return;
    $query = "INSERT INTO notes VALUES (NULL, $identifier, $modified, '', '', '$bible', '$passage', 'New', 2, 0, '$summary', '$contents')";
    $server->runQuery ($query);
    // Return this new note´s identifier.
    return $identifier;
  }


  /**
  * Returns an array of note identifiers selected.
  */
  public function selectNotes ()
  {
    $identifiers = array ();
    $server = Database_Instance::getInstance ();
    $query = "SELECT identifier FROM notes;";
    $result = $server->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $identifier = $row[0];
      $identifiers []= $identifier;
    }
    return $identifiers;
  }
  
  
  public function getSummary ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT summary FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return $row[0];
    }
    return "";
  }
  

  public function getContents ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT contents FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return $row[0];
    }
    return "";
  }
  
  
  public function delete ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "DELETE FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
  }

  /**
  * Add a $comment to an exiting note identified by $identifier.
  */
  public function addComment ($identifier, $comment)
  {
    $identifier = Database_SQLInjection::no ($identifier);
    if ($comment == "") return;

    $server = Database_Instance::getInstance ();
    $session_logic = Session_Logic::getInstance();

    $modified = time();
    $query = "UPDATE notes SET modified = $modified WHERE identifier = $identifier;";
    $server->runQuery ($query);

    $contents = $this->assembleContents ($identifier, $comment);
    $contents = Database_SQLInjection::no ($contents);
    $query = "UPDATE notes SET contents = '$contents' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }
  
  
  /**
  * Subscribe the current user to the note identified by $identifier.
  */
  public function subscribe ($identifier)
  {
    $session_logic = Session_Logic::getInstance();
    $user = $session_logic->currentUser ();
    $this->subscribeUser ($identifier, $user);
  }


  /**
  * Subscribe the $user to the note identified by $identifier.
  */
  public function subscribeUser ($identifier, $user)
  {
    // If the user already is subscribed to the note, bail out.
    $subscribers = $this->getSubscribers ($identifier);
    if (in_array ($user, $subscribers)) return;
    // Subscribe $user.
    $subscribers[]= "$user";
    $subscribers = implode ("\n", $subscribers);
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $subscribers = Database_SQLInjection::no ($subscribers);
    $query = "UPDATE notes SET subscriptions = '$subscribers' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }


  /**
  * Returns an array with the subscribers to the note identified by $identifier.
  */  
  public function getSubscribers ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT subscriptions FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    $row = $result->fetch_row();
    $subscribers = explode ("\n", $row[0]);
    $subscribers = array_diff ($subscribers, array (""));
    return $subscribers;
  }


  /**
  * Returns true if $user is subscribed to the note identified by $identifier.
  */
  public function isSubscribed ($identifier, $user)
  {
    $subscribers = $this->getSubscribers ($identifier);
    return in_array ($user, $subscribers);
  }

  
  /**
  * Unsubscribes the currently logged in user from the note identified by $identifier.
  */
  public function unsubscribe ($identifier)
  {
    $session_logic = Session_Logic::getInstance();
    $user = $session_logic->currentUser ();
    $this->unsubscribeUser ($identifier, $user);
  }


  /**
  * Unsubscribes $user from the note identified by $identifier.
  */
  public function unsubscribeUser ($identifier, $user)
  {
    // If the user is not subscribed to the note, bail out.
    $subscribers = $this->getSubscribers ($identifier);
    if (!in_array ($user, $subscribers)) return;
    // Unsubscribe $user.
    $subscribers = array_diff ($subscribers, array ($user));
    $subscribers = implode ("\n", $subscribers);
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $subscribers = Database_SQLInjection::no ($subscribers);
    $query = "UPDATE notes SET subscriptions = '$subscribers' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }


  /**
  * Assign the note identified by $identifier to $user.
  */
  public function assignUser ($identifier, $user)
  {
    // If the note already is assigned to the user, bail out.
    $assignees = $this->getAssignees ($identifier);
    if (in_array ($user, $assignees)) return;
    // Assign the note to the user.
    $assignees[]= "$user";
    $assignees = implode ("\n", $assignees);
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $assignees = Database_SQLInjection::no ($assignees);
    $query = "UPDATE notes SET assigned = '$assignees' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }


  /**
  * Returns an array with the assignees to the note identified by $identifier.
  */  
  public function getAssignees ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT assigned FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    $row = $result->fetch_row();
    $assignees = explode ("\n", $row[0]);
    $assignees = array_diff ($assignees, array (""));
    return $assignees;
  }


  /**
  * Returns true if the note identified by $identifier has been assigned to $user.
  */
  public function isAssigned ($identifier, $user)
  {
    $assignees = $this->getAssignees ($identifier);
    return in_array ($user, $assignees);
  }

  
  /**
  * Unassignes the currently logged in user from the note identified by $identifier.
  */
  public function unassign ($identifier)
  {
    $session_logic = Session_Logic::getInstance();
    $user = $session_logic->currentUser ();
    $this->unassignUser ($identifier, $user);
  }


  /**
  * Unassigns $user from the note identified by $identifier.
  */
  public function unassignUser ($identifier, $user)
  {
    // If the notes is not assigned to the user, bail out.
    $assignees = $this->getAssignees ($identifier);
    if (!in_array ($user, $assignees)) return;
    // Remove assigned $user.
    $assignees = array_diff ($assignees, array ($user));
    $assignees = implode ("\n", $assignees);
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $assignees = Database_SQLInjection::no ($assignees);
    $query = "UPDATE notes SET assigned = '$assignees' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }

  

  public function getBible ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT bible FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return $row[0];
    }
    return "";
  }


  public function setBible ($identifier, $bible)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $bible = Database_SQLInjection::no ($bible);
    $query = "UPDATE notes SET bible = '$bible' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }



  /**
  * Encodes the book, chapter and verse, like to, e.g.: "40.5.13",
  * and returns this as a string.
  */  
  private function encodePassage ($book, $chapter, $verse)
  {
    return "$book.$chapter.$verse";
  }
  
  
  /**
  * Takes the $passage as a string, and returns an array with book, chapter, and verse.
  */
  private function decodePassage ($passage)
  {
    return explode (".", $passage);
  }


  /**
  * Returns an array with the passages that the note identified by $identifier refers to.
  * Each passages is an array (book, chapter, verse).
  */  
  public function getPassages ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT passage FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    $row = $result->fetch_row();
    $lines = explode ("\n", $row[0]);
    $lines = array_diff ($lines, array (""));
    $passages = array ();
    foreach ($lines as $line) {
      $passage = $this->decodePassage ($line);
      $passages[] = $passage;
    }
    return $passages;
  }


  /**
  * Assign the passages to the note $identifier.
  * $passages is an array of an array (book, chapter, verse) passages.
  */
  public function setPassages ($identifier, $passages)
  {
    $server = Database_Instance::getInstance ();
    foreach ($passages as $passage) {
      $line .= $this->encodePassage ($passage[0], $passage[1], $passage[2]);
      $line .= "\n";
    }
    $identifier = Database_SQLInjection::no ($identifier);
    $passages = Database_SQLInjection::no ($passages);
    $query = "UPDATE notes SET passage = '$line' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }


  /**
  * Add the passage of $book, $chapter, $verse to the note identified by $identifier.
  */
  public function addPassage ($identifier, $book, $chapter, $verse)
  {
    // If the passage is already in, bail out.
    $passage = array ($book, $chapter, $verse);
    $passages = $this->getPassages ($identifier);
    if (in_array ($passage, $passages)) return;
    // Add the passage to the note.
    $passages[]= $passage;
    $this->setPassages ($identifier, $passages);
  }


  /**
  * Removes $passage from the note identified by $identifier.
  */
  public function removePassage ($identifier, $book, $chapter, $verse)
  {
    // A special method is used to remove the passage, because array_diff 
    // does not operate properly on multi-dimensional arrays.
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT passage FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    $row = $result->fetch_row();
    $lines = explode ("\n", $row[0]);
    $lines = array_diff ($lines, array (""));
    $lines = array_diff ($lines, array ($this->encodePassage ($book, $chapter, $verse)));
    $passages = array ();
    foreach ($lines as $line) {
      $passage = $this->decodePassage ($line);
      $passages[] = $passage;
    }
    $this->setPassages ($identifier, $passages);
  }


  /**
  * Returns true if the $passage is contained in the note identified by $identifier.
  */
  public function passageContained ($identifier, $book, $chapter, $verse)
  {
    $passages = $this->getPassages ($identifier);
    $passage = array ($book, $chapter, $verse);
    return in_array ($passage, $passages);
  }



  /**
  * Gets the raw status of a note.
  * Returns it as a string.
  */
  public function getRawStatus ($identifier)
  {
    // Get status.
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT status FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    $status = "";
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $status = $row[0];
    }
    return $status;
  }


  /**
  * Gets the localized status of a note.
  * Returns it as a string.
  */
  public function getStatus ($identifier)
  {
     $status = $this->getRawStatus ($identifier);
    // Localize status if it is a standard one.
    if (in_array ($status, $this->standard_statuses)) $status = gettext ($status);
    // Return status.
    return $status;
  }


  /**
  * Sets the $status of the note identified by $identifier.
  * $status is a string.
  */
  public function setStatus ($identifier, $status)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $status = Database_SQLInjection::no ($status);
    $query = "UPDATE notes SET status = '$status' WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }


  /**
  * Gets an array with the possible statuses of consultation notes.
  */
  public function getPossibleStatuses ()
  {
    // Get an array with the statuses used in the database, ordered by occurrence, most often used ones first.
    $statuses = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT status, COUNT(status) AS occurrences FROM notes GROUP BY status ORDER BY occurrences DESC;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i <$result->num_rows ; $i++) {
      $row = $result->fetch_row ();
      $statuses[] = $row[0];
    }
    // Ensure the standard statuses are there too.
    foreach ($this->standard_statuses as $standard_status) {
      if (!in_array ($standard_status, $statuses)) {
        $statuses[] = $standard_status;
      }
    }
    // Localize the results.
    foreach ($statuses as $status) {
      $localization = $status;
      if (in_array ($status, $this->standard_statuses)) $localization = gettext ($status);
      $localized_status = array ($status, $localization);
      $localized_statuses [] = $localized_status;
    }
    // Return result.
    return $localized_statuses;
  }
  

  /**
  * Returns the severity of a note as a localized string.
  */
  public function getSeverity ($identifier)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $query = "SELECT severity FROM notes WHERE identifier = $identifier;";
    $result = $server->runQuery ($query);
    $severity = 2;
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $severity = $row[0];
    }
    $severity = $this->standard_severities[$severity];
    if ($severity == "") $severity = "Normal";
    $severity = gettext ($severity);
    return $severity;
  }


  /**
  * Sets the $severity of the note identified by $identifier.
  * $severity is a  number.
  */
  public function setSeverity ($identifier, $severity)
  {
    $server = Database_Instance::getInstance ();
    $identifier = Database_SQLInjection::no ($identifier);
    $severity = Database_SQLInjection::no ($severity);
    $query = "UPDATE notes SET severity = $severity WHERE identifier = $identifier;";
    $server->runQuery ($query);
  }


  /**
  * Gets an array with the possible severities.
  */
  public function getPossibleSeverities ()
  {
    for ($i = 0; $i <= 5; $i++) {
      $severities[] = array ($i, gettext ($this->standard_severities[$i]));
    }
    return $severities;
  }
  




 
 

}



?>
