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
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Notes();
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
status varchar (256),     # Note status: New / Need info / Confirmed / Invalid / In progress / Done
severity int,             # Severity of note: Wish / Minor / Normal / Important / Major / Critical
private bool,             # Whether note is private.
summary varchar (256),    # Note summary.
contents text             # Note contents.
);
EOD;
    $database_instance->runQuery ($query);
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
    $datetime = new DateTime();
    Filter_Datetime::user_zone ($datetime);
    $datetime = $datetime->format(DATE_RSS);
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
    $passage = Database_SQLInjection::no ($this->getPassageMarkup ($book, $chapter, $verse));
    // If the $summary is not given, take the first line of the $contents as the $summary.
    if ($summary == "") {
      $summary = explode ("\n", $contents);
      $summary = $summary[0];
    }
    $summary = Database_SQLInjection::no ($summary);
    $contents = $this->assembleContents ($identifier, $contents);
    $contents = Database_SQLInjection::no ($contents);
    if (($contents == "") && ($summary == "")) return;
    $query = "INSERT INTO notes VALUES (NULL, $identifier, $modified, '', '$bible', '$passage', 'New', 'Normal', 0, '$summary', '$contents')";
    $server->runQuery ($query);
    // Return this new note´s identifier.
    return $identifier;
  }


  public function getPassageMarkup ($book, $chapter, $verse)
  {
    $markup = " $book.$chapter.$verse ";
    return $markup;
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


  
  

}



?>
