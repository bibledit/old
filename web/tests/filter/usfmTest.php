<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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

 
class filterUsfmTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }


  public function tearDown ()
  {
    $GLOBALS[0] = true;
    unset ($GLOBALS[0]);
  }


  public function testGetMarkersAndText()
  {
$usfm = <<<'EOD'
\id GEN\c 10
EOD;
    $output = Filter_Usfm::getMarkersAndText ($usfm);
    $this->assertEquals (array ("\\id ", "GEN", "\\c ", "10"), $output);

$usfm = <<<'EOD'
noise\id GEN\c 10
EOD;
    $output = Filter_Usfm::getMarkersAndText ($usfm);
    $this->assertEquals (array ("noise", "\\id ", "GEN", "\\c ", "10"), $output);

$usfm = <<<'EOD'
\p\v 1 In \add the\add*
EOD;
    $output = Filter_Usfm::getMarkersAndText ($usfm);
    $this->assertEquals (array ("\\p", "\\v ", "1 In ", "\\add ", "the", "\\add*"), $output);

$usfm = <<<'EOD'
\v 2 Text \add of the \add*1st\add second verse\add*.
EOD;
    $output = Filter_Usfm::getMarkersAndText ($usfm);
    $this->assertEquals (array ('\v ', '2 Text ', '\add ', 'of the ', '\add*', '1st', '\add ', 'second verse', '\add*', '.'), $output);

$usfm = <<<'EOD'
\p\v 1 In \+add the\+add*
EOD;
    $output = Filter_Usfm::getMarkersAndText ($usfm);
    $this->assertEquals (array ('\p', '\v ', '1 In ', '\+add ', 'the', '\+add*'), $output);
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
    $this->assertEquals("id", Filter_Usfm::getMarker ('\id GEN'));
    $this->assertEquals("", Filter_Usfm::getMarker (' \id GEN'));
    $this->assertEquals("add", Filter_Usfm::getMarker ('\add insertion'));
    $this->assertEquals("add", Filter_Usfm::getMarker ('\add'));
    $this->assertEquals("add", Filter_Usfm::getMarker ('\add*'));
    $this->assertEquals("add", Filter_Usfm::getMarker ('\add*\add'));
    $this->assertEquals("add", Filter_Usfm::getMarker ('\+add'));
    $this->assertEquals("add", Filter_Usfm::getMarker ('\+add*'));
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

  
  public function testOffset2VerseNumberOne ()
  {
$usfm = <<<'EOD'
\id MIC
EOD;
    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 0));
    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 7));
    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 17));
  }


  public function testOffset2VerseNumberTwo ()
  {
$usfm = <<<'EOD'
\id MIC
\v 1 Verse
EOD;
    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 7));
    $this->assertEquals (1, Filter_Usfm::offset2verseNumber ($usfm, 8));
  }


  public function testOffset2VerseNumberThree ()
  {
$usfm = <<<'EOD'
\v 1 Verse
EOD;
    $this->assertEquals (1, Filter_Usfm::offset2verseNumber ($usfm, 0));
    $this->assertEquals (1, Filter_Usfm::offset2verseNumber ($usfm, 2));
  }

  
  public function testOffset2VerseNumberFour ()
  {
$usfm = <<<'EOD'
\p
\v 3 Verse 3 (out of order).
\v 1 Verse 1.
\v 2 Verse 1.
EOD;
    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 0));
    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 1));

    $this->assertEquals (0, Filter_Usfm::offset2verseNumber ($usfm, 2));
    $this->assertEquals (3, Filter_Usfm::offset2verseNumber ($usfm, 3));
    $this->assertEquals (3, Filter_Usfm::offset2verseNumber ($usfm, 4));

    $this->assertEquals (3, Filter_Usfm::offset2verseNumber ($usfm, 31));
    $this->assertEquals (1, Filter_Usfm::offset2verseNumber ($usfm, 32));
    $this->assertEquals (1, Filter_Usfm::offset2verseNumber ($usfm, 33));

    $this->assertEquals (1, Filter_Usfm::offset2verseNumber ($usfm, 45));
    $this->assertEquals (2, Filter_Usfm::offset2verseNumber ($usfm, 46));
    $this->assertEquals (2, Filter_Usfm::offset2verseNumber ($usfm, 47));
  }
  
  
  public function testVerseNumber2OffsetOne ()
  {
$usfm = <<<'EOD'
\p
\v 1 Verse 1.
\v 2 Verse 2.
\v 3 Verse 3.
EOD;
    $result = Filter_Usfm::verseNumber2offset ($usfm, 1);
    $this->assertEquals (3, $result);
    $result = Filter_Usfm::verseNumber2offset ($usfm, 2);
    $this->assertEquals (17, $result);
    $result = Filter_Usfm::verseNumber2offset ($usfm, 3);
    $this->assertEquals (31, $result);
    $result = Filter_Usfm::verseNumber2offset ($usfm, 4);
    $this->assertEquals (44, $result);
    $result = Filter_Usfm::verseNumber2offset ($usfm, 5);
    $this->assertEquals (44, $result);
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
    $this->assertTrue (Filter_Usfm::isUsfmMarker ('\id'));
    $this->assertTrue (Filter_Usfm::isUsfmMarker ('\c '));
    $this->assertFalse (Filter_Usfm::isUsfmMarker ('c'));
    $this->assertTrue (Filter_Usfm::isUsfmMarker ('\add '));
    $this->assertTrue (Filter_Usfm::isUsfmMarker ('\add*'));
    $this->assertTrue (Filter_Usfm::isUsfmMarker ('\+add*'));
  }

  
  public function testIsOpeningMarker()
  {
    $this->assertTrue (Filter_Usfm::isOpeningMarker ('\id'));
    $this->assertTrue (Filter_Usfm::isOpeningMarker ('\c '));
    $this->assertFalse (Filter_Usfm::isOpeningMarker ('\c*'));
    $this->assertTrue (Filter_Usfm::isOpeningMarker ('\+add '));
    $this->assertFalse (Filter_Usfm::isOpeningMarker ('\+add*'));
  }

  
  public function testIsEmbeddedMarker()
  {
    $this->assertFalse (Filter_Usfm::isEmbeddedMarker ('\add'));
    $this->assertFalse (Filter_Usfm::isEmbeddedMarker ('\add*'));
    $this->assertTrue (Filter_Usfm::isEmbeddedMarker ('\+add'));
    $this->assertTrue (Filter_Usfm::isEmbeddedMarker ('\+add '));
    $this->assertTrue (Filter_Usfm::isEmbeddedMarker ('\+add*'));
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

  
  public function testGetVerseNumbersOne ()
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


  public function testGetVerseNumbersTwo ()
  {
$usfm = <<<'EOD'
\c 80
\s Umkhuleko wokusizwa kukaIsrayeli
\d Kumqondisi wokuhlabelela. NgeShoshanimi. Ubufakazi. Isihlabelelo sikaAsafi
\p
\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa\x + Hlab. 81.5.\x* njengomhlambi\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa\x + Hlab. 50.2.\x*.
\v 2 Phambi kukaEfrayimi loBhenjamini loManase\x + Nani 2.18-23.\x* vusa amandla akho, uze ube lusindiso lwethu\x + Hlab. 35.23.\x*.
\p
\v 3 Siphendule, Nkulunkulu\x + 80.7,14,19. Hlab. 60.1. 85.4. Lilo 5.21.\x*, wenze ubuso bakho bukhanye, ngakho sizasindiswa\x + Nani 6.25. Hlab. 4.6.\x*.
\p
\v 4 N\nd kosi\nd* Nkulunkulu wamabandla\x + Ps 59.5. 84.8.\x*, koze kube nini uthukuthelela umkhuleko wabantu bakho\x + Hlab. 74.10.\x*?
\v 5 Ubenze badla isinkwa sezinyembezi\x + Hlab. 42.3. Hlab. 102.9.\x*, wabanathisa izinyembezi ngesilinganiso\x + Isa. 40.12.\x*.
\v 6 Usenza sibe yingxabano kubomakhelwane bethu, lezitha zethu ziyahlekisana \add ngathi\x + Hlab. 44.13. 79.4.\x*\add*.
\p
\v 7 Siphendule, Nkulunkulu wamabandla, wenze ubuso bakho bukhanye, ngakho sizasindiswa\x + 80.7,14,19. Hlab. 60.1. 85.4. Lilo 5.21.\x*.
\p
\v 8 Waliletha ivini livela eGibhithe\x + Isa. 5.1-7. 27.2. Jer. 2.21. 12.10. Hez. 15.6. 17.6. 19.10. Mat. 21.33. Mark. 12.1. Luka 20.9. Joha. 15.1-6.\x*, wazixotsha izizwe\x + Hlab. 78.55.\x*, walihlanyela lona\x + Hlab. 44.2.\x*.
\v 9 Walungisa \add indawo\add* phambi kwalo\x + Gen. 24.31. Josh. 24.12.\x*, wagxilisa impande zalo, laze lagcwala umhlaba.
\v 10 Izintaba zembeswa ngomthunzi walo, lezingatsha zalo zi\add njenge\add*misedari kaNkulunkulu.
\v 11 Lanabisela ingatsha zalo elwandle\x + Jobe 14.9.\x*, lamahlumela alo\x + Jobe 8.16.\x* emfuleni\x + Hlab. 72.8.\x*.
\v 12 Uyibhobozeleni imiduli yalo\x + Hlab. 89.40,41. Isa. 5.5.\x*, ukuze balikhe bonke abadlula ngendlela\x + Hlab. 89.40,41. Isa. 5.5.\x*?
\v 13 Ingulube yasehlathini iyalihlikiza\x + Jer. 5.6.\x*, lenyamazana yeganga iyalidla\x + Hlab. 50.11.\x*.
\p
\v 14 Nkulunkulu wamabandla, akubuyele\x + 80.3.\x*, ukhangele phansi usemazulwini\x + Isa. 63.15.\x*, ubone, wethekelele lelivini\x + Hlab. 84.9.\x*,
\v 15 ngitsho isivini isandla sakho sokunene esasihlanyelayo, lendodana\x + Gen. 49.22.\x* \add o\add*waziqinisela \add yona\x + 80.17. Isa.44.14.\x*\add*.
\v 16 Sitshisiwe ngomlilo\x + Isa. 33.12.\x*, saqunyelwa phansi\x + Isa. 33.12.\x*; bayabhubha ngokukhuza kobuso bakho\x + Hlab. 76.6. Hlab. 39.11.\x*.
\v 17 Isandla sakho kasibe phezu komuntu\x + Hlab. 89.21. Luka 1.66.\x* wesandla sakho sokunene\x + 80.15.\x*, phezu kwendodana yomuntu\x + Hlab. 8.4,5.\x*, \add o\add*ziqinisele \add yona\x + 80.17. Isa. 44.14.\x*\x + Hlab. 89.21. Luka 1.66.\x*\add*.
\v 18 Ngakho kasiyikubuyela emuva sisuke kuwe; sivuselele, khona sizabiza ibizo lakho\x + Hlab. 71.20.\x*.
\p
\v 19 N\nd kosi\nd*, Nkulunkulu wamabandla, siphendule, wenze ubuso bakho bukhanye, ngakho sizasindiswa\x + 80.3,7.\x*.
EOD;
    $this->assertEquals(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19), Filter_Usfm::getVerseNumbers ($usfm));
  }
  

  public function testGetOpeningUsfm ()
  {
    $this->assertEquals ('\id ', Filter_Usfm::getOpeningUsfm ('id'));
    $this->assertEquals ('\add ', Filter_Usfm::getOpeningUsfm ('add'));
    $this->assertEquals ('\add ', Filter_Usfm::getOpeningUsfm ('add', false));
    $this->assertEquals ('\+add ', Filter_Usfm::getOpeningUsfm ('add', true));
  }


  public function testGetClosingUsfm ()
  {
    $this->assertEquals ('\wj*', Filter_Usfm::getClosingUsfm ('wj'));
    $this->assertEquals ('\add*', Filter_Usfm::getClosingUsfm ('add'));
    $this->assertEquals ('\add*', Filter_Usfm::getClosingUsfm ('add', false));
    $this->assertEquals ('\+add*', Filter_Usfm::getClosingUsfm ('add', true));
  }
  
  
  public function testExtractNotesOne ()
  {
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa\x + Hlab. 81.5.\x* njengomhlambi\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa\x + Hlab. 50.2.\x*.';
    $xrefs = Filter_Usfm::extractNotes ($usfm, array ("x"));
    $standard = array (
      array (55, '\x + Hlab. 81.5.\x*'),
      array (69, '\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $this->assertEquals ($standard, $xrefs);
  }
  
  
  public function testExtractNotesTwo ()
  {
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa\f + Hlab. 81.5.\f* njengomhlambi\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\fe*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa\x + Hlab. 50.2.\x*.';
    $notes = Filter_Usfm::extractNotes ($usfm, array ("x", "f", "fe"));
    $standard = array (
      array (55, '\f + Hlab. 81.5.\f*'),
      array (69, '\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\fe*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $this->assertEquals ($standard, $notes);
  }
  
  
  public function testExtractNotesThree ()
  {
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi\f + Hlab. 81.5.\f*\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\fe*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa\x + Hlab. 50.2.\x*.';
    $notes = Filter_Usfm::extractNotes ($usfm, array ("x", "f", "fe"));
    $standard = array (
      array (69, '\f + Hlab. 81.5.\f*'),
      array (69, '\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\fe*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $this->assertEquals ($standard, $notes);
  }
  
  
  public function testRemoveNotesOne ()
  {
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi\f + Hlab. 81.5.\f*\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\fe*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa\x + Hlab. 50.2.\x*.';
    $usfm = Filter_Usfm::removeNotes ($usfm, array ("x", "f", "fe"));
    $standard = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add* \w kwamakherubhi\w**, khanyisa.';
    $this->assertEquals ($standard, $usfm);
  }
  
  
  public function testInsertNotesEmpty ()
  {
    $usfm = Filter_Usfm::insertNotes (NULL, array (1, ""), 0.9);
    $this->assertNull ($usfm);
    $usfm = Filter_Usfm::insertNotes ("", array (1, ""), 0.9);
    $this->assertEquals ("", $usfm);
    $usfm = Filter_Usfm::insertNotes ('\v 1 Text.', NULL, 0.9);
    $this->assertEquals ('\v 1 Text.', $usfm);
    $usfm = Filter_Usfm::insertNotes ('\v 1 Text.', array (), 0.9);
    $this->assertEquals ('\v 1 Text.', $usfm);
  }


  public function testInsertNotesOne ()
  {
    // Test regular notes placement.
    $usfm = '\v 1 Melusi kaIsrayeli\x + Ps. 1.1\x*, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add* \w kwamakherubhi\w**, khanyisa.';
    $notes = array (
      array (54, '\x + Hlab. 81.5.\x*'),
      array (69, '\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $usfm = Filter_Usfm::insertNotes ($usfm, $notes, 0.9);
    $standard = '\v 1 Melusi kaIsrayeli\x + Ps. 1.1\x*, beka indlebe, okhokhela uJosefa\x + Hlab. 81.5.\x* njengomhlambi\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 50.2.\x*\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa.';
    $this->assertEquals ($standard, $usfm);
  }


  public function testInsertNotesTwo ()
  {
    // Test notes placement where non-placed notes are added to the very end of the string.
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add* \w kwamakherubhi\w**, khanyisa.';
    $notes = array (
      array (55, '\x + Hlab. 81.5.\x*'),
      array (69, '\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $usfm = Filter_Usfm::insertNotes ($usfm, $notes, 1.5);
    $standard = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add*\x + Hlab. 81.5.\x* \w kwamakherubhi\w**\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*, khanyisa\x + Hlab. 50.2.\x*\x + Hlab. 99.1. Eks. 25.22.\x*.';
    $this->assertEquals ($standard, $usfm);
  }
  
  
  public function testMoveNoteOne ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $usfm = Filter_Usfm::moveNote ($usfm, 1, 1);
$standard = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara\x + Judg. 2.16.\x* yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $this->assertEquals ($standard, $usfm);
  }


  public function testMoveNoteTwo ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $usfm = Filter_Usfm::moveNote ($usfm, -1, 2);
$standard = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika\x + Judg. 6.4,5,6.\x*. Umwe murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $this->assertEquals ($standard, $usfm);
  }


  public function testMoveNoteThree ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $usfm = Filter_Usfm::moveNote ($usfm, -1, 10);
$standard = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $this->assertEquals ($standard, $usfm);
  }


  public function testGetNewNotePositionOne ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 5, 0);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 5, 1);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 5, -1);
    $this->assertEquals (4, $position);
  }


  public function testGetNewNotePositionTwo ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 62, 0);
    $this->assertEquals (67, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 62, 1);
    $this->assertEquals (67, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 62, -1);
    $this->assertEquals (59, $position);
  }


  public function testGetNewNotePositionThree ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 0);
    $this->assertEquals (19, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 1);
    $this->assertEquals (28, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, -1);
    $this->assertEquals (10, $position);
  }


  public function testGetNewNotePositionFour ()
  {
$usfm = <<<'EOD'
\v 1 Zvino.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 0);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 1);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, -1);
    $this->assertEquals (10, $position);
  }
  
  
  public function testGetNewNotePositionFive ()
  {
    // It tests whether a note is not inserted straight after opening USFM code.
    $usfm = '\v 1 Zvino namazuva \add okutonga\add* kwavatongi nzara yakange iripo panyika.';

    $position = Filter_Usfm::getNewNotePosition ($usfm, 20, 0);
    $this->assertEquals (38, $position);

    $position = Filter_Usfm::getNewNotePosition ($usfm, 20, 1);
    $this->assertEquals (38, $position);

    $position = Filter_Usfm::getNewNotePosition ($usfm, 31, -1);
    $this->assertEquals (19, $position);
  }


  public function testGetNewNotePositionSix ()
  {
    // It tests whether a note is not inserted within another note.
    $usfm = '\v 1 Zvino namazuva\x + Gen.1.1.\x* okutonga kwavatongi nzara yakange iripo panyika.';

    $position = Filter_Usfm::getNewNotePosition ($usfm, 10, 0);
    $this->assertEquals (10, $position);

    $position = Filter_Usfm::getNewNotePosition ($usfm, 20, 0);
    $this->assertEquals (21, $position);
  }
  
  
}


?>
