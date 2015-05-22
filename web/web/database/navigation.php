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


// Database resilience: It is re-created every night 


class Database_Navigation
{

  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("navigation");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Navigation ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS navigation (
  timestamp integer,
  username text,
  book integer,
  chapter integer,
  verse integer,
  active boolean
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function trim () 
  {
    $query = "DELETE FROM navigation;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function record ($time, $user, $book, $chapter, $verse)
  {
    // Make input safe.
    $time = Database_SQLiteInjection::no ($time);
    $user = Database_SQLiteInjection::no ($user);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);

    // Clear any 'active' flags.
    $query = "UPDATE navigation SET active = 0 WHERE username = '$user';";
    Database_SQLite::exec ($this->db, $query);

    // Remove too recent entries.
    $recent = $time - 5;
    $query = "DELETE FROM navigation WHERE timestamp >= $recent AND username = '$user';";
    Database_SQLite::exec ($this->db, $query);

    // Record entry.
    $query = "INSERT INTO navigation VALUES ($time, '$user', $book, $chapter, $verse, 1);";
    Database_SQLite::exec ($this->db, $query);
  }


  public function previousExists ($user)
  {
    return ($this->getPreviousId ($user) != 0);
  }


  public function nextExists ($user)
  {
    return ($this->getNextId ($user) != 0);
  }


  public function getPrevious ($user)
  {
    $id = $this->getPreviousId ($user);
    if ($id == NULL) return NULL;

    // Update the 'active' flag.
    $query = "UPDATE navigation SET active = 0 WHERE username = '$user';";
    Database_SQLite::exec ($this->db, $query);
    $query = "UPDATE navigation SET active = 1 WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);

    $query = "SELECT book, chapter, verse FROM navigation WHERE rowid = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row ['book']);
      unset ($row ['chapter']);
      unset ($row ['verse']);
      return $row;
    }
    return array ();
  }


  public function getNext ($user)
  {
    $id = $this->getNextId ($user);
    if ($id == NULL) return NULL;

    // Update the 'active' flag.
    $query = "UPDATE navigation SET active = 0 WHERE username = '$user';";
    Database_SQLite::exec ($this->db, $query);
    $query = "UPDATE navigation SET active = 1 WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);

    $query = "SELECT book, chapter, verse FROM navigation WHERE rowid = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row ['book']);
      unset ($row ['chapter']);
      unset ($row ['verse']);
      return $row;
    }
    return array ();
  }


  private function getPreviousId ($user)
  {
    $user = Database_SQLiteInjection::no ($user);

    // Get the ID of the active entry for the user.
    $id = 0;
    $query = "SELECT rowid FROM navigation WHERE username = '$user' AND active = 1;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $id = $row[0];
    }

    // If no active ID was found, return NULL.
    if ($id == 0) return NULL;

    // Get the ID of the entry just before the active entry.
    $query = "SELECT rowid FROM navigation WHERE rowid < $id AND username = '$user' ORDER BY rowid DESC LIMIT 1;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }

    // Nothing found.
    return NULL;
  }


  private function getNextId ($user)
  {
    $user = Database_SQLiteInjection::no ($user);

    // Get the ID of the active entry for the user.
    $id = 0;
    $query = "SELECT rowid FROM navigation WHERE username = '$user' AND active = 1;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $id = $row[0];
    }

    // If no active ID was found, return NULL.
    if ($id == 0) return NULL;

    // Get the ID of the entry just after the active entry.
    $query = "SELECT rowid FROM navigation WHERE rowid > $id AND username = '$user' ORDER BY rowid ASC LIMIT 1;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }

    // Nothing found.
    return NULL;
  }


}


?>
