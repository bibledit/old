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


// Database robustness:
// 1. It is infrequently written to.
// 2. Chances of corruption are low.
// 3. In case of corruption, data for the sprints is lost. 
//    This is acceptable, because translation work can still continue.


class Database_Sprint
{

  private static $instance;
  public $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("sprint");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Sprint ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS sprint (
  year integer,
  month integer,
  title text,
  body text,
  complete integer
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS yearmonth ON sprint (year, month)";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS complete ON sprint (complete)";
    Database_SQLite::exec ($this->db, $sql);

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS sprinthistory (
  year integer,
  month integer,
  day integer,
  tasks integer,
  complete int
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    // Upgrade the two tables: Add a column for the Bible.
    $columns = array ();
    $query = "PRAGMA table_info (sprint);";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $columns [] = $row ['name'];
    }
    if (!in_array ("bible", $columns)) {
      $sql = "ALTER TABLE sprint ADD COLUMN bible text;";
      Database_SQLite::exec ($this->db, $sql);
    }
    $columns = array ();
    $query = "PRAGMA table_info (sprinthistory);";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $columns [] = $row ['name'];
    }
    if (!in_array ("bible", $columns)) {
      $sql = "ALTER TABLE sprinthistory ADD COLUMN bible text;";
      Database_SQLite::exec ($this->db, $sql);
    }
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "REINDEX sprint;");
    Database_SQLite::exec ($this->db, "VACUUM sprint;");
    Database_SQLite::exec ($this->db, "VACUUM sprinthistory;");
  }


  public function storeTask ($bible, $year, $month, $title)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $year = Database_SQLiteInjection::no ($year);
    $month = Database_SQLiteInjection::no ($month);
    $title = Database_SQLiteInjection::no ($title);
    $query = "INSERT INTO sprint VALUES ($year, $month, '$title', '', 0, '$bible');";
    Database_SQLite::exec ($this->db, $query);
  }


  public function deleteTask ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "DELETE FROM sprint WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function getTasks ($bible, $year, $month)
  {
    $ids = array ();
    $bible = Database_SQLiteInjection::no ($bible);
    $year = Database_SQLiteInjection::no ($year);
    $month = Database_SQLiteInjection::no ($month);
    $query = "SELECT rowid FROM sprint WHERE bible = '$bible' AND year = $year AND month = $month ORDER BY rowid ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $ids [] = $row [0];
    }
    return $ids;
  }


  public function getTitle ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT title FROM sprint WHERE rowid = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  public function updateComplete ($id, $percentage)
  {
    $id = Database_SQLiteInjection::no ($id);
    $percentage = Database_SQLiteInjection::no ($percentage);
    $query = "UPDATE sprint SET complete = $percentage WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function getComplete ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT complete FROM sprint WHERE rowid = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  public function updateMonthYear ($id, $month, $year)
  {
    $id = Database_SQLiteInjection::no ($id);
    $month = Database_SQLiteInjection::no ($month);
    $year = Database_SQLiteInjection::no ($year);
    $query = "UPDATE sprint SET month = $month, year = $year WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  public function logHistory ($bible, $year, $month, $day, $tasks, $complete)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $year = Database_SQLiteInjection::no ($year);
    $month = Database_SQLiteInjection::no ($month);
    $day = Database_SQLiteInjection::no ($day);
    $tasks = Database_SQLiteInjection::no ($tasks);
    $complete = Database_SQLiteInjection::no ($complete);
    $query = "DELETE FROM sprinthistory WHERE bible = '$bible' AND year = $year AND month = $month AND day = $day;";
    Database_SQLite::exec ($this->db, $query);
    $query = "INSERT INTO sprinthistory VALUES ($year, $month, $day, $tasks, $complete, '$bible');";
    Database_SQLite::exec ($this->db, $query);
  }


  public function getHistory ($bible, $year, $month)
  {
    $history = array ();
    $bible = Database_SQLiteInjection::no ($bible);
    $year = Database_SQLiteInjection::no ($year);
    $month = Database_SQLiteInjection::no ($month);
    $query = "SELECT day, tasks, complete FROM sprinthistory WHERE bible = '$bible' AND year = $year AND month = $month ORDER BY day ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      for ($i = 0; $i < 3; $i++) unset ($row[$i]);
      $history [] = $row;
    }
    return $history;
  }


  public function clearHistory ($bible, $year, $month)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $year = Database_SQLiteInjection::no ($year);
    $month = Database_SQLiteInjection::no ($month);
    $query = "DELETE FROM sprinthistory WHERE bible = '$bible' AND year = $year AND month = $month;";
    Database_SQLite::exec ($this->db, $query);
  }


}


?>
