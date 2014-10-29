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


// This is a database for the various versification systems.
// Resilience: It is normally not written to, so corruption is unlikely.


class Database_Versifications
{
  public $db;
  private static $instance;
  private function __construct() {
    $this->db = Database_SQLite::connect ("versifications");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Versifications();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS names (
system integer,
name text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS data (
system integer,
book integer,
chapter integer,
verse integer
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }
  

  public function optimize () {
    Database_SQLite::exec ($this->db, "VACUUM names;");
    Database_SQLite::exec ($this->db, "VACUUM data;");
  }


  public function importBibleditXml ($contents, $name)
  {
    // Delete old system if it is there, and create new one.
    $this->delete ($name);
    $id = $this->createSystem ($name);
    $database_books = Database_Books::getInstance();
    $xml = simplexml_load_string ($contents);
    foreach ($xml as $triad) {
      $book = $triad->book;
      $book = $database_books->getIdFromEnglish ($book);
      $chapter = $triad->chapter;
      $verse = $triad->verse;
      $query = "INSERT INTO data (system, book, chapter, verse) VALUES ($id, $book, $chapter, $verse);";
      Database_SQLite::exec ($this->db, $query);
    }
  }


  public function exportBibleditXmlFile ($name)
  {
    $database_books = Database_Books::getInstance();
    $xml = new SimpleXMLElement ("<bibledit-versification-system></bibledit-versification-system>");
    $versification_data = $this->getBooksChaptersVerses ($name);
    foreach ($versification_data as $row) {
      $triad = $xml->addChild ("triad");
      $book = $row["book"];
      $book = $database_books->getEnglishFromId ($book);
      $triad->addChild ("book", $book);
      $chapter = $row["chapter"];
      $triad->addChild ("chapter", $chapter);
      $verse = $row["verse"];
      $triad->addChild ("verse", $verse);
    }
    $domnode = dom_import_simplexml ($xml);
    $dom = new DOMDocument ("1.0", "UTF-8");
    $domnode = $dom->importNode ($domnode, true);
    $dom->appendChild ($domnode);
    $dom->formatOutput = true;
    $string = $dom->saveXML ();
    return $string;
  }


  // Delete a versification system.
  public function delete ($name)
  {
    $id = $this->getID ($name);
    $query = "DELETE FROM names WHERE system = $id;";
    Database_SQLite::exec ($this->db, $query);
    $query = "DELETE FROM data WHERE system = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  // Returns the ID for a named versification system.
  public function getID ($name)
  {
    $name = Database_SQLiteInjection::no ($name);
    $query = "SELECT system FROM names WHERE name = '$name';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  // Creates a new empty versification system.
  // Returns the new ID.
  public function createSystem ($name)
  {
    // If the versification system already exists, return its ID.
    $id = $this->getID ($name);
    if ($id > 0) {
      return $id;
    }
    // Get the first free ID starting from 1000.
    $id = 0;
    $query = "SELECT system FROM names ORDER BY system DESC LIMIT 1;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $id = $row[0];
    }
    $id++;
    if ($id < 1000) $id = 1000;
    // Create the empty system.
    $name = Database_SQLiteInjection::no ($name);
    $query = "INSERT INTO names VALUES ($id, '$name');";
    Database_SQLite::exec ($this->db, $query);
    // Return new ID.
    return $id;
  }


  /**
    * getSystems - Returns an array of the available versification systems.
    */
  public function getSystems ()
  {
    $systems = array ();
    $query = "SELECT name FROM names ORDER BY name ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $systems[] = $row[0];
    }
    return $systems;
  }


  // Returns the books, chapters, verses for the given versification system.
  public function getBooksChaptersVerses ($name)
  {
    $data = array ();
    $id = $this->getID ($name);
    $query = "SELECT book, chapter, verse FROM data WHERE system = $id ORDER BY book, chapter, verse ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $data [] = $row;
    }
    return $data;
  }


  public function getBooks ($name)
  {
    $id = $this->getID ($name);
    $query = "SELECT DISTINCT book FROM data WHERE system = $id ORDER BY book ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    $books = array ();
    foreach ($result as $row) {
      $books [] = $row[0];
    }
    return $books;
  }


  // This returns all the chapters in $book of versification system $name.
  // $include0: Includes chapter 0 also.
  public function getChapters ($name, $book, $include0 = false)
  {
    $id = $this->getID ($name);
    $book = Database_SQLiteInjection::no ($book);
    $query = "SELECT DISTINCT chapter FROM data WHERE system = $id AND book = $book ORDER BY chapter ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    $chapters = array ();
    if ($include0) $chapters [] = 0;
    foreach ($result as $row) {
      $chapters [] = $row[0];
    }
    return $chapters;
  }


  public function getVerses ($name, $book, $chapter)
  {
    $id = $this->getID ($name);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $query = "SELECT DISTINCT verse FROM data WHERE system = $id AND book = $book AND chapter = $chapter ORDER BY verse ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    $verses = array ();
    foreach ($result as $row) {
      $verse = $row[0];
      for ($i = 0; $i <= $verse; $i++) {
        $verses [] = $i;
      }
    }
    // Put verse 0 in chapter 0.
    if ($chapter == 0) $verses [] = 0;
    return $verses;
  }


}



?>
