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


// Database resilience. 
// It is written to by a single user.
// No simultaneous writes are are rare.
// In case of corruption, the work done on the consultation notes is lost.
// Remove the database file, and re-run setup to correct the problem.


class Database_NoteActions
{

  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("noteactions");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_NoteActions ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS noteactions (
  username text,
  note integer,
  timestamp integer,
  action integer,
  content text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function clear ()
  {
    Database_SQLite::exec ($this->db, "DROP TABLE IF EXISTS noteactions;");
    $this->create ();
  }
  

  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM noteactions;");
  }


  public function record ($username, $note, $action, $content)
  {
    $username = Database_SQLiteInjection::no ($username);
    $note = Database_SQLiteInjection::no ($note);
    $timestamp = time ();
    $action = Database_SQLiteInjection::no ($action);
    $content = Database_SQLiteInjection::no ($content);
    $query = "INSERT INTO noteactions VALUES ('$username', $note, $timestamp, $action, '$content');";
    Database_SQLite::exec ($this->db, $query);
  }


  public function getNotes ()
  {
    $notes = array ();
    $query = "SELECT DISTINCT note FROM noteactions ORDER BY timestamp;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $notes [] = $row [0];
    }
    return $notes;
  }


  public function getNoteData ($note)
  {
    $note = Database_SQLiteInjection::no ($note);
    $data = array ();
    $query = "SELECT ROWID, username, timestamp, action, content FROM noteactions WHERE note = $note ORDER BY timestamp;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row [0]);
      unset ($row [1]);
      unset ($row [2]);
      unset ($row [3]);
      unset ($row [4]);
      $data [] = $row;
    }
    return $data;
  }


  // Update all actions for a note with identifier $old to $new.
  public function updateNotes ($old, $new)
  {
    $old = Database_SQLiteInjection::no ($old);
    $new = Database_SQLiteInjection::no ($new);
    $query = "UPDATE noteactions SET note = $new WHERE note = $old;";
    Database_SQLite::exec ($this->db, $query);
  }
  

  public function delete ($rowid)
  {
    $rowid = Database_SQLiteInjection::no ($rowid);
    $query = "DELETE FROM noteactions where rowid = $rowid;";
    Database_SQLite::exec ($this->db, $query);
  }
   

}



?>
