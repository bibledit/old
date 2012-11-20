<?php


class filterTextTest extends PHPUnit_Framework_TestCase
{


private $temporary_folder;


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {

  }

  
  /**
  * Test extraction of all sorts of information from USFM code
  * Test basic formatting into OpenDocument.
  */
  public function testOne()
  {
$usfm = <<<'EOD'
\id GEN
\h Header
\h1 Header1
\h2 Header2
\h3 Header3
\toc1 The Book of Genesis
\toc2 Genesis
\toc3 Gen
\cl Chapter
\c 1
\cp Ⅰ
\p
\v 1 Text chapter 1
\c 2
\cp ②
\h Header4
\p
\v 2 Text chapter 2
EOD;
    $filter_text = new Filter_Text ("");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    // Check that it finds the running headers.
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'h', 'value' => 'Header'),  $filter_text->runningHeaders[0]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'h1', 'value' => 'Header1'), $filter_text->runningHeaders[1]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'h2', 'value' => 'Header2'), $filter_text->runningHeaders[2]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'h3', 'value' => 'Header3'), $filter_text->runningHeaders[3]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 2, 'verse' => 0, 'marker' => 'h', 'value' => 'Header4'), $filter_text->runningHeaders[4]);
    // Check on Table of Contents items.
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'toc1', 'value' => 'The Book of Genesis'), $filter_text->longTOCs[0]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'toc2', 'value' => 'Genesis'), $filter_text->shortTOCs[0]);
    // Check book abbreviation.
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'toc3', 'value' => 'Gen'), $filter_text->bookAbbreviations[0]);
    // Check chapter specials.
    $this->assertEquals (array ('book' => 1, 'chapter' => 0, 'verse' => 0, 'marker' => 'cl', 'value' => 'Chapter'), $filter_text->chapterLabels[0]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 1, 'verse' => 0, 'marker' => 'cp', 'value' => 'Ⅰ'), $filter_text->publishedChapterMarkers[0]);
    $this->assertEquals (array ('book' => 1, 'chapter' => 2, 'verse' => 0, 'marker' => 'cp', 'value' => '②'), $filter_text->publishedChapterMarkers[1]);
    $this->assertEquals (array (1 => 2), $filter_text->numberOfChaptersPerBook);
    @unlink ("/tmp/TextTest1.odt");
    $filter_text->odf_text_standard->save ("/tmp/TextTest1.odt");
    exec ("odt2txt /tmp/TextTest1.odt", $output, $return_var);
    $this->assertEquals (array ("", "Header4 Ⅰ", "=========", "", "[-- Image: frame1 --]", "", "Ⅰ", "", "Text chapter 1", "", "Header4 ②", "=========", "", "[-- Image: frame2 --]", "", "②", "", "Text chapter 2", ""), $output);
    @unlink ("/tmp/TextTest1.odt");
  }


  /**
  * There are two books here. This normally gives one new page between these two books.
  * Test that basic USFM code gets transformed correctly.
  */
  public function testTwo()
  {
$usfm = <<<'EOD'
\id GEN
\ide XYZ
\c 1
\p Text Genesis 1
\c 2
\p Text Genesis 2
\id MAT
\c 1
\p Text Matthew 1
\c 2
\p Text Matthew 2
\rem Comment
\xxx Unknown markup
EOD;
    $filter_text = new Filter_Text ("");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    @unlink ("/tmp/TextTest2.odt");
    $filter_text->odf_text_standard->save("/tmp/TextTest2.odt");
    exec ("odt2txt /tmp/TextTest2.odt", $output, $return_var);
    $this->assertEquals (array ("", "Genesis 1", "=========", "", "Text Genesis 1", "", "Genesis 2", "=========", "", "Text Genesis 2", "", "Matthew 1", "=========", "", "Text Matthew 1", "", "Matthew 2", "=========", "", "Text Matthew 2", ""), $output);
    $this->assertEquals (array ('Genesis 0:0 Text encoding indicator not supported. Encoding is always in UTF8: \ide XYZ',
                                'Matthew 2:0 Unknown marker \xxx, formatting error: Unknown markup'), $filter_text->fallout);
    $this->assertEquals (array ('Matthew 2:0 Comment: \rem Comment'), $filter_text->info);
    @unlink ("/tmp/TextTest2.odt");
  }


  /**
  * Test transformation of verse numbers and text following.
  */
  public function testThree()
  {
$usfm = <<<'EOD'
\id GEN
\v 1 Verse One.
\p Paragraph One.
\v 2 Verse Two.
\p
\v 3 Verse Three.
\v 4 Verse Four.
\v 5 Verse Five.
EOD;
    $filter_text = new Filter_Text ("");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    @unlink ("/tmp/TextTest3.odt");
    $filter_text->odf_text_standard->save ("/tmp/TextTest3.odt");
    exec ("odt2txt /tmp/TextTest3.odt", $output, $return_var);
    $this->assertEquals (array ("", "1" . Filter_Character::enSpace () . "Verse One.", "", "Paragraph One. 2" . Filter_Character::enSpace () . "Verse Two.", "", "3" . Filter_Character::enSpace () . "Verse Three. 4" . Filter_Character::enSpace () . "Verse Four. 5" . Filter_Character::enSpace () . "Verse Five.", ""), $output);
    @unlink ("/tmp/TextTest3.odt");
  }


  /**
  * Test footnotes and cross references.
  */
  public function testFour()
  {
$usfm = <<<'EOD'
\id GEN
\v 1 Text 1\x + \xt Isa. 1.1.\x*\x - \xt Isa. 2.2.\x*\x + \xt Isa. 3.3.\x*, text 2\f + \fk Word1: \fl Heb. \fq Explanation1.\f*\f + \fk Word2: \fl Heb. \fq Explanation2.\f*, text3.\f + \fk Test: \fl Heb. \fq Note at the very end.\f*
EOD;
    $filter_text = new Filter_Text ("");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    @unlink ("/tmp/TextTest4.odt");
    $filter_text->odf_text_standard->save ("/tmp/TextTest4.odt");
    exec ("odt2txt /tmp/TextTest4.odt", $output, $return_var);
    $this->assertEquals (array ("", "1" . Filter_Character::enSpace () . "Text 1a", "", "Isa. 1.1.", "", "Isa. 2.2.", "", "b", "", "Isa. 3.3.", "", ", text 21", "", "Word1: Heb. Explanation1.", "", "2", "", "Word2: Heb. Explanation2.", "", ", text3.3", "", "Test: Heb. Note at the very end.", ""), $output);
    @unlink ("/tmp/TextTest4.odt");
  }


}


?>
