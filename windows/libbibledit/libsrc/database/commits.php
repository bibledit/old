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


// Database resilience. It is only written to during the night.
// In case of corruption, some information is lost.
// Remove the database file, and re-run setup to correct the problem.


class Database_Commits
{

  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("commits");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Commits ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS commits (
  bible text,
  sha1 text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS bible ON commits (bible)";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS sha1 ON commits (sha1)";
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "REINDEX commits;");
    Database_SQLite::exec ($this->db, "VACUUM commits;");
  }


  public function record ($bible, $sha1)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $sha1 = Database_SQLiteInjection::no ($sha1);
    $query = "INSERT INTO commits VALUES ('$bible', '$sha1');";
    Database_SQLite::exec ($this->db, $query);
  }


  public function get ($bible)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $commits = array ();
    $query = "SELECT sha1 FROM commits WHERE bible = '$bible';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $commits [] = $row [0];
    }
    return $commits;
  }


}



?>
