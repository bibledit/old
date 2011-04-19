<?php
require_once 'PHPUnit/Framework.php';
 
class htmlTextTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
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
    @unlink ("/tmp/HtmlTextTest1.html");
    $html_text->save ("/tmp/HtmlTextTest1.html");
    $this->assertFileEquals (dirname (__FILE__) . "/test1.html", "/tmp/HtmlTextTest1.html");
  }


  public function testTwo()
  {
    $html_text = new Html_Text ("TestTwo");
    $html_text->addText ("Should create new paragraph automatically");
    @unlink ("/tmp/HtmlTextTest2");
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
    @unlink ("/tmp/HtmlTextTest3.html");
    $html_text->save ("/tmp/HtmlTextTest3.html");
    $this->assertFileEquals (dirname (__FILE__) . "/test3.html", "/tmp/HtmlTextTest3.html");
  }


}
?>


