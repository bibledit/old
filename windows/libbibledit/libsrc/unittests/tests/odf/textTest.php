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


class odfdomTextTest extends PHPUnit_Framework_TestCase
{


private $temporary_folder;


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    @unlink ("/tmp/OdfTextTest.odt");
  }


  public function testParagraphs ()
  {
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->createPageBreakStyle ();
    $odf_text->newParagraph ();
    $this->assertEquals ("Standard", $odf_text->currentParagraphStyle);
    $odf_text->addText ("Paragraph One");
    $this->assertEquals ("Paragraph One", $odf_text->currentParagraphContent);
    $odf_text->newParagraph ();
    $this->assertEquals ("", $odf_text->currentParagraphContent);
    $odf_text->addText ("Paragraph Two");
    $this->assertEquals ("Paragraph Two", $odf_text->currentParagraphContent);
    $odf_text->newHeading1 ("Heading One");
    $this->assertEquals ("", $odf_text->currentParagraphContent);
    $odf_text->newPageBreak ();
    $odf_text->newParagraph ();
    $odf_text->addText ("Paragraph Three");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Paragraph One

Paragraph Two

Heading One
===========

Paragraph Three
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testAutomaticParagraph ()
  {
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->addText ("Should create new paragraph automatically");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Should create new paragraph automatically
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testBasicNote ()
  {
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("Text");
    $odf_text->addNote ("†", "");
    $odf_text->addNoteText ("Note");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Text†

Note

.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testBasicFormattedText ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $add = $database_styles->getMarkerData ("Standard", "add");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("text");
    $odf_text->openTextStyle ($add, false, false);
    $odf_text->addText ("add");
    $odf_text->closeTextStyle (false, false);
    $odf_text->addText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
textaddnormal.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testBasicFormattedNote ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $add = $database_styles->getMarkerData ("Standard", "add");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("Text");
    $odf_text->addNote ("𐌰", "f");
    $odf_text->openTextStyle ($add, true, false);
    $odf_text->addNoteText ("Add");
    $odf_text->closeTextStyle (true, false);
    $odf_text->addNoteText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Text𐌰

Addnormal

.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testEmbeddedFormattedText ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $add = array ("marker" => "add", "italic" => ooitOn, "bold" => NULL, "underline" => NULL, "smallcaps" => NULL, "superscript" => false, "color" => "000000");
    $nd = array ("marker" => "nd", "italic" => NULL, "bold" => NULL, "underline" => NULL, "smallcaps" => ooitOn, "superscript" => false, "color" => "000000");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("text");
    $odf_text->openTextStyle ($add, false, false);
    $odf_text->addText ("add");
    $odf_text->openTextStyle ($nd, false, true);
    $odf_text->addText ("nd");
    $odf_text->closeTextStyle (false, false);
    $odf_text->addText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
textaddndnormal.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testEmbeddedFormattedNote ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $add = array ("marker" => "add", "italic" => ooitOn, "bold" => NULL, "underline" => NULL, "smallcaps" => NULL, "superscript" => false, "color" => "000000");
    $nd = array ("marker" => "nd", "italic" => NULL, "bold" => NULL, "underline" => NULL, "smallcaps" => ooitOn, "superscript" => false, "color" => "000000");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("text");
    $odf_text->addNote ("𐌰", "f");
    $odf_text->openTextStyle ($add, true, false);
    $odf_text->addNoteText ("add");
    $odf_text->openTextStyle ($nd, true, true);
    $odf_text->addNoteText ("nd");
    $odf_text->closeTextStyle (true, false);
    $odf_text->addNoteText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
text𐌰

addndnormal

.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


}


?>
