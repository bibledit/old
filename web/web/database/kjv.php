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


// This is the database for the Strong's numbers and English glosses.
// Resilience: It is never written to. 
// Chances of corruption are nearly zero.


class Database_Kjv
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("kjv");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Kjv ();
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
CREATE TABLE kjv (
  book integer,
  chapter integer,
  verse integer,
  strong text,
  english text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $book = 0;
    $chapter = 0;
    $verse = 0;
    $lemma = "";
    $english = "";
    
    $xml = new XMLReader();
    
    $xml->open ("../kjv/kjvfull.xml");
    
    while ($xml->read ()) {
    
      $nodeType = $xml->nodeType;
    
      $name = $xml->name;
    
      if ($nodeType == XMLReader::ELEMENT) {
    
        if ($name == "div") {
          $type = $xml->getAttribute ("type");
          if ($type == "book") {
            $book++;
            $database_logs->log ("Importing Strong's data and English glosses for Bible book" . " " . $book);
            $chapter = 0;
            $verse = 0;
          }
        }
    
        if ($name == "chapter") {
          $chapter++;
          $verse = 0;
        }
    
        if ($name == "verse") {
          $sID = $xml->getAttribute ("sID");
          if ($sID) {
            $verse++;
          }
        }
    
        if ($name == "w") {
          $lemma = $xml->getAttribute ("lemma");
          $lemma = trim ($lemma);
        }
    
      }
    
      if ($nodeType == XMLReader::TEXT) {
        $value = $xml->value;
        $english = trim ($value);
      }
    
      if ($nodeType == XMLReader::END_ELEMENT) {
        if ($name == "w") {
          $lemma = explode (" ", $lemma);
          foreach ($lemma as $strong) {
            if (strpos ($strong, "strong") === false) continue;
            $strong = str_replace ("strong:", "", $strong);
            $strong = str_replace ("'", "''", $strong);
            $english = str_replace ("'", "''", $english);
            $sql = "INSERT INTO kjv (book, chapter, verse, strong, english) VALUES ($book, $chapter, $verse, '$strong', '$english');";
            Database_SQLite::exec ($this->db, $sql);
          }
          $lemma = "";
          $english = "";
        }
      }
    
    }
    
    $xml->close ();
  
  }
  

  // Get Strong's numbers and English snippets for $book $chapter $verse.
  public function getVerse ($book, $chapter, $verse)
  {
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $query = "SELECT strong, english FROM kjv WHERE book = $book AND chapter = $chapter AND verse = $verse;";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      $hits [] = $row;
    }
    return $hits;
  }


  // Get array of book / chapter / verse of all passages that contain a $strong's number.
  public function searchStrong ($strong)
  {
    $strong = Database_SQLiteInjection::no ($strong);
    $query = "SELECT DISTINCT book, chapter, verse FROM kjv WHERE strong = '$strong';";
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
