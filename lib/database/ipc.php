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


// Database resilience: Stored in plain file system.


class Database_Ipc
{
  private static $instance;
  private $db;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Ipc();
    }
    return self::$instance;
  }

  
  private function folder ()
  {
    $folder = realpath (__DIR__ . "/../databases/ipc");
    return $folder;
  }
  
  
  private function file ($file)
  {
    $file = $this->folder () . "/$file";
    return $file;
  }


  // Reads most fields of all data in this database.
  // Returns an array containing the above.
  // The fields are those that can be obtained from the name of the files.
  // One file is one entry in the database.
  // The filename looks like this: rowid__user__channel__command
  private function readData ()
  {
    $folder = $this->folder ();
    $files = scandir ($folder);
    $files = Filter_Folders::cleanup ($files);
    $data = array ();
    foreach ($files as $file) {
      $explosion = explode ("__", $file);
      $rowid = $explosion [0];
      $user = $explosion [1];
      $channel = $explosion [2];
      $command = $explosion [3];
      $fields = array ('file' => $file, 'rowid' => $rowid, 'user' => $user, 'channel' => $channel, 'command' => $command);
      $data [] = $fields;
    }
    return $data;
  }
    
  
  private function writeRecord ($rowid, $user, $channel, $command, $message)
  {
    $file = $rowid . '__' . $user . '__' . $channel . '__' . $command;
    $file = $this->file ($file);
    file_put_contents ($file, $message);
  }
  

  // Returns the next available row identifier.
  private function getNextId ($data)
  {
    $id = 0;
    foreach ($data as $record) {
      $rowid = $record ['rowid'];
      if ($rowid > $id) $id = $rowid;
    }
    $id++;
    return $id;
  }
  
  
  public function trim ()
  {
    $data = $this->readData ();
    foreach ($data as $record) {
      if ($record ['user'] == "") {
        $this->deleteMessage ($record ['rowid']);
      }
    }
  }


  public function storeMessage ($user, $channel, $command, $message)
  {
    // Load entire database into memory.
    $data = $this->readData ();

    // Gather information about records to delete.
    $deletes = array ();
    if ($channel == "") {
      foreach ($data as $record) {
        if (($record ['user'] == $user) && ($record ['channel'] == $channel) && ($record ['command'] == $command)) {
          $deletes [] = $record ['rowid'];
        }
      }
    }
    
    // Write new information.
    $rowid = $this->getNextId ($data);
    $this->writeRecord ($rowid, $user, $channel, $command, $message);
    
    // Actually delete the records.
    // Do this after writing new data, to be sure there is always some data on disk.
    foreach ($deletes as $delete) {
      $this->deleteMessage ($delete);
    }
  }


  /**
  * Retrieves a message if there is any.
  * Returns NULL if there was nothing,
  * else an associated array with keys and values for "id", "channel", "command" and "message".
  */
  public function retrieveMessage ($id, $user, $channel, $command)
  {
    $highestId = 0;
    $hitChannel = "";
    $hitCommand = "";
    $hitMessage = "";
    $data = $this->readData ();
    foreach ($data as $record) {
      // Selection condition 1: The database record has a message identifier younger than the calling identifier.
      $recordid = $record ['rowid'];
      if ($recordid > $id) {
        // Selection condition 2: Channel matches calling channel, or empty channel.
        $recordchannel = $record ['channel'];
        if (($recordchannel == $channel) || ($recordchannel == '')) {
          // Selection condition 3: Record user matches calling user, or empty user.
          $recorduser = $record ['user'];
          if (($recorduser == $user) || ($recorduser == '')) {
            // Selection condition 4: Matching command.
            $recordcommand = $record ['command'];
            if ($recordcommand == $command) {
              if ($recordid > $highestId) {
                $highestId = $recordid;
                $hitChannel = $recordchannel;
                $hitCommand = $recordcommand;
                $hitMessage = file_get_contents ($this->file ($record ['file']));
              }
            }
          }
        }
      }
    }
    if ($highestId) {
      return array ('id' => $highestId, "channel" => $hitChannel, "command" => $hitCommand, "message" => $hitMessage);
    }
    return NULL;
  }


  public function deleteMessage ($id)
  {
    $data = $this->readData ();
    foreach ($data as $record) {
      if ($record ['rowid'] == $id) {
        $file = $this->file ($record ['file']);
        @unlink ($file);
      }
    }
  }


  public function getFocus ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();

    $highestId = 0;
    $hitMessage = "";
    $data = $this->readData ();
    foreach ($data as $record) {
      $recordid = $record ['rowid'];
      // Conditions: Command is "focus", and matching user.
      if ($record ['command'] == "focus") {
        if ($record ['user'] == $user) {
          if ($recordid > $highestId) {
            $highestId = $recordid;
            $hitMessage = file_get_contents ($this->file ($record ['file']));
          }
        }
      }
    }

    if ($highestId) {
      return $hitMessage;
    }

    // No focus found: Return Genesis 1:1.
    return "1.1.1";
  }


  public function getNote ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();

    $highestId = 0;
    $hitMessage = "";

    $data = $this->readData ();
    foreach ($data as $record) {
      $recordid = $record ['rowid'];
      // Conditions: Command is "opennote", and matching user.
      if ($record ['command'] == "opennote") {
        if ($record ['user'] == $user) {
          if ($recordid > $highestId) {
            $highestId = $recordid;
            $hitMessage = file_get_contents ($this->file ($record ['file']));
          }
        }
      }
    }

    if ($highestId) {
      return array ('id' => $highestId, 'message' => $hitMessage);
    }

    return NULL;
  }


  public function getNotesAlive ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();

    $highestId = 0;
    $hitMessage = "";

    $data = $this->readData ();
    foreach ($data as $record) {
      $recordid = $record ['rowid'];
      // Conditions: Command is "notesalive", and matching user.
      if ($record ['command'] == "notesalive") {
        if ($record ['user'] == $user) {
          if ($recordid > $highestId) {
            $highestId = $recordid;
            $hitMessage = file_get_contents ($this->file ($record ['file']));
          }
        }
      }
    }

    if ($highestId) {
      return $hitMessage;
    }

    return false;
  }


  public function getBibleAlive ()
  {
    $session_logic = Session_Logic::getInstance ();
    $user = $session_logic->currentUser ();

    $highestId = 0;
    $hitMessage = "";

    $data = $this->readData ();
    foreach ($data as $record) {
      $recordid = $record ['rowid'];
      if ($record ['command'] == "biblealive") {
        if ($record ['user'] == $user) {
          if ($recordid > $highestId) {
            $highestId = $recordid;
            $hitMessage = file_get_contents ($this->file ($record ['file']));
          }
        }
      }
    }

    if ($highestId) {
      return $hitMessage;
    }

    return false;
  }


  public function debug ()
  {
    $data = $this->readData ();
    var_dump ($data);
  }


}


?>
