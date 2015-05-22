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

 
class htmlTextTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testParagraphs ()
  {
    $html_text = new Html_Text ("TestOne");
    $html_text->newParagraph ();
    $this->assertEquals ("", $html_text->currentParagraphStyle);
    $html_text->addText ("Paragraph One");
    $this->assertEquals ("Paragraph One", $html_text->currentParagraphContent);
    $html_text->newParagraph ();
    $this->assertEquals ("", $html_text->currentParagraphContent);
    $html_text->addText ("Paragraph Two");
    $this->assertEquals ("Paragraph Two", $html_text->currentParagraphContent);
    $html_text->newHeading1 ("Heading One");
    $this->assertEquals ("", $html_text->currentParagraphContent);
    $html_text->newParagraph ();
    $html_text->addText ("Paragraph Three");
    $html = $html_text->getInnerHtml ();
$standard = <<<'EOD'
<p><span>Paragraph One</span></p>
<p><span>Paragraph Two</span></p>
<h1>Heading One</h1>
<p><span>Paragraph Three</span></p>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }


  public function testAutomaticParagraph ()
  {
    $html_text = new Html_Text ("TestTwo");
    $html_text->addText ("Should create new paragraph automatically");
    $html = $html_text->getInnerHtml ();
$standard = <<<'EOD'
<p><span>Should create new paragraph automatically</span></p>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }


  public function testBasicNote ()
  {
    $html_text = new Html_Text ("TestThree");
    $html_text->newParagraph ();
    $html_text->addText ("Text1");
    $html_text->addNote ("†", "");
    $html_text->addNoteText ("Note1.");
    $html_text->addText (".");
    $html = $html_text->getInnerHtml ();
$standard = <<<'EOD'
<p><span>Text1</span><a href="#note1" id="citation1" class="superscript">†</a><span>.</span></p>
<div><p class=""><a href="#citation1" id="note1">†</a><span> </span><span>Note1.</span></p></div>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }
  
  
  public function testInnerHtml ()
  {
    $html_text = new Html_Text ("test");
    $html_text->newParagraph ();
    $html_text->addText ("Paragraph One");
    $html_text->newParagraph ();
    $html_text->addText ("Paragraph Two");
    $result = $html_text->getInnerHtml ();
$standard = <<<'EOD'
<p><span>Paragraph One</span></p>
<p><span>Paragraph Two</span></p>
EOD;
    $this->assertEquals ($standard, trim ($result));
  }


  public function testBasicFormattedNote ()
  {
    $html_text = new Html_Text ("");
    $html_text->newParagraph ();
    $html_text->addText ("Text");
    $html_text->addNote ("𐌰", "f");
    $html_text->openTextStyle (array ('marker' => "add"), true, false);
    $html_text->addNoteText ("Add");
    $html_text->closeTextStyle (true, false);
    $html_text->addNoteText ("normal");
    $html_text->addText (".");
    $html = $html_text->getInnerHtml ();
$standard = <<<'EOD'
<p><span>Text</span><a href="#note1" id="citation1" class="superscript">𐌰</a><span>.</span></p>
<div><p class="f"><a href="#citation1" id="note1">𐌰</a><span> </span><span class="add">Add</span><span>normal</span></p></div>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }
  
  
  public function testEmbeddedFormattedNote ()
  {
    $html_text = new Html_Text ("");
    $html_text->newParagraph ();
    $html_text->addText ("text");
    $html_text->addNote ("𐌰", "f");
    $html_text->openTextStyle (array ('marker' => "add"), true, false);
    $html_text->addNoteText ("add");
    $html_text->openTextStyle (array ('marker' => "nd"), true, true);
    $html_text->addNoteText ("nd");
    $html_text->closeTextStyle (true, false);
    $html_text->addNoteText ("normal");
    $html_text->addText (".");
    $html = $html_text->getInnerHtml ();
$standard = <<<'EOD'
<p><span>text</span><a href="#note1" id="citation1" class="superscript">𐌰</a><span>.</span></p>
<div><p class="f"><a href="#citation1" id="note1">𐌰</a><span> </span><span class="add">add</span><span class="add nd">nd</span><span>normal</span></p></div>
EOD;
    $this->assertEquals (trim ($standard), trim ($html));
  }
  
  
}


?>
