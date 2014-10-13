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


// Database resilience: It is written to during the night.
// If corruption occurs, the history is lost.


class Database_History
{

  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("history");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_History ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS history (
  timestamp integer,
  author text,
  bible text,
  book integer,
  chapter integer,
  verse integer,
  oldtext text,
  modification text,
  newtext text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS author ON history (author)";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS timestamp ON history (timestamp)";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS bible ON history (bible)";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS passage ON history (book, chapter, verse)";
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "REINDEX history;");
    Database_SQLite::exec ($this->db, "VACUUM history;");
  }


  public function trim ()
  {
    // Delete older items.
    $time = strtotime ("-6 months");
    //$query = "DELETE FROM history WHERE timestamp < $time;";
    //Database_SQLite::exec ($this->db, $query);
  }


  public function record ($timestamp, $author, $bible, $book, $chapter, $verse, $oldtext, $modification, $newtext)
  {
    $timestamp = Database_SQLiteInjection::no ($timestamp);
    $author = Database_SQLiteInjection::no ($author);
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $oldtext = Database_SQLiteInjection::no ($oldtext);
    $modification = Database_SQLiteInjection::no ($modification);
    $newtext = Database_SQLiteInjection::no ($newtext);
    $query = "INSERT INTO history VALUES ($timestamp, '$author', '$bible', $book, $chapter, $verse, '$oldtext', '$modification', '$newtext');";
    Database_SQLite::exec ($this->db, $query);
  }


  public function count ($author, $bibles, $book, $chapter, $verse)
  {
    $query = $this->query ($author, $bibles, $book, $chapter, $verse, true);
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return 0;
  }


  public function get ($author, $bible, $book, $chapter, $verse, $start)
  {
    $data = array ();
    $query = $this->query ($author, $bible, $book, $chapter, $verse, false, $start);
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      for ($i = 0; $i <= 8; $i++) unset ($row[$i]);
      $data [] = $row;
    }
    return $data;
  }


  private function query ($author, $bibles, $book, $chapter, $verse, $count = false, $start = NULL)
  {
    $query = array ();
    $query [] = "SELECT";
    if ($count) $query [] = "COUNT(*)";
    else $query [] = "timestamp, author, bible, book, chapter, verse, oldtext, modification, newtext";
    $query [] = "FROM history WHERE 1";
    if ($author != "") {
      $author = Database_SQLiteInjection::no ($author);
      $query [] = "AND author = '$author'";
    }
    if (!empty ($bibles)) {
      $query [] = "AND";
      $query [] = "(";
      foreach ($bibles as $offset => $bible) {
        if ($offset) $query [] = "OR";
        $bible = Database_SQLiteInjection::no ($bible);
        $query [] = "bible = '$bible'";
      }
      $query [] = ")";
    }
    if (is_numeric ($book)) {
      $book = Database_SQLiteInjection::no ($book);
      $query [] = "AND book = $book";
    }
    if (is_numeric ($chapter)) {
      $chapter = Database_SQLiteInjection::no ($chapter);
      $query [] = "AND chapter = $chapter";
    }
    if (is_numeric ($verse)) {
      $verse = Database_SQLiteInjection::no ($verse);
      $query [] = "AND verse = $verse";
    }
    if (!$count) $query [] = "ORDER BY timestamp DESC";
    if (is_numeric ($start)) {
      if ($start < 0) $start = 0;
      $start = Database_SQLiteInjection::no ($start);
      $query [] = "LIMIT $start, 50";
    }
    $query [] = ";";
    $query = implode (" ", $query);
    return $query;
  }


  // This function returns an array of authors in the history database.
  // The search is limited to the Bibles in array $bibles.
  // The authors are sorted by the number of modifications made by them.
  public function authors ($bibles)
  {
    $authors = array ();
    $bibleCondition = " WHERE ";
    foreach ($bibles as $offset => $bible) {
      if ($offset) $bibleCondition .= " OR ";
      $bible = Database_SQLiteInjection::no ($bible);
      $bibleCondition .= " bible = '$bible' ";
    }
    $query = "SELECT author, COUNT(*) as count FROM history $bibleCondition GROUP BY author ORDER BY count DESC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $authors [] = $row [0];
    }
    return $authors;
  }


}


?>
