<?php


class Database_Bibles
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Bibles();
    }
    return self::$instance;
  }

  /*
   * Optimize database table.
   */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE bible_names;");
    $database_instance->runQuery ("OPTIMIZE TABLE bible_data;");
    $database_instance->runQuery ("OPTIMIZE TABLE bible_diff;");
  }


  /*
   * Returns an array with the available Bibles.
   */
  public function getBibles ()
  {
    $bibles = array ();
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT name FROM bible_names ORDER BY name ASC;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $bibles[] = $row[0];
    }
    return $bibles;
  }


  /*
   * Returns the ID for a named Bible.
   */      
  public function getID ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $query = "SELECT bible FROM bible_names WHERE name = '$name';";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows == 0) {
      return 0;
    }
    $row = $result->fetch_row();
    return $row[0];
  }


  /*
   * Returns the Bible name for a Bible ID.
   */      
  public function getName ($id)
  {
    $database_instance = Database_Instance::getInstance();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT name FROM bible_names WHERE bible = $id;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows == 0) {
      return "Unknown";
    }
    $row = $result->fetch_row();
    return $row[0];
  }


  /*
   * Creates a new empty Bible. Returns its ID.
   */
  public function createBible ($name)
  {
    // If the Bible already exists, return its ID.
    $id = $this->getID ($name);
    if ($id > 0) {
      return $id;
    }   

    // Get the first free ID.
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT bible FROM bible_names ORDER BY bible DESC LIMIT 1;";
    $result = $database_instance->runQuery ($query);
    $id = 0;
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
    }
    $id++;
    // Create the empty system.
    $name = Database_SQLInjection::no ($name);
    $query = "INSERT INTO bible_names VALUES (NULL, $id, '$name', 'English');";
    $database_instance->runQuery ($query);
    // Return new ID.
    return $id;
  }


  /*
   * Deletes a Bible.
   */      
  public function deleteBible ($name)
  {
    // Store diff data prior to deletion.
    $this->storeDiffBible ($name);
    // Delete it.
    $database_instance = Database_Instance::getInstance();
    $id = $this->getID ($name);
    $query = "DELETE FROM bible_names WHERE bible = $id;";
    $database_instance->runQuery ($query);
    $query = "DELETE FROM bible_data WHERE bible = $id;";
    $database_instance->runQuery ($query);
  }

  
  /*
   * Gets the versification system of Bible $name.
   */
  public function getVersification ($name)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $query = "SELECT versification FROM bible_names WHERE name = '$name';";
    $result = $database_instance->runQuery ($query);
    $row = $result->fetch_row();
    return $row[0];
  }


  public function setVersification ($name, $versification)
  {
    $database_instance = Database_Instance::getInstance();
    $name = Database_SQLInjection::no ($name);
    $versification = Database_SQLInjection::no ($versification);
    $query = "UPDATE bible_names SET versification = '$versification' WHERE name = '$name';";
    $database_instance->runQuery ($query);
  }


  /*
   * Stores data of one chapter in Bible $name.
   */
  public function storeChapter ($name, $book, $chapter_number, $chapter_text)
  {
    // Return if the Bible $name does not exist.
    $bible_id = $this->getID ($name);
    if ($bible_id == 0) {
      $database_logs = Database_Logs::getInstance();
      $database_logs->log ("Bible $name does not exist: Failed to store a chapter");
      return;
    }
    // Store the data of the current chapter in the diff table prior to storing the new data.
    $this->storeDiff ($name, $book, $chapter_number);
    // Store the chapter.
    $database_instance = Database_Instance::getInstance();
    $book = Database_SQLInjection::no ($book);
    $chapter_number = Database_SQLInjection::no ($chapter_number);
    $chapter_text = Database_SQLInjection::no ($chapter_text);
    $query = "DELETE FROM bible_data WHERE bible = $bible_id AND book = $book AND chapter = $chapter_number;";
    $database_instance->runQuery ($query);
    $query = "INSERT INTO bible_data VALUES (NULL, $bible_id, $book, $chapter_number, '$chapter_text', NULL, NULL);";
    $database_instance->runQuery ($query);
    // Update search fields. 
    $this->updateSearchFields ($name, $book, $chapter_number);
  }


  /*
   * Updates the search fields for one chapter in Bible $name.
   */
  public function updateSearchFields ($name, $book, $chapter)
  {
    // Extract clean text from the USFM.
    $filter_text = new Filter_Text ("");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $usfm = $this->getChapter ($name, $book, $chapter);
    $filter_text->addUsfmCode ($usfm);
    $database_config_user = Database_Config_User::getInstance ();
    $stylesheet = $database_config_user->getStylesheet ();
    $filter_text->run ($stylesheet);

    // The clean text to search on.
    $searchText = array ();

    // First add the clean verse texts.
    $texts = $filter_text->verses_text;
    foreach ($texts as $verse => $text) {
      if (isset ($searchText [$verse])) {
        $searchText [$verse] .= "$text\n";
      } else {
        $searchText [$verse] = "$verse $text\n";
      }
    }
    
    // Secondly add the clean headings.
    $headings = $filter_text->verses_headings;
    foreach ($headings as $verse => $heading) {
      if (isset ($searchText [$verse])) {
        $searchText [$verse] .= "$heading\n";
      } else {
        $searchText [$verse] = "$verse $heading\n";
      }
    }

    // Sort the clean text on verse number.
    ksort ($searchText);

    // Create search texts for the columns "forward"  and "reverse"
    $forward = implode ("\n", $searchText);
    $reverse = Filter_String::reverse ($forward);

    // Store the search data in the table.
    $database_instance = Database_Instance::getInstance();
    $bible = $this->getID ($name);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $forward = Database_SQLInjection::no ($forward);
    $reverse = Database_SQLInjection::no ($reverse);
    $query = "UPDATE bible_data SET forward = '$forward', reverse = '$reverse' WHERE bible = $bible AND book = $book AND chapter = $chapter;";
    $database_instance->runQuery ($query);
  }
  

  /*
   * Returns an array with the available books in a Bible.
   */
  public function getBooks ($bible)
  {
    // Read the books from the database.
    $database_instance = Database_Instance::getInstance();
    $bible = $this->getID ($bible);
    $query = "SELECT DISTINCT book FROM bible_data WHERE bible = $bible;";
    $result = $database_instance->runQuery ($query);
    $books = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $books[] = $row[0];
    }
    // Sort the books into the sequence as defined in the books database.
    $sequence = array ();
    $database_books = Database_Books::getInstance ();  
    foreach ($books as $book) {
      $sequence [] = $database_books->getSequenceFromId ($book);
    }
    array_multisort ($sequence, $books);
    // Result.
    return $books;
  }


  public function deleteBook ($bible, $book)
  {
    // Store diff data prior to deleting the book.
    $this->storeDiffBook ($bible, $book);
    // Delete the book.
    $database_instance = Database_Instance::getInstance();
    $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $query = "DELETE FROM bible_data WHERE bible = $bible AND book = $book;";
    $database_instance->runQuery ($query);
  }


  /*
   * Returns an array with the available chapters in a $book in a Bible.
   */
  public function getChapters ($bible, $book)
  {
    // Read the chapters from the database.
    $database_instance = Database_Instance::getInstance();
    $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $query = "SELECT DISTINCT chapter FROM bible_data WHERE bible = $bible AND book = $book ORDER BY chapter ASC;";
    $result = $database_instance->runQuery ($query);
    $chapters = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $chapters[] = $row[0];
    }
    return $chapters;
  }


  public function deleteChapter ($bible, $book, $chapter)
  {
    // Store diff data prior to deletion.
    $this->storeDiff ($bible, $book, $chapter);
    // Delete the chapter.
    $database_instance = Database_Instance::getInstance();
    $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $query = "DELETE FROM bible_data WHERE bible = $bible AND book = $book AND chapter = $chapter;";
    $database_instance->runQuery ($query);
  }

  /*
   * Gets the chapter data as a string.
   */
  public function getChapter ($bible, $book, $chapter)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    if (!is_numeric ($bible)) $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $query = "SELECT data FROM bible_data WHERE bible = $bible AND book = $book AND chapter = $chapter;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return $row[0];
    }
    return "";    
  }
      

  /*
   * Returns true if diff data exists for the chapter.
   * Else it returns false.
   * The "diff" data helps producing the daily differences.
   */
  public function diffExists ($bible, $book, $chapter)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $query = "SELECT data FROM bible_diff WHERE bible = $bible AND book = $book AND chapter = $chapter;";
    $result = $database_instance->runQuery ($query);
    return ($result->num_rows > 0);
  }


  /*
   * Stores diff data for a "bible" (string) and $book (int) and $chapter (int).
   */
  private function storeDiff ($bible, $book, $chapter)
  {
    // Return when the diff exists.
    if ($this->diffExists ($bible, $book, $chapter)) {
      return;
    }
    // Return when the $bible does not exist.
    $bible = $this->getID ($bible);
    if ($bible == 0) {
      return;
    }
    // Store.
    $database_instance = Database_Instance::getInstance();
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $query = "INSERT INTO bible_diff SELECT id, bible, book, chapter, data FROM bible_data WHERE bible = $bible AND book = $book AND chapter = $chapter;";
    $database_instance->runQuery ($query);
  }


  /*
   * Gets the diff data as a string.
   */
  public function getDiff ($bible, $book, $chapter)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    if (!is_numeric ($bible)) $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $query = "SELECT data FROM bible_diff WHERE bible = $bible AND book = $book AND chapter = $chapter;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      return $row[0];
    }
    return "";    
  }
      

  /*
   * Stores diff data for all chapters in a "bible" (string) and $book (int).
   */
  public function storeDiffBook ($bible, $book)
  {
    $chapters = $this->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $this->storeDiff ($bible, $book, $chapter);
    }
  }


  /*
   * Stores diff data for all books in a "bible" (string).
   */
  public function storeDiffBible ($bible)
  {
    $books = $this->getBooks ($bible);
    foreach ($books as $book) {
      $this->storeDiffBook ($bible, $book);
    }
  }


  /*
   * Deletes the diffs for a whole Bible.
   */
  public function deleteDiffBible ($bible)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    if (!is_numeric ($bible)) $bible = $this->getID ($bible);
    $query = "DELETE FROM bible_diff WHERE bible = $bible;";
    $result = $database_instance->runQuery ($query);
  }
      

  /*
   * Returns an array with the available chapters that have diff data in a $book in a Bible.
   */
  public function getDiffChapters ($bible, $book)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    if (!is_numeric ($bible)) $bible = $this->getID ($bible);
    $book = Database_SQLInjection::no ($book);
    $query = "SELECT DISTINCT chapter FROM bible_diff WHERE bible = $bible AND book = $book ORDER BY chapter ASC;";
    $result = $database_instance->runQuery ($query);
    $chapters = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $chapters[] = $row[0];
    }
    return $chapters;
  }


  /*
   * Returns an array with the available books that have diff data in a Bible.
   * The $bible can be a name, or an identifier. This is because the $bible identifier may no longer exist.
   */
  public function getDiffBooks ($bible)
  {
    $database_instance = Database_Instance::getInstance();
    $bible = Database_SQLInjection::no ($bible);
    if (!is_numeric ($bible)) $bible = $this->getID ($bible);
    $query = "SELECT DISTINCT book FROM bible_diff WHERE bible = $bible ORDER BY book ASC;";
    $result = $database_instance->runQuery ($query);
    $books = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $books[] = $row[0];
    }
    return $books;
  }


  /*
   * Returns an array with the available Bibles that have diff data.
   */
  public function getDiffBibles ()
  {
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT bible FROM bible_diff ORDER BY bible ASC;";
    $result = $database_instance->runQuery ($query);
    $bibles = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $bibles[] = $row[0];
    }
    return $bibles;
  }


  /*
   * Truncates all diff data.
   */
  public function truncateDiffs ()
  {
    $database_instance = Database_Instance::getInstance();
    $result = $database_instance->runQuery ("TRUNCATE TABLE bible_diff;");
  }


  /**
  * Searches the text of the Bibles.
  * Returns an array of ids of the relevant chapters.
  * If $limit is non-NULL, it indicates the starting limit for the selection.
  * $search: Contains the text to search for.
  */
  public function searchText ($search, $limit = NULL)
  {
    $ids = array ();

    $search = str_replace (",", "", $search);
    $forward = Database_SQLInjection::no ($search);
    $reverse = Database_SQLInjection::no (Filter_String::reverse ($search));

    $query = " SELECT id , ";
    // Matches in natural language mode have the highest relevance.
    $query .= " (MATCH (forward) AGAINST ('$forward' IN NATURAL LANGUAGE MODE) * 10) ";
    $query .= " + ";
    // Matches in phrase search have higher relevance.
    $query .= " (MATCH (forward) AGAINST ('\"$forward\"' IN BOOLEAN MODE) * 5) ";
    $query .= " + ";
    // Prefix and suffix wildcards to the search words and search in boolean mode.
    $query .= " (MATCH (forward) AGAINST ('$forward*' IN BOOLEAN MODE)) + (MATCH (reverse) AGAINST ('$reverse*' IN BOOLEAN MODE) * 1) ";
    // Create relevance column to sort on.
    $query .= " AS relevance ";

    $query .= " FROM bible_data WHERE ";

    $query .= " (MATCH (forward) AGAINST ('$forward*' IN BOOLEAN MODE)) OR (MATCH (reverse) AGAINST ('$reverse*' IN BOOLEAN MODE)) ";

    // Notes get ordered on relevance of search hits.
    $query .= " ORDER BY relevance DESC ";

    // Optionally limit the number of hits.
    if (is_numeric ($limit)) {
      $limit = Database_SQLInjection::no ($limit);
      $query .= " LIMIT $limit, 50 ";
    }

    $query .= ";";

    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $id = $row[0];
      $ids []= $id;
    }

    return $ids;
  }
  
  
  /*
   * Returns an array with the bible, book and chapter of arecord $id.
   */
  public function getBibleBookChapter ($id)
  {
    $database_instance = Database_Instance::getInstance();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT bible, book, chapter FROM bible_data WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    return $result->fetch_assoc ();
  }


  public function getSearchField ($id)
  {
    $database_instance = Database_Instance::getInstance();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT forward FROM bible_data WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    $row = $result->fetch_row();
    return $row[0];
  }


}



?>
