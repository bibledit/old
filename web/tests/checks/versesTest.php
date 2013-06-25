<?php


class checksVersesTest extends PHPUnit_Framework_TestCase
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


  public function testPattern1 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::patterns (1, 1, 1, $verses, array ("did"));
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'id' => "1",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "3",
     'data' => "Pattern found in text: did"
    ));
    $this->assertEquals ($standard, $result);
  }


  public function testPattern2 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::patterns (1, 1, 1, $verses, array ("Did"));
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array ();
    $this->assertEquals ($standard, $result);
  }


  public function testPattern3 ()
  {
    $verses = array (2 => "He said.", 3 => "He didn't say", 4 => "He said.");
    Checks_Verses::patterns (1, 1, 1, $verses, array ("said"));
    $database_check = Database_Check::getInstance ();
    $result = $database_check->getHits ();
    $standard = array (array (
     'id' => "1",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "2",
     'data' => "Pattern found in text: said"
    ),
    array (
     'id' => "2",
     'bible' => "0",
     'book' => "1",
     'chapter' => "1",
     'verse' => "4",
     'data' => "Pattern found in text: said"
    ));
    $this->assertEquals ($standard, $result);
  }


}
?>
