<?php

 
class bookTest extends PHPUnit_Framework_TestCase
{

  public function testPassageDisplay()
  {
    $this->assertEquals("Genesis 10:2", Filter_Books::passageDisplay (1, 10, 2));
    $this->assertEquals("Exodus 11:2", Filter_Books::passageDisplay (2, 11, 2));
  }

  public function testPassagesDisplayInline()
  {
    $this->assertEquals("Genesis 10:2", Filter_Books::passagesDisplayInline (array (array (1, 10, 2))));
    $this->assertEquals("", Filter_Books::passagesDisplayInline (array ()));
    $this->assertEquals("Genesis 10:2 | Exodus 777:777", Filter_Books::passagesDisplayInline (array (array (1, 10, 2), array (2, 777, 777))));
    $this->assertEquals("Unknown 10:2 | Exodus 777:777", Filter_Books::passagesDisplayInline (array (array (-1, 10, 2), array (2, 777, 777))));
  }

  public function testPassagesDisplayMultiline()
  {
    $this->assertEquals("Genesis 10:2\n", Filter_Books::passagesDisplayMultiline (array (array (1, 10, 2))));
    $this->assertEquals("", Filter_Books::passagesDisplayMultiline (array ()));
    $this->assertEquals("Genesis 10:2\nExodus 777:777\n", Filter_Books::passagesDisplayMultiline (array (array (1, 10, 2), array (2, 777, 777))));
    $this->assertEquals("Unknown 10:2\nExodus 777:777\n", Filter_Books::passagesDisplayMultiline (array (array (-1, 10, 2), array (2, 777, 777))));
  }

  public function testInterpretBook()
  {
    // Test interpretation of full English names.
    $this->assertEquals(1, Filter_Books::interpretBook ("Genesis"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 Corinthians"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Song of Solomon"));
    // Test interpretation of USFM abbreviations.
    $this->assertEquals(1, Filter_Books::interpretBook ("GEN"));
    $this->assertEquals(67, Filter_Books::interpretBook ("FRT"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1CO"));
    // Test interpretation of OSIS books.
    $this->assertEquals(1, Filter_Books::interpretBook ("Gen"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Cor"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Song"));
    // Test interpretation of BibleWorks abbreviations.
    $this->assertEquals(1, Filter_Books::interpretBook ("Gen"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Co"));
    $this->assertEquals(22, Filter_Books::interpretBook ("Sol"));
    // Test interpretation of Online Bible abbreviations.
    $this->assertEquals(1, Filter_Books::interpretBook ("Ge"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1Co"));
    $this->assertEquals(22, Filter_Books::interpretBook ("So"));
    // Test partial book names.
    $this->assertEquals(1, Filter_Books::interpretBook ("G"));
    $this->assertEquals(1, Filter_Books::interpretBook ("g"));
    $this->assertEquals(1, Filter_Books::interpretBook ("ge"));
    $this->assertEquals(1, Filter_Books::interpretBook ("gene"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 Cori"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 cori"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1 corint"));
    $this->assertEquals(46, Filter_Books::interpretBook ("1cor"));
    $this->assertEquals(22, Filter_Books::interpretBook ("song"));
    $this->assertEquals(22, Filter_Books::interpretBook ("song of"));
    $this->assertEquals(22, Filter_Books::interpretBook ("song of sol"));
    $this->assertEquals(11, Filter_Books::interpretBook ("1ki"));
  }


  public function testExplodePassage()
  {
    $this->assertEquals(array (1, 2, 2), Filter_Books::explodePassage ("Genesis 2:2"));
    $this->assertEquals(array (46, 2, 2), Filter_Books::explodePassage ("1 Corinth. 2:2"));
    $this->assertEquals(array (22, 2, 2), Filter_Books::explodePassage ("Song of Sol. 2:2"));
    $this->assertEquals(array (66, 2, 2), Filter_Books::explodePassage ("Revelation 2:2"));
  }
  
  
 
}
?>


