<?php

 
class usfmTest extends PHPUnit_Framework_TestCase
{

  public function testGetMarkersAndText()
  {
    $this->assertEquals(array("\\id ", "GEN", "\\c ", "10"), Filter_Usfm::getMarkersAndText ("\\id GEN\\c 10"));
    $this->assertEquals(array("noise", "\\id ", "GEN", "\\c ", "10"), Filter_Usfm::getMarkersAndText ("noise\\id GEN\\c 10"));
    $this->assertEquals(array("\\p", "\\v ", "1 In ", "\\add ", "the", "\\add*"), Filter_Usfm::getMarkersAndText ("\\p\\v 1 In \\add the\\add*"));
  }

  public function testOneString()
  {
    $this->assertEquals("", Filter_Usfm::oneString (""));
    $this->assertEquals("\\id GEN", Filter_Usfm::oneString ("\\id GEN\n"));
    $this->assertEquals("\\v 10 text", Filter_Usfm::oneString ("\\v 10\ntext"));
    $this->assertEquals("\\v 10\\v 11", Filter_Usfm::oneString ("\\v 10\n\\v 11"));
    $this->assertEquals("\\v 10 text\\p\\v 11", Filter_Usfm::oneString ("\\v 10 text\n\\p\\v 11"));
  }
  
  public function testGetMarker()
  {
    $this->assertEquals("", Filter_Usfm::getMarker (""));
    $this->assertEquals("id", Filter_Usfm::getMarker ("\\id GEN"));
    $this->assertEquals("", Filter_Usfm::getMarker (" \\id GEN"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add insertion"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add*"));
    $this->assertEquals("add", Filter_Usfm::getMarker ("\\add*\add"));
  }

  public function testImport()
  {
    $this->assertEquals(array(), Filter_Usfm::import ("", "Standard"));
    $this->assertEquals(array(array (33, 0, "\\id MIC"), array (33, 1, "\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.")), Filter_Usfm::import ("\\id MIC\n\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.", "Standard"));
  }
  
  public function testLineNumber2VerseNumber()
  {
$usfm = <<<EOD
\\id MIC
EOD;
    $this->assertEquals (0, Filter_Usfm::lineNumber2VerseNumber ($usfm, 0));
$usfm = <<<EOD
\\id MIC
\\v 1 Verse
EOD;
    $this->assertEquals (1, Filter_Usfm::lineNumber2VerseNumber ($usfm, 1));
$usfm = <<<EOD
\\v 1 Verse
EOD;
    $this->assertEquals (1, Filter_Usfm::lineNumber2VerseNumber ($usfm, 0));
$usfm = <<<EOD
\\p
\\v 3 Verse 3 (out of order).
\\v 1 Verse 1. 
\\v 2 Verse 1.
EOD;
    $this->assertEquals (0, Filter_Usfm::lineNumber2VerseNumber ($usfm, 0));
    $this->assertEquals (3, Filter_Usfm::lineNumber2VerseNumber ($usfm, 1));
    $this->assertEquals (1, Filter_Usfm::lineNumber2VerseNumber ($usfm, 2));
    $this->assertEquals (2, Filter_Usfm::lineNumber2VerseNumber ($usfm, 3));
  }
  
  public function testGetVerseText()
  {
$usfm = <<<EOD
\\p
\\v 1 One
EOD;
    $this->assertEquals ("\\v 1 One", Filter_Usfm::getVerseText ($usfm, 1));
    $this->assertEquals ("\\p", Filter_Usfm::getVerseText ($usfm, 0));
    $this->assertEquals ("", Filter_Usfm::getVerseText ($usfm, 2));
$usfm = <<<'EOD'
\c 1
\s Isibingelelo
\p
\v 1 Umdala
\p
\v 2 Sithandwa
\v 3 Ngoba
\v 4 Kangilantokozo
\s Inkathazo
\p
\v 5 Sithandwa
\v 6 abafakazele
\v 7 Ngoba
\v 8 Ngakho
\p
\v 9 Ngabhalela
\v 10 Ngakho
\p
\v 11 Sithandwa
\v 12 NgoDemetriyu
\s Isicino
\p
\v 13 Bengilezinto
\v 14 kodwa
\p Ukuthula
EOD;
$output = <<<'EOD'
\c 1
\s Isibingelelo
\p
EOD;
    $this->assertEquals ($output, Filter_Usfm::getVerseText ($usfm, 0));
$output = <<<'EOD'
\v 1 Umdala
\p
EOD;
    $this->assertEquals ($output, Filter_Usfm::getVerseText ($usfm, 1));
$output = <<<'EOD'
\v 12 NgoDemetriyu
\s Isicino
\p
EOD;
    $this->assertEquals ($output, Filter_Usfm::getVerseText ($usfm, 12));
  }
  
  public function testIsUsfmMarker()
  {
    $this->assertTrue (Filter_Usfm::isUsfmMarker ("\\id"));
    $this->assertTrue (Filter_Usfm::isUsfmMarker ("\\c "));
    $this->assertFalse (Filter_Usfm::isUsfmMarker ("c"));
  }
  
  public function testIsOpeningMarker()
  {
    $this->assertTrue (Filter_Usfm::isOpeningMarker ("\\id"));
    $this->assertTrue (Filter_Usfm::isOpeningMarker ("\\c "));
    $this->assertFalse (Filter_Usfm::isOpeningMarker ("\\c*"));
  }
  
  public function testGetBookIdentifier()
  {
    $this->assertEquals ("GEN", Filter_Usfm::getBookIdentifier (array ("\\id", "GEN"), 0));
    $this->assertEquals ("XXX", Filter_Usfm::getBookIdentifier (array ("\\id", "GEN"), 1));
    $this->assertEquals ("GE", Filter_Usfm::getBookIdentifier (array ("\\id", "GE"), 0));
    $this->assertEquals ("GEN", Filter_Usfm::getBookIdentifier (array ("\\id", "GENxxx"), 0));
    $this->assertEquals ("GEN", Filter_Usfm::getBookIdentifier (array ("", "GENxxx"), 0));
  }
  
  public function testPeekVerseNumber()
  {
    $this->assertEquals ("1", Filter_Usfm::peekVerseNumber ("1"));
    $this->assertEquals ("1", Filter_Usfm::peekVerseNumber ("1 "));
    $this->assertEquals ("1a", Filter_Usfm::peekVerseNumber ("1a"));
    $this->assertEquals ("2-3", Filter_Usfm::peekVerseNumber ("2-3"));
    $this->assertEquals ("2b,3", Filter_Usfm::peekVerseNumber ("2b,3"));
    $this->assertEquals ("2b,3,", Filter_Usfm::peekVerseNumber ("2b,3, 4"));
    $this->assertEquals ("2a-3b", Filter_Usfm::peekVerseNumber ("2a-3b And he said"));
  }
  
  public function testGetVerseNumbers ()
  {
$usfm = <<<'EOD'
\c 1
\s Isibingelelo
\p
\v 1 Umdala
\p
\v 2 Sithandwa
\v 3 Ngoba
\v 4 Kangilantokozo
\s Inkathazo
\p
\v 5 Sithandwa
\v 6 abafakazele
\v 7 Ngoba
\v 8 Ngakho
\p
\v 9 Ngabhalela
\v 10 Ngakho
\p
\v 11 Sithandwa
\v 12 NgoDemetriyu
\s Isicino
\p
\v 13 Bengilezinto
\v 14 kodwa
\p Ukuthula
EOD;
    $this->assertEquals(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14), Filter_Usfm::getVerseNumbers ($usfm));
  }
  
  
}
?>
