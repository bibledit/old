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


// This is the database for the Hebrew Bible.
// Resilience: It is never written to.
// Chances of corruption are nearly zero.


class Database_Morphhb
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("morphhb");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Morphhb ();
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
CREATE TABLE IF NOT EXISTS morphhb (
  book integer,
  chapter integer,
  verse integer,
  hebrew text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $books = array (
      "Gen",
      "Exod",
      "Lev",
      "Num",
      "Deut",
      "Josh",
      "Judg",
      "Ruth",
      "1Sam",
      "2Sam",
      "1Kgs",
      "2Kgs",
      "1Chr",
      "2Chr",
      "Ezra",
      "Neh",
      "Esth",
      "Job",
      "Ps",
      "Prov",
      "Eccl",
      "Song",
      "Isa",
      "Jer",
      "Lam",
      "Ezek",
      "Dan",
      "Hos",
      "Joel",
      "Amos",
      "Obad",
      "Jonah",
      "Mic",
      "Nah",
      "Hab",
      "Zeph",
      "Hag",
      "Zech",
      "Mal"
    );

    foreach ($books as $book => $osis) {
    
      $book++;
      $database_logs->log ("Importing Hebrew data for $book $osis");
    
      $xml = new XMLReader();
    
      $xml->open ("../morphhb/$osis.xml");
    
      $chapter = 0;
      $verse = 0;
      $word = false;
      $hebrew = "";
    
      while ($xml->read ()) {
    
        $nodeType = $xml->nodeType;
      
        $name = $xml->name;
    
        if ($nodeType == XMLReader::ELEMENT) {
      
          if ($name == "verse") {
            $osisID = $xml->getAttribute ("osisID");
            $osisID = explode (".", $osisID);
            $chapter = intval ($osisID[1]);
            $verse = intval ($osisID[2]);
          }
      
          if ($name == "w") {
            $word = true;
          }
      
        }
    
        if ($nodeType == XMLReader::TEXT) {
          if ($word) {
            $hebrew = $xml->value;
            $hebrew = trim ($hebrew);
            $hebrew = str_replace ("/", "", $hebrew);
            $hebrew = str_replace ("'", "''", $hebrew);
            $sql = "INSERT INTO morphhb (book, chapter, verse, hebrew) VALUES ($book, $chapter, $verse, '$hebrew');";
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
    
  }


  // Get Hebrew words for $book $chapter $verse.
  public function getVerse ($book, $chapter, $verse)
  {
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $query = "SELECT hebrew FROM morphhb WHERE book = $book AND chapter = $chapter AND verse = $verse;";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      $hits [] = $row;
    }
    return $hits;
  }
  

  // Get array of book / chapter / verse of all passages that contain a $hebrew word.
  public function searchHebrew ($hebrew)
  {
    $hebrew = Database_SQLiteInjection::no ($hebrew);
    $query = "SELECT DISTINCT book, chapter, verse FROM morphhb WHERE hebrew = '$hebrew';";
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
