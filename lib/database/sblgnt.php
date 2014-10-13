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


// This is the database for the Greek New Testament.
// Resilience: It is never written to. 
// Chances of corruption are nearly zero.


class Database_Sblgnt
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("sblgnt");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Sblgnt ();
    }
    return self::$instance;
  }


  public function create ()
  {
    $database_logs = Database_Logs::getInstance ();

    Database_SQLite::exec ($this->db, "PRAGMA temp_store = MEMORY;");
    Database_SQLite::exec ($this->db, "PRAGMA synchronous = OFF;");
    Database_SQLite::exec ($this->db, "PRAGMA journal_mode = OFF;");

$sql = <<<'EOD'
CREATE TABLE sblgnt (
  book integer,
  chapter integer,
  verse integer,
  greek text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $book = 40 - 1;
    $chapter = 0;
    $verse = 0;
    $word = false;
    $greek = "";
    
    $xml = new XMLReader();
    
    $xml->open ("../sblgnt/sblgnt.xml");
    
    while ($xml->read ()) {
    
      $nodeType = $xml->nodeType;
    
      $name = $xml->name;
    
      if ($nodeType == XMLReader::ELEMENT) {
    
        if ($name == "book") {
          $book++;
          $chapter = 0;
          $verse = 0;
          $database_logs->log ("Importing Greek data for book $book");
          $type = $xml->getAttribute ("type");
          if ($type == "book") {
          }
        }
    
        if ($name == "verse-number") {
          $id = $xml->getAttribute ("id");
          $id = explode (":", $id);
          $chapter = substr ($id[0], -2, 2);
          $chapter = intval ($chapter);
          $verse = intval ($id[1]);
        }
    
        if ($name == "w") {
          $word = true;
        }
    
      }
    
      if ($nodeType == XMLReader::TEXT) {
        if ($word) {
          $greek = $xml->value;
          $greek = trim ($greek);
          $greek = str_replace ("'", "''", $greek);
          $sql = "INSERT INTO sblgnt (book, chapter, verse, greek) VALUES ($book, $chapter, $verse, '$greek');";
          Database_SQLite::exec ($this->db, $sql);
        }
      }
    
      if ($nodeType == XMLReader::END_ELEMENT) {
        if ($name == "w") {
          $word = false;
        }
      }
    
    }
    
    $xml->close ();

  }


  // Get Greek words for $book $chapter $verse.
  public function getVerse ($book, $chapter, $verse)
  {
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $query = "SELECT greek FROM sblgnt WHERE book = $book AND chapter = $chapter AND verse = $verse;";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      $hits [] = $row;
    }
    return $hits;
  }
  

  // Get array of book / chapter / verse of all passages that contain a $greek word.
  public function searchGreek ($greek)
  {
    $greek = Database_SQLiteInjection::no ($greek);
    $query = "SELECT DISTINCT book, chapter, verse FROM sblgnt WHERE greek = '$greek';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      unset ($row[2]);
      $hits [] = $row;
    }
    return $hits;
  }
  

}


?>
