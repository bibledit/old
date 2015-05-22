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


class filterTextTest extends PHPUnit_Framework_TestCase
{


private $frames_state;



  public function setUp ()
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $this->frames_state = $database_config_bible->getExportChapterDropCapsFrames ("phpunit");
  }

  
  public function tearDown ()
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $database_config_bible->setExportChapterDropCapsFrames ("phpunit", $this->frames_state);
    @unlink ("/tmp/TextTest.odt");
    @unlink ("/tmp/TextTest.html");
  }


  // Test extraction of all sorts of information from USFM code
  // Test basic formatting into OpenDocument.
  public function testOne ()
  {
    $database_config_bible = Database_Config_Bible::getInstance ();
    $database_config_bible->setExportChapterDropCapsFrames ("phpunit", true);
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
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
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
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    exec ("odt2txt /tmp/TextTest.odt", $output, $return_var);
    $this->assertEquals (array ("", "Header4 Ⅰ", "=========", "", "[-- Image: frame1 --]", "", "Ⅰ", "", "Text chapter 1", "", "Header4 ②", "=========", "", "[-- Image: frame2 --]", "", "②", "", "Text chapter 2", ""), $output);
  }


  // There are two books here. This normally gives one new page between these two books.
  // Test that basic USFM code gets transformed correctly.
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
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    exec ("odt2txt /tmp/TextTest.odt", $output, $return_var);
    $this->assertEquals (array ("", "Genesis 1", "=========", "", "Text Genesis 1", "", "Genesis 2", "=========", "", "Text Genesis 2", "", "Matthew 1", "=========", "", "Text Matthew 1", "", "Matthew 2", "=========", "", "Text Matthew 2", ""), $output);
    $this->assertEquals (array ('Genesis 0:0 Text encoding indicator not supported. Encoding is always in UTF8: \ide XYZ',
                                'Matthew 2:0 Unknown marker \xxx, formatting error: Unknown markup'), $filter_text->fallout);
    $this->assertEquals (array ('Matthew 2:0 Comment: \rem Comment'), $filter_text->info);
  }


  // Test transformation of verse numbers and text following.
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
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    exec ("odt2txt /tmp/TextTest.odt", $output, $return_var);
    $this->assertEquals (array ("", "1" . Filter_Character::enSpace () . "Verse One.", "", "Paragraph One. 2" . Filter_Character::enSpace () . "Verse Two.", "", "3" . Filter_Character::enSpace () . "Verse Three. 4" . Filter_Character::enSpace () . "Verse Four. 5" . Filter_Character::enSpace () . "Verse Five.", ""), $output);
  }


  // Test footnotes and cross references.
  public function testNotesOpenDocumentOne ()
  {
$usfm = <<<'EOD'
\id GEN
\v 1 Text 1\x + \xt Isa. 1.1.\x*\x - \xt Isa. 2.2.\x*\x + \xt Isa. 3.3.\x*, text 2\f + \fk Word1: \fl Heb. \fq Explanation1.\f*\f + \fk Word2: \fl Heb. \fq Explanation2.\f*, text3.\f + \fk Test: \fl Heb. \fq Note at the very end.\f*
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    exec ("odt2txt /tmp/TextTest.odt", $output, $return_var);
    $this->assertEquals (array ("", "1" . Filter_Character::enSpace () . "Text 1a", "", "Isa. 1.1.", "", "Isa. 2.2.", "", "b", "", "Isa. 3.3.", "", ", text 21", "", "Word1: Heb. Explanation1.", "", "2", "", "Word2: Heb. Explanation2.", "", ", text3.3", "", "Test: Heb. Note at the very end.", ""), $output);
  }


  public function testClearTextExport1 ()
  {
$usfm = <<<'EOD'
\id GEN
\h Genesis
\toc2 Genesis
\mt2 The book of
\mt Genesis
\c 1
\p
\v 1 In the be\x + \xt Isa. 1.1.\x*\x - \xt Isa. 2.2.\x*ginning, God created\f + \fk Word1: \fl Heb. \fq Explanation1.\f*\f + \fk Word2: \fl Heb. \fq Explanation2.\f* the heavens and the earth.\f + \fk Test: \fl Heb. \fq Note at the very end.\f*
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->text_text = new Text_Text ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->text_text->get ();
$standard = <<<'EOD'
The book of
Genesis
1
1 In the beginning, God created the heavens and the earth.
EOD;
    $this->assertEquals ($output, $standard);
  }


  public function testClearTextExport2 ()
  {
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 1 Chapter 1, verse one.
\v 2 Verse two.
\c 2
\p
\v 1 Chapter 2, verse one.
\v 2 Verse two.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->text_text = new Text_Text ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->text_text->get ();
$standard = <<<'EOD'
1
1 Chapter 1, verse one. 2 Verse two.
2
1 Chapter 2, verse one. 2 Verse two.
EOD;
    $this->assertEquals ($output, $standard);
  }


  public function testVersesHeadingsOne ()
  {
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 1 Verse one.
\v 2 Verse two.
\s Heading one
\p
\v 3 Verse three
\p
\s Heading two
\p
\v 4 Verse four.
\v 5 Verse five.
\c 2
\s Heading three
\p
\v 1 Verse one.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->verses_headings;
    $this->assertEquals ($output, array (2 => "Heading one", 3 => "Heading two", 0 => "Heading three"));
  }


  public function testVersesHeadingsTwo ()
  {
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 1 Verse one.
\s \s Usuku lweN\nd kosi\nd* luyeza masinyane
\p
\v 2 Verse two
\p
\s Heading \add two\add*
\p
\v 3 Verse three
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->verses_headings;
    $this->assertEquals ($output, array (1 => "Usuku lweNkosi luyeza masinyane", 2 => "Heading two"));
  }


  public function testVersesTextOne ()
  {
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 2 Verse\x + \xt Isa. 1.1.\x* two.
\v 3 Verse three\x + \xt Isa. 1.1.\x*.
\s Heading one
\p
\v 4 Verse four.
\p
\s Heading two
\p
\v 5 Verse five.
\v 6 Verse six.
\c 2
\s Heading three
\p
\v 1 Verse one\x + \xt Isa. 1.1.\x*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->getVersesText ();
    $standard = array (
      2 => "Verse two.",
      3 => "Verse three.",
      4 => "Verse four.",
      5 => "Verse five.",
      6 => "Verse six.",
      1 => "Verse one."
    );
    $this->assertEquals ($standard, $output);
  }


  public function testVersesTextTwo ()
  {
$usfm = <<<'EOD'
\c 15
\s Heading
\p
\v 1 He said:
\p I will sing to the Lord.
\v 2 The Lord is my strength.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->getVersesText ();
    $standard = array (
      1 => "He said: I will sing to the Lord.",
      2 => "The Lord is my strength."
    );
    $this->assertEquals ($standard, $output);
  }


  public function testParagraphPositionsOne ()
  {
$usfm = <<<'EOD'
\c 1
\s Heading
\p
\v 1 He said:
\p I will sing to the Lord.
\v 2 The Lord is my strength.
\p I trust in Him.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $output = $filter_text->paragraph_start_positions;
    $standard = array (0, 9, 58);
    $this->assertEquals ($standard, $output);
  }

  
  public function testEmbeddedTextOne ()
  {
$usfm = <<<'EOD'
\c 1
\p
\v 1 He said: I will sing \add to the \+nd Lord\+nd*\add*.
\v 2 The \nd Lord\nd* is my strength.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $text = $filter_text->getVersesText ();
    $standard = array (
      1 => "He said: I will sing to the Lord.",
      2 => "The Lord is my strength."
    );
    $this->assertEquals ($standard, $text);
  }
  
  
  public function testEmbeddedHtmlOne ()
  {
    // Open character style, and embedded character style, and close both normally.
$usfm = <<<'EOD'
\c 1
\p
\v 1 I will sing \add to the \+nd Lord\+nd*\add*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->html_text_standard = new Html_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $html = $filter_text->html_text_standard->getInnerHtml ();
$standard = <<<'EOD'
<p class="p"><span class="v">1</span><span> I will sing </span><span class="add">to the </span><span class="add nd">Lord</span><span>.</span></p>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }
  
  
  public function testEmbeddedHtmlTwo ()
  {
    // Open character style, open embedded character style, close embedded one, then close the outer one.
$usfm = <<<'EOD'
\c 1
\p
\v 1 I will sing \add to the \+nd Lord\+nd* God\add*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->html_text_standard = new Html_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $html = $filter_text->html_text_standard->getInnerHtml ();
$standard = <<<'EOD'
<p class="p"><span class="v">1</span><span> I will sing </span><span class="add">to the </span><span class="add nd">Lord</span><span class="add"> God</span><span>.</span></p>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }
  
  
  public function testEmbeddedHtmlThree ()
  {
    // Open character style, open embedded character style, then closing the outer one closes the embedded one also.
$usfm = <<<'EOD'
\c 1
\p
\v 1 I will sing \add to the \+nd Lord\add*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->html_text_standard = new Html_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $html = $filter_text->html_text_standard->getInnerHtml ();
$standard = <<<'EOD'
<p class="p"><span class="v">1</span><span> I will sing </span><span class="add">to the </span><span class="add nd">Lord</span><span>.</span></p>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }


  public function testEmbeddedOpenDocumentOne ()
  {
    // Open character style, and embedded character style, and close both normally.
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 1 I will sing \add to the \+nd Lord\+nd*\add*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/TextTest.odt");
$standard = <<<'EOD'
Genesis 1
=========

1 I will sing to the Lord.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  // Exercise bits in document to generate text and note citations.
  public function testTextNoteCitationsOpenDocumentOne ()
  {
$usfm = <<<'EOD'
\id GEN
\v 1 Text 1\x + \xt Isa. 1.1.\x* text\f + \fk Word: \fl Heb. \fq Explanation1.\f* text\fe + \fk Word: \fl Heb. \fq Explanation1.\fe*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_text_and_note_citations = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_text_and_note_citations->save ("/tmp/TextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/TextTest.odt");
$standard = <<<'EOD'
1 Text 1a text1 text1.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testEmbeddedOpenDocumentTwo ()
  {
    // Open character style, open embedded character style, close embedded one, then close the outer one.
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 1 I will sing \add to the \+nd Lord\+nd* God\add*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/TextTest.odt");
$standard = <<<'EOD'
Genesis 1
=========

1 I will sing to the Lord God.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testEmbeddedOpenDocumentThree ()
  {
    // Open character style, open embedded character style, then closing the outer one closes the embedded one also.
$usfm = <<<'EOD'
\id GEN
\c 1
\p
\v 1 I will sing \add to the \+nd Lord\add*.
EOD;
    $filter_text = new Filter_Text ("phpunit");
    $filter_text->odf_text_standard = new Odf_Text ("phpunit");
    $filter_text->addUsfmCode ($usfm);
    $filter_text->run ("Standard");
    $filter_text->odf_text_standard->save ("/tmp/TextTest.odt");
    $filter_text->odf_text_standard->save ("/tmp/TextTest2.odt");
    $odt = shell_exec ("odt2txt /tmp/TextTest.odt");
$standard = <<<'EOD'
Genesis 1
=========

1 I will sing to the Lord.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


}


?>
