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


  public function testAvailableChapters () // Todo
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
    $book = 8; // Ruth.
    $chapters = array (0, 2, 3, 4, 5);
    Checks_Versification::chapters ("Bible", $book, $chapters);
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




}
?>
