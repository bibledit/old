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


// This is a database for the verse mapping systems.
// Resilience: It is normally not written to, so corruption is unlikely.


class Database_Mappings
{
  public $db;
  private static $instance;
  private function __construct() {
    $this->db = Database_SQLite::connect ("mappings");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Mappings ();
    }
    return self::$instance;
  }


  public function create1 ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS maps (
name text,
book integer,
chapter integer,
verse integer,
origbook integer,
origchapter integer,
origverse integer
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $sql = "DROP INDEX IF EXISTS bible;";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "DROP INDEX IF EXISTS original;";
    Database_SQLite::exec ($this->db, $sql);
  }
  

  public function create2 ()
  {
    $sql = "CREATE INDEX IF NOT EXISTS bible ON maps (name, book, chapter, verse);";
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS original ON maps (name, book, chapter, verse);";
    Database_SQLite::exec ($this->db, $sql);
  }
  

  // Import the default mappings that come with Bibledit-Web.
  public function defaults () 
  {
    $folder = realpath (__DIR__ . "/../mapping");
    $paths = glob ("$folder/*.txt");
    foreach ($paths as $path) {
      $name = basename ($path);
      $name = substr ($name, 0, -4);
      $name = str_replace ("_", " ", $name);
      $name = ucwords ($name);
      $data = file_get_contents ($path);
      $this->import ($name, $data);
    }
  }


  public function optimize () 
  {
    Database_SQLite::exec ($this->db, "VACUUM maps;");
  }


  public function import ($name, $data)
  {
    // Delete existing mapping with this name.
    $this->delete ($name);

    // Begin a transaction for better speed.
    Database_SQLite::exec ($this->db, "BEGIN;");
    
    $lastPassage = array (1, 1, 1);
    $lastOriginal = array (1, 1, 1);
    
    $name = Database_SQLiteInjection::no ($name);
    
    $data = Filter_String::string2array ($data);
    foreach ($data as $line) {
      
      // Each line looks like this: 
      // Haggai 2:15 = Haggai 2:14
      // At the left is the passage in this versification.
      // At the right is the passage in the original Hebrew and Greek versification.
      $line = trim ($line);
      if ($line == "") continue;

      // Cut the line into two: The two passages.
      $entry = explode ("=", $line);
      if (count ($entry) != 2) continue;
      
      $passage = trim ($entry [0]);
      $original = trim ($entry [1]);
      
      $passage = Filter_Books::interpretPassage ($lastPassage, $passage);
      $lastPassage = $passage;
      $original = Filter_Books::interpretPassage ($lastOriginal, $original);
      $lastOriginal = $original;

      $book        = (integer) $passage  [0];
      $chapter     = (integer) $passage  [1];
      $verse       = (integer) $passage  [2];
      $origbook    = (integer) $original [0];
      $origchapter = (integer) $original [1];
      $origverse   = (integer) $original [2];

      $query = "INSERT INTO maps VALUES ('$name', $book, $chapter, $verse, $origbook, $origchapter, $origverse);";
      Database_SQLite::exec ($this->db, $query);

    }

    // Commit the transaction.
    Database_SQLite::exec ($this->db, "COMMIT;");
  }


  public function export ($name)
  {
    $data = array ();
    $database_books = Database_Books::getInstance ();
    $name = Database_SQLiteInjection::no ($name);
    $query = "SELECT * FROM maps WHERE name = '$name' ORDER BY book ASC, chapter ASC, verse ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $book = $row ['book'];
      $book = $database_books->getEnglishFromId ($book);
      $chapter = $row ['chapter'];
      $verse = $row ['verse'];
      $origbook = $row ['origbook'];
      $origbook = $database_books->getEnglishFromId ($origbook);
      $origchapter = $row ['origchapter'];
      $origverse = $row ['origverse'];
      $item = "$book $chapter:$verse = $origbook $origchapter:$origverse";
      $data [] = $item;
    }
    $data = implode ("\n", $data);
    return $data;
  }


  public function create ($name)
  {
    $name = Database_SQLiteInjection::no ($name);
    // Insert one entry, so the $name makes it into the database.
    $query = "INSERT INTO maps VALUES ('$name', 1, 1, 1, 1, 1, 1);";
    Database_SQLite::exec ($this->db, $query);
  }
  

  public function delete ($name)
  {
    $name = Database_SQLiteInjection::no ($name);
    $query = "DELETE FROM maps WHERE name = '$name';";
    Database_SQLite::exec ($this->db, $query);
  }


  // Returns the mapping names in the database.
  public function names ()
  {
    $names = array ();
    
    $query = "SELECT DISTINCT name FROM maps;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $names [] = $row[0];
    }

    // Ensure the original mapping is there too.
    if (!in_array ($this->original (), $names)) {
      $names [] = $this->original ();
    }

    sort ($names);

    return $names;
  }


  public function original ()
  {
    return "Hebrew Greek";
  }
  

  // This function translates a $book, $chapter, and $verse 
  // in the $input versification to a $book, $chapter and $verse 
  // in the $output versification.
  // It returns an array with one passage in most cases.
  // When the verses in the $input and $output versifications overlap,
  // it may return an array with two passages.
  public function translate ($input, $output, $book, $chapter, $verse)
  {
    // Care for situation that the input and output are the same.
    if ($input == $output) {
      $passage = array ($book, $chapter, $verse);
      return array ($passage);
    }

    // Get the $input mapping for the passage from the database.
    // This maps the $input to the Hebrew/Greek versification system.
    // Skip this phase if the $input mapping is Hebrew / Greek.
    $origpassage = array ();
    if ($input != $this->original ()) {
      $input = Database_SQLiteInjection::no ($input);
      $book = (integer) $book;
      $chapter = (integer) $chapter;
      $verse = (integer) $verse;
      $query = "SELECT origbook, origchapter, origverse FROM maps WHERE name = '$input' AND book = $book AND chapter = $chapter AND verse = $verse;";
      $result = Database_SQLite::query ($this->db, $query);
      foreach ($result as $row) {
        unset ($row ['origbook']);
        unset ($row ['origchapter']);
        unset ($row ['origverse']);
        $origpassage [] = $row;
      }
    }
    
    // Check that the search yields a passage.
    // If there is none, it means that the $input passage is the same as in Hebrew/Greek.
    if (empty ($origpassage)) {
      $passage = array ($book, $chapter, $verse);
      $origpassage = array ($passage);
    }

    // If the $output mapping is Hebrew/Greek, then we're done.
    if ($output == $this->original ()) {
      return $origpassage;
    }
    
    // Get the $output mapping for the passage or two passages from the database.
    // This is a translation from Hebrew/Greek to the $output system.
    $targetpassage = array ();
    $output = Database_SQLiteInjection::no ($output);
    foreach ($origpassage as $passage) {
      $origbook = $passage [0];
      $origchapter = $passage [1];
      $origverse = $passage [2];
      $query = "SELECT book, chapter, verse FROM maps WHERE name = '$output' AND origbook = $origbook AND origchapter = $origchapter AND origverse = $origverse;";
      $result = Database_SQLite::query ($this->db, $query);
      foreach ($result as $row) {
        unset ($row ['book']);
        unset ($row ['chapter']);
        unset ($row ['verse']);
        $row [0] = (integer) $row [0];
        $row [1] = (integer) $row [1];
        $row [2] = (integer) $row [2];
        if (array_search ($row, $targetpassage) === false) {
          $targetpassage [] = $row;
        }
      }
    }
    
    // Check that the search yields a passage.
    // If none, it means that the $output passage is the same as in Hebrew/Greek.
    if (empty ($targetpassage)) {
      $targetpassage = $origpassage;
    }
    
    // Result.
    return $targetpassage;
  }


}


?>
