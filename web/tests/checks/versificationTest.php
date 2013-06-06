<?php


class versificationTest extends PHPUnit_Framework_TestCase
{


  public $versificationID;

 
  public function setUp ()
  {
    $this->tearDown ();
    $database_versifications = Database_Versifications::getInstance ();
    $this->versificationID = $database_versifications->getID ("English");
  }

  
  public function tearDown ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->truncateOutput ();
  }


  public function testAvailableBooks ()
  {
    $database_versifications = Database_Versifications::getInstance ();
    $data = $database_versifications->getBooksChaptersVerses ("English");
    $books = array ();
    while ($row = $data->fetch_assoc()) {
      $book       = $row["book"];
      $books[]    = $book;
    }
    $books = array_unique ($books, SORT_NUMERIC);
    $fault = array_diff ($books, array (10));
    Checks_Versification::books ("Bible", $fault);
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'id' => "1",
     'bible' => "0",
     'book' => "10",
     'chapter' => "1",
     'verse' => "1",
     'data' => "This book is absent from the Bible"
    ));
    $this->assertEquals ($standard, $result);
  }


  public function testAvailableChapters ()
  {
    // Ruth.
    $book = 8; 
    // One missing and one extra chapter.
    $chapters = array (0, 2, 3, 4, 5);
    // Run check.
    Checks_Versification::chapters ("Bible", $book, $chapters);
    // Verify results.
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (
      array (
       'id' => "1",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "1",
       'data' => "This chapter is missing"
      ), 
      array (
       'id' => "2",
       'bible' => "0",
       'book' => "8",
       'chapter' => "5",
       'verse' => "1",
       'data' => "This chapter is extra"
      )
    );
    $this->assertEquals ($standard, $result);
  }


  public function testAvailableVersesOne ()
  {
    // Ruth 1.
    $book = 8; 
    $chapter = 1;
    // One missing and one extra verse.
    $verses = array (0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24);
    // Run check.
    Checks_Versification::verses ("Bible", $book, $chapter, $verses);
    // Verify results.
    $database_check = Database_Check::getInstance ();
    $results = $database_check->getHits ();
    $standard = array (
      array (
       'id' => "1",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "1",
       'data' => "This verse is missing according to the versification system"
      ), 
      array (
       'id' => "2",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "24",
       'data' => "This verse is extra according to the versification system"
      ),
      array (
       'id' => "3",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "0",
       'data' => "Not all verses in this chapter are in ascending order"
      )
    );
    $this->assertEquals ($standard, $results);
  }


  public function testAvailableVersesTwo ()
  {
    // Ruth 1.
    $book = 8; 
    $chapter = 1;
    // One missing and one extra verse.
    $verses = array (0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
    // Run check.
    Checks_Versification::verses ("Bible", $book, $chapter, $verses);
    // Verify results.
    $database_check = Database_Check::getInstance ();
    $results = $database_check->getHits ();
    $standard = array (
      array (
       'id' => "1",
       'bible' => "0",
       'book' => "8",
       'chapter' => "1",
       'verse' => "0",
       'data' => "Not all verses in this chapter are in ascending order"
      )
    );
    $this->assertEquals ($standard, $results);
  }


}
?>
