<?php


class checksVersesTest extends PHPUnit_Framework_TestCase // Todo
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_check = Database_Check::getInstance ();
    $database_check->truncateOutput ();
  }


  public function testMissingPunctuationAtEnd1 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::missingPunctuationAtEnd (1, 1, 1, $verses, ", ;", ". ! ? :");
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'id' => "1",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "3",
     'data' => "No punctuation at end of verse: y"
    ));
    $this->assertEquals ($standard, $result);
  }



}
?>
