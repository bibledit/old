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
// Write operations to table "suppress" are very infrequent.
// Write operations to table "output" happen every night.
// In case of database corruption, running the Bibledit-Web setup re-creates
// this table. The table does not contain important data.
// In cases of extreme corruption, the database file should be manually removed
// before running setup.


class Database_Check
{

  private static $instance;
  public $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("check");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Check ();
    }
    return self::$instance;
  }


  public function create ()
  {
    Database_SQLite::exec ($this->db, "DROP TABLE IF EXISTS output;");

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS output (
  bible integer,
  book integer,
  chapter integer,
  verse integer,
  data text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS suppress (
  bible integer,
  book integer,
  chapter integer,
  verse integer,
  data text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM output;");
    Database_SQLite::exec ($this->db, "VACUUM suppress;");
  }


  public function truncateOutput ($bible)
  {
    if ($bible == "") {
      Database_SQLite::exec ($this->db, "DELETE FROM output;");
    } else {
      $database_bibles = Database_Bibles::getInstance ();
      $bible = $database_bibles->getID ($bible);
      Database_SQLite::exec ($this->db, "DELETE FROM output WHERE bible = $bible;");
    }
  }


  public function recordOutput ($bible, $book, $chapter, $verse, $data)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $bible = $database_bibles->getID ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    if ($verse == "") $verse = 0;
    $verse = Database_SQLiteInjection::no ($verse);
    $data = Database_SQLiteInjection::no ($data);
    $query = "INSERT INTO output VALUES ($bible, $book, $chapter, $verse, '$data');";
    Database_SQLite::exec ($this->db, $query);
  }


  public function getHits ()
  {
    $hits = array ();
    $query = "SELECT rowid, bible, book, chapter, verse, data FROM output WHERE NOT EXISTS (SELECT * FROM suppress WHERE output.bible = suppress.bible AND output.book = suppress.book AND output.chapter = suppress.chapter AND output.verse = suppress.verse AND output.data = suppress.data);";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      for ($i = 0; $i <= 5; $i++) unset ($row[$i]);
      $hits [] = $row;
    }
    return $hits;
  }


  public function approve ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    // The query copies all values, apart from the auto_increment id.
    $query = "INSERT INTO suppress (bible, book, chapter, verse, data) SELECT bible, book, chapter, verse, data FROM output WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function delete ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "DELETE FROM output WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function getPassage ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT book, chapter, verse FROM output WHERE rowid = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      for ($i = 0; $i < 3; $i++) unset ($row[$i]);
      return $row;
    }
    return NULL;
  }


  public function getSuppressions ()
  {
    $suppressions = array ();
    $query = "SELECT rowid, bible, book, chapter, verse, data FROM suppress;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      for ($i = 0; $i <= 5; $i++) unset ($row[$i]);
      $suppressions [] = $row;
    }
    return $suppressions;
  }


  public function release ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "DELETE FROM suppress WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


}


?>
