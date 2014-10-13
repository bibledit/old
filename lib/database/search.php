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


/*

Database resilience: The index is re-created every night.

Optional improvement:
Improved searching on bible and notes, also using concepts like 'near' and 'relevance' and 'score'.
Search via Zend_Lucene: 
http://www.keesschepers.nl/2011/01/03/zend-lucene/
http://framework.zend.com/manual/1.12/en/zend.search.lucene.html
Advantages: No libraries needed. Pure PHP.
It could leave the "cleantext" in SQLite for the literal string searching, and use Lucene for more intelligent searches.
Disadvantage: More complex and slower mechanism than through SQLite only.

*/

class Database_Search
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("search");
    Database_SQLite::exec ($this->db, "PRAGMA temp_store = MEMORY;");
    Database_SQLite::exec ($this->db, "PRAGMA synchronous = OFF;");
    Database_SQLite::exec ($this->db, "PRAGMA journal_mode = OFF;");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Search ();
    }
    return self::$instance;
  }


  public function create ()
  {
    // Searching in SQLite could use the FTS3 or FTS4 module.
    // This module is optional. 
    // On some shared hosting accounts it may be available. On others it may not.
    // Therefore it cannot be used.
    // Searching uses a stanard table.
    
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS bibles (
  bible text,
  book integer,
  chapter integer,
  verse integer,
  usfmraw text,
  usfmlower text,
  plainraw text,
  plainlower text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    $sql = "CREATE INDEX IF NOT EXISTS speedup ON bibles (bible, book, chapter)";
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize () 
  {
    Database_SQLite::exec ($this->db, "REINDEX bibles;");
    Database_SQLite::exec ($this->db, "VACUUM bibles;");
  }
  

  // Does a checkup on the health of the database.
  // Returns false if the database is healthy.
  // Returns true if the database was re-created, and is now healthy.
  // If $recreate is true, then it will be always recreated.
  public function checkup ($recreate = false)
  {
    // Do an integrity check, and bail out if the database is healthy.
    if (!$recreate) {
      $query = "PRAGMA integrity_check;";
      $result = Database_SQLite::query ($this->db, $query);
      foreach ($result as $row) {
        if ($row[0] == "ok") return false;
      }
    }

    // Recreate the database.
    unset ($this->db);
    @unlink ("../databases/search.sqlite");
    $this->__construct ();
    $this->create ();
    return true;
  }


  // Updates the search fields for a chapter in Bible $name.
  public function updateSearchFields ($name, $book, $chapter)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_config_bible = Database_Config_Bible::getInstance ();

    $usfm = $database_bibles->getChapter ($name, $book, $chapter);
    $stylesheet = $database_config_bible->getExportStylesheet ($name);

    // Data to store.
    $usfmraw = array ();
    $usfmlower = array ();
    $plainraw = array ();
    $plainlower = array ();

    // Get the verses in the chapter.
    $verses = Filter_Usfm::getVerseNumbers ($usfm);
    $verses = array_unique ($verses);
    sort ($verses, SORT_NUMERIC);

    // One column contains the raw USFM as it is, and another one the lowercase text.
    foreach ($verses as $verse) {
      $raw = Filter_Usfm::getVerseText ($usfm, $verse);
      $lower = mb_convert_case ($raw, MB_CASE_LOWER);
      $usfmraw [$verse] = $raw;
      $usfmlower [$verse] = $lower;
    }
    
    // Text filter for getting the plain text.
    $filter_text = new Filter_Text ($name);
    $filter_text->text_text = new Text_Text ();
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ($stylesheet);

    // Get the clean verse texts.
    $texts = $filter_text->getVersesText ();
    foreach ($texts as $verse => $text) {
      if (!isset ($plainraw [$verse])) $plainraw [$verse] = "";
      $plainraw [$verse] .= "$text\n";
    }

    // Add any clean headings.
    $headings = $filter_text->verses_headings;
    foreach ($headings as $verse => $heading) {
      if (!isset ($plainraw [$verse])) $plainraw [$verse] = "";
      $plainraw [$verse] .= "$heading\n";
    }
    
    // Create the lower case plain text.
    foreach ($plainraw as $verse => $raw) {
      $plainlower [$verse] = mb_convert_case ($raw, MB_CASE_LOWER);
    }
    
    // Get all possible verses.
    $allverses = array_merge (array_keys ($usfmraw), array_keys ($plainraw));
    $allverses = array_unique ($allverses);

    // Store everything.
    Database_SQLite::exec ($this->db, "BEGIN;");
    $name = Database_SQLiteInjection::no ($name);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $query = "DELETE FROM bibles WHERE bible = '$name' AND book = $book AND chapter = $chapter;";
    Database_SQLite::exec ($this->db, $query);
    foreach ($allverses as $verse) {
      if ($verse == "") $verse = 0;
      $verse = (integer) $verse;
      @$ur = Database_SQLiteInjection::no ($usfmraw [$verse]);
      @$ul = Database_SQLiteInjection::no ($usfmlower [$verse]);
      @$pr = Database_SQLiteInjection::no ($plainraw [$verse]);
      @$pl = Database_SQLiteInjection::no ($plainlower [$verse]);
      $query = "INSERT INTO bibles (bible, book, chapter, verse, usfmraw, usfmlower, plainraw, plainlower) VALUES ('$name', $book, $chapter, $verse, '$ur', '$ul', '$pr', '$pl');";
      Database_SQLite::exec ($this->db, $query);
    }
    Database_SQLite::exec ($this->db, "COMMIT;");
  }


  /**
  * Searches the text of the Bibles.
  * Returns an array with the rowid's of matching verses.
  * $search: Contains the text to search for.
  * $bibles: Array of Bible names to search in.
  */
  public function searchText ($search, $bibles)
  {
    $ids = array ();

    if ($search == "") {
      return $ids;
    }

    $search = mb_convert_case ($search, MB_CASE_LOWER);
    $search = str_replace (",", "", $search);
    $search = Database_SQLiteInjection::no ($search);

    $bibleCondition = "1";
    if (!empty ($bibles)) {
      $bibleCondition = " ( ";
      foreach ($bibles as $offset => $bible) {
        if ($offset) $bibleCondition .= " OR ";
        $bible = Database_SQLiteInjection::no ($bible);
        $bibleCondition .= " bible = '$bible' ";
      }
      $bibleCondition .= " ) ";
    }

    $query = "SELECT rowid FROM bibles WHERE $bibleCondition AND plainlower LIKE '%$search%';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $hits [] = $row [0];
    }
    
    return $hits;
  }


  /**
  * Performs a case-insensitive search of the text of one $bible.
  * Returns an array with the rowid's of matching verses.
  * $search: Contains the text to search for.
  */
  public function searchBibleText ($bible, $search)
  {
    $ids = array ();

    $bible = Database_SQLiteInjection::no ($bible);

    $search = mb_convert_case ($search, MB_CASE_LOWER);
    $search = Database_SQLiteInjection::no ($search);

    $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND plainlower LIKE '%$search%';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $hits [] = $row [0];
    }
    
    return $hits;
  }


  /**
  * Performs a case-sensitive search of the text of one $bible.
  * Returns an array with the rowid's of matching verses.
  * $search: Contains the text to search for.
  */
  public function searchBibleTextCaseSensitive ($bible, $search)
  {
    $ids = array ();

    $bible = Database_SQLiteInjection::no ($bible);
    $search = Database_SQLiteInjection::no ($search);

    $query = "PRAGMA case_sensitive_like = true;";
    Database_SQLite::exec ($this->db, $query);
    
    $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND plainraw LIKE '%$search%';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $hits [] = $row [0];
    }

    $query = "PRAGMA case_sensitive_like = false;";
    Database_SQLite::exec ($this->db, $query);
    
    return $hits;
  }


  /**
  * Performs a case-insensitive search of the USFM of one $bible.
  * Returns an array with the rowid's of matching verses.
  * $search: Contains the text to search for.
  */
  public function searchBibleUsfm ($bible, $search)
  {
    $ids = array ();

    $bible = Database_SQLiteInjection::no ($bible);

    $search = mb_convert_case ($search, MB_CASE_LOWER);
    $search = Database_SQLiteInjection::no ($search);

    $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND usfmlower LIKE '%$search%';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $hits [] = $row [0];
    }
    
    return $hits;
  }


  /**
  * Performs a case-sensitive search of the USFM of one $bible.
  * Returns an array with the rowid's of matching verses.
  * $search: Contains the text to search for.
  */
  public function searchBibleUsfmCaseSensitive ($bible, $search)
  {
    $ids = array ();

    $bible = Database_SQLiteInjection::no ($bible);
    $search = Database_SQLiteInjection::no ($search);

    $query = "PRAGMA case_sensitive_like = true;";
    Database_SQLite::exec ($this->db, $query);
    
    $query = "SELECT rowid FROM bibles WHERE bible = '$bible' AND usfmraw LIKE '%$search%';";
    $hits = array ();
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $hits [] = $row [0];
    }

    $query = "PRAGMA case_sensitive_like = false;";
    Database_SQLite::exec ($this->db, $query);
    
    return $hits;
  }


  public function debug ()
  {
    $result = Database_SQLite::query ($this->db, "SELECT * from bibles;");
    foreach ($result as $row) {
      for ($i = 0; $i < 10; $i++) unset ($row[$i]);
      var_dump ($row);
    }
  }


  // Returns the Bible and passage for a $rowid.
  public function getBiblePassage ($rowid)
  {
    $rowid = Database_SQLiteInjection::no ($rowid);
    $query = "SELECT bible, book, chapter, verse FROM bibles WHERE rowid = $rowid;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row [0]);
      unset ($row [1]);
      unset ($row [2]);
      unset ($row [3]);
      return $row;
    }
    return NULL;
  }
  

  // Gets the plain raw text for the $bible and passage given.  
  public function getBibleVerseText ($bible, $book, $chapter, $verse)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $query = "SELECT plainraw FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "";
  }


  // Gets the raw USFM for the $bible and passage given.
  public function getBibleVerseUsfm ($bible, $book, $chapter, $verse)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $query = "SELECT usfmraw FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter AND verse = $verse;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "";
  }


  public function getBibles ()
  {
    $bibles = array ();
    $query = "SELECT DISTINCT bible FROM bibles ORDER BY bible ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $bibles [] = $row[0];
    }
    return $bibles;
  }
  
  
  public function deleteBible ($bible)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "DELETE FROM bibles WHERE bible = '$bible';";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  public function getBooks ($bible)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $books = array ();
    $query = "SELECT DISTINCT book FROM bibles WHERE bible = '$bible' ORDER BY book ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $books [] = $row[0];
    }
    return $books;
  }


  public function deleteBook ($bible, $book)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $query = "DELETE FROM bibles WHERE bible = '$bible' AND book = $book;";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  public function getChapters ($bible, $book)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapters = array ();
    $query = "SELECT DISTINCT chapter FROM bibles WHERE bible = '$bible' AND book = $book ORDER BY chapter ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $chapters [] = $row[0];
    }
    return $chapters;
  }


  public function deleteChapter ($bible, $book, $chapter)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $query = "DELETE FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter;";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  public function getVerses ($bible, $book, $chapter)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $book = Database_SQLiteInjection::no ($book);
    $chapter = Database_SQLiteInjection::no ($chapter);
    $verses = array ();
    $query = "SELECT DISTINCT verse FROM bibles WHERE bible = '$bible' AND book = $book AND chapter = $chapter ORDER BY chapter ASC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $verses [] = $row[0];
    }
    return $verses;
  }
  
  
  public function getVerseCount ($bible)
  {
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "SELECT count(*) FROM bibles WHERE bible = '$bible';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


}


?>
