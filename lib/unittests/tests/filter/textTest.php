<?php // Todo C++Port.



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
  $this->assertEquals (array ("", "1" . get_en_space () . "Verse One.", "", "Paragraph One. 2" . get_en_space () . "Verse Two.", "", "3" . get_en_space () . "Verse Three. 4" . get_en_space () . "Verse Four. 5" . get_en_space () . "Verse Five.", ""), $output);
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
  $this->assertEquals (array ("", "1" . get_en_space () . "Text 1a", "", "Isa. 1.1.", "", "Isa. 2.2.", "", "b", "", "Isa. 3.3.", "", ", text 21", "", "Word1: Heb. Explanation1.", "", "2", "", "Word2: Heb. Explanation2.", "", ", text3.3", "", "Test: Heb. Note at the very end.", ""), $output);
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
