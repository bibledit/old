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


class Database_BibleActions
{


  private static $instance;
  private $db;
  private function __construct () {
    $this->db = Database_SQLite::connect ("bibleactions");
  }
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_BibleActions ();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS bibleactions (
  bible text,
  book integer,
  chapter integer,
  usfm text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function clear ()
  {
    Database_SQLite::exec ($this->db, "DROP TABLE IF EXISTS bibleactions;");
    $this->create ();
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM bibleactions;");
  }


  public function record ($bible, $book, $chapter, $usfm)
  {
    if ($this->getUsfm ($bible, $book, $chapter) === false) {
      $bible = Database_SQLiteInjection::no ($bible);
      $book = Database_SQLiteInjection::no ($book);
      $chapter = Database_SQLiteInjection::no ($chapter);
      $usfm = Database_SQLiteInjection::no ($usfm);
      $query = "INSERT INTO bibleactions VALUES ('$bible', $book, $chapter, '$usfm');";
      Database_SQLite::exec ($this->db, $query);
    }
  }


  public function getBibles ()
  {
    $notes = array ();
    $query = "SELECT DISTINCT bible FROM bibleactions ORDER BY bible;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $notes [] = $row [0];
    }
    return $notes;
  }


  public function getBooks ($bible)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $books = array ();
    $query = "SELECT DISTINCT book FROM bibleactions WHERE bible = '$bible' ORDER BY book;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $books [] = $row [0];
    }
    return $books;
  }


  public function getChapters ($bible, $book)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapters = array ();
    $query = "SELECT DISTINCT chapter FROM bibleactions WHERE bible = '$bible' AND book = $book ORDER BY chapter;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $chapters [] = $row [0];
    }
    return $chapters;
  }


  public function getUsfm ($bible, $book, $chapter)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $query = "SELECT usfm FROM bibleactions WHERE bible = '$bible' AND book = $book AND chapter = $chapter;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return false;
  }


  public function delete ($bible, $book, $chapter)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $query = "DELETE FROM bibleactions WHERE bible = '$bible' AND book = $book AND chapter = $chapter;";
    Database_SQLite::exec ($this->db, $query);
  }
   

}


?>
