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
    @unlink ("/tmp/HtmlTextTest1.html");
    @unlink ("/tmp/HtmlTextTest2.html");
    @unlink ("/tmp/HtmlTextTest3.html");
  }


  public function testOne()
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
    $html_text->save ("/tmp/HtmlTextTest1.html");
    $this->assertFileEquals (dirname (__FILE__) . "/test1.html", "/tmp/HtmlTextTest1.html");
  }


  public function testTwo()
  {
    $html_text = new Html_Text ("TestTwo");
    $html_text->addText ("Should create new paragraph automatically");
    $html_text->save ("/tmp/HtmlTextTest2.html");
    $this->assertFileEquals (dirname (__FILE__) . "/test2.html", "/tmp/HtmlTextTest2.html");
  }


  public function testThree()
  {
    $html_text = new Html_Text ("TestThree");
    $html_text->newParagraph ();
    $html_text->addText ("Text1");
    $html_text->addNote ("†", "");
    $html_text->addNoteText ("Note1.");
    $html_text->addText (".");
    $html_text->save ("/tmp/HtmlTextTest3.html");
    $this->assertFileEquals (dirname (__FILE__) . "/test3.html", "/tmp/HtmlTextTest3.html");
  }


}
?>
